/**
* \file
*
* \brief Empty user application template
*
*/

/**
* \mainpage User Application template doxygen documentation
*
* \par Empty user application template
*
* Bare minimum empty user application template
*
* \par Content
*
* -# Include the ASF header files (through asf.h)
* -# Minimal main function that starts with a call to system_init()
* -# "Insert application code here" comment
*
*/

/*
* Include header files for all drivers that have been imported from
* Atmel Software Framework (ASF).
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

#define SLIM_DEBUG

#include <asf.h>
#include <math.h>
#include "DS3231/DS3231.h"
#include "DHT22/DHT22.h"
#include "MAX31856/MAX31856.h"
#include "TimedSleep/TimedSleep.h"
#include "SDI12/SDI12.h"
#include "Dendro/Dendro.h"
#include "SD_FileUtils/SD_FileUtils.h"
#include "LedCodes/LedCodes.h"




//number of loggable values, outside of datetime and sdi-12 values
#define NUM_LOG_VALUES						14
#define LOG_VALUES_TC_BEFORE_INDEX 			0
#define LOG_VALUES_TC_AFTER_INDEX			4
#define LOG_VALUES_DHT1_INDEX				8
#define LOG_VALUES_DHT2_INDEX				10
#define LOG_VALUES_DEND1_INDEX				12
#define LOG_VALUES_DEND2_INDEX				13

#define FLOAT_TO_STR_PRECISION				9

static inline void MainLoop(void);
static inline void RunSapFluxSystem(void);
static inline void RunDht22System(void);
static inline void ReadThermocouples(float *tcValuesOut);
static inline void ReadDendrometers(void);
static inline void RecordDateTime(FIL *dataFile);
static inline void RecordNonSdiValues(FIL *dataFile);
static inline void QueryAndRecordSdiValues(FIL *dataFile);
static inline void InitBodDetection(void);
static inline void WriteDataFileNanOrFloat(float value, FIL *datafile);


struct spi_module spiMasterModule;
static struct spi_slave_inst spiSlaveInstance;
static struct i2c_master_module i2cMasterModule;
static struct adc_module adcModule;
static struct tc_module tcInstance;
static struct LoggerConfig loggerConfig;
FATFS fatFileSys;

#define dateTimeBufferLen  20	//defined as to not variably modify length at file scope.
static char dateTimeBuffer[dateTimeBufferLen] = "\n00/00/2000,00:00:00";	//buffer starts with \n since this always starts a measurement.
static float LogValues[NUM_LOG_VALUES];


int main (void)
{
	//Initialize SAM D20 on-chip hardware
	system_init();
	bod_enable(BOD_BOD33);
	delay_init();
	irq_initialize_vectors();
	cpu_irq_enable();
	
	//set these as output, and keep it that way, otherwise can power when not expecting to.
	PORTA.DIRSET.reg = ALL_POWER_ENABLE | HEATER_MOSFET_PINMASK | SDI_PIN_PINMASK;
	
	InitSleepTimerCounter(&tcInstance);
	
	InitBodDetection();
	//if we just woke up, and we're in brownout, wait 10 minutes until we try to start. This allows
	//a solar panel to gather some energy
	while(bod_is_detected(BOD_BOD33)){
		bod_clear_detected(BOD_BOD33);
		TimedSleepSeconds(&tcInstance, 600);
	}
	bod_disable(BOD_BOD33);
	
	DS3231_init_i2c(&i2cMasterModule);
	ConfigureDendroADC(&adcModule);

	PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;
	
	while(!SdCardInit(&fatFileSys)){
		LedFlashStatusCode(LED_CODE_SD_CARD_NOT_FOUND);
	}
	
	
	//if we can read the time file, set the DS3231 time.
	if(TryReadTimeFile(&dateTimeBuffer[1])){
		DS3231_setTimeFromString(&i2cMasterModule, &dateTimeBuffer[1]);
		f_unlink(SD_TIME_FILENAME);
	}
	int configFileSuccess = ReadConfigFile(&loggerConfig);
	if(!configFileSuccess){
		LedFlashStatusCode(LED_CODE_CONFIG_MISSING);
	}
	SD_CreateWithHeaderIfMissing(&loggerConfig);
	
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	
	ExternalInterruptInit();
	
	PORTA.OUTCLR.reg = PWR_3V3_POWER_ENABLE;
	
	//disable the SD sercom module
	SD_SERCOM_MODULE->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_ENABLE;
	PORTA.OUTCLR.reg = 1 << SD_CS_PIN;
	

	if(loggerConfig.configFlags & CONFIG_FLAGS_START_ON_HOUR){
		//flash success so that the user knows that, even though it's not logging now, it worked.
		LedFlashStatusCode(LED_CODE_START_SUCCESS);
		
		DS3231_setAlarm(&i2cMasterModule, NULL);
		ExternalInterruptSleep();
		DS3231_disableAlarm(&i2cMasterModule);
	}
	

	MainLoop();
}

static inline void MainLoop(void){
	while(1){
		//flash success to show now logging
		LedFlashStatusCode(LED_CODE_START_SUCCESS);
		
		//get the time string from the DS3231. Load it into the buffer, starting at the second index to ignore the starting newline.
		DS3231_getTimeToString(&i2cMasterModule, &dateTimeBuffer[1]);
		
		if(loggerConfig.loggingInterval != 0){
			DS3231_setAlarmFromTime(&i2cMasterModule, loggerConfig.loggingInterval, &dateTimeBuffer[1]);
		}
		
		// run sap flux system, dendrometers, and power up 5v rail to give sensors time to initialize.
		PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;

		//DHT22 goes first because it has 2s built in delay, giving dend and sap flux time to init
		RunDht22System();
		ReadDendrometers();
		RunSapFluxSystem();
		
		//SD card requires 3v3, and SDI-12 requires 3v3 and 5v.
		PORTA.OUTSET.reg = ALL_POWER_ENABLE;
		PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
		//sleep for a second to allow the SDI12 sensors to wake up and initialize if the above sensors were disabled.
		TimedSleepSeconds(&tcInstance, 1);
		
		FIL dataFile;
		//reinitialize the sd card
		PORTA.OUTSET.reg = 1 << SD_CS_PIN;
		SD_SERCOM_MODULE->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
		while(!SdCardInit(&fatFileSys));
		
		bod_enable(BOD_BOD33);
		
		f_open(&dataFile, SD_DATALOG_FILENAME, FA_WRITE);
		f_lseek(&dataFile, f_size(&dataFile));	//append to the end of the file.
		
		RecordDateTime(&dataFile);
		RecordNonSdiValues(&dataFile);
		QueryAndRecordSdiValues(&dataFile);

		f_close(&dataFile);
		
		
		bod_disable(BOD_BOD33);
		bod_clear_detected(BOD_BOD33);
		
		//disable the SD sercom module
		SD_SERCOM_MODULE->SPI.CTRLA.reg &= ~SERCOM_SPI_CTRLA_ENABLE;
		//close everything down, and get ready to sleep.
		PORTA.OUTCLR.reg = ALL_POWER_ENABLE | SDI_PIN_PINMASK | HEATER_MOSFET_PINMASK | DHT22_ALL_PINMASK | TC_MUX_SELECT_ALL_PINMASK | SDI_PIN_PINMASK | LED_PIN_PINMASK | (1 << SD_CS_PIN);
		PORTA.DIRCLR.reg = DHT22_ALL_PINMASK | TC_MUX_SELECT_ALL_PINMASK | LED_PIN_PINMASK ;
		
		if(loggerConfig.loggingInterval != 0){
			EnableExtintWakeup();
			//with the extint wakeup enabled, go to sleep with the Timer/counter as a backup, set one minute later than DS3231 alarm
			TimedSleepSeconds(&tcInstance, (loggerConfig.loggingInterval+1) * 60);
			//turn off the timer backup
			tc_disable(&tcInstance);
			//disable the DS3231 alarm
			DS3231_disableAlarm(&i2cMasterModule);
		}

	}
}

static inline void RunDht22System(void){
	//DHT22 requires at least 2 seconds of power before reading, so sleep here to give them time to think.
	if(loggerConfig.configFlags & (CONFIG_FLAGS_ENABLE_DHT_1 | CONFIG_FLAGS_ENABLE_DHT_2)){
		PORTA.DIRSET.reg = DHT22_1_PINMASK | DHT22_2_PINMASK;
		PORTA.OUTSET.reg = DHT22_1_PINMASK | DHT22_2_PINMASK;
		TimedSleepSeconds(&tcInstance, 2);
	}
	if(loggerConfig.configFlags & (CONFIG_FLAGS_ENABLE_DHT_1)){
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT1_INDEX]), &(LogValues[LOG_VALUES_DHT1_INDEX + 1]), DHT22_1_PINMASK);
	}
	else{
		LogValues[LOG_VALUES_DHT1_INDEX] = NAN;
		LogValues[LOG_VALUES_DHT1_INDEX + 1] = NAN;
	}
	if(loggerConfig.configFlags & (CONFIG_FLAGS_ENABLE_DHT_2)){
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT2_INDEX]), &(LogValues[LOG_VALUES_DHT2_INDEX + 1]), DHT22_2_PINMASK);
	}
	else{
		LogValues[LOG_VALUES_DHT2_INDEX] = NAN;
		LogValues[LOG_VALUES_DHT2_INDEX + 1] = NAN;
	}
	//set back to low power mode
	PORTA.DIRCLR.reg = DHT22_1_PINMASK | DHT22_2_PINMASK;
	PORTA.OUTCLR.reg = DHT22_2_PINMASK | DHT22_1_PINMASK;
}

static inline void RecordDateTime(FIL *dataFile){
	UINT bytesWritten;
	
	if(bod_is_detected(BOD_BOD33)) {
		f_close(dataFile);
	}
	else {
		f_write(dataFile, dateTimeBuffer, dateTimeBufferLen, &bytesWritten);
	}
}

static inline void RecordNonSdiValues(FIL *dataFile){
	for(uint8_t logValueIndex = 0; logValueIndex < NUM_LOG_VALUES; logValueIndex++){
		//at each log value, check for brown out. if it's found, close the file, and leave the function.
		WriteDataFileNanOrFloat(LogValues[logValueIndex], dataFile);
	}
	f_sync(dataFile);
}

static inline void QueryAndRecordSdiValues(FIL *dataFile){
	if(loggerConfig.configFlags & CONFIG_FLAGS_ENABLE_SDI){
		for(uint8_t sdiIndex = 0; sdiIndex < loggerConfig.numSdiSensors; sdiIndex++){
			
			//create the array for the values, and initialize the values to NAN.
			const uint8_t numValsForSensor = loggerConfig.SDI12_SensorNumValues[sdiIndex];
			float sdiValuesForSensor[numValsForSensor];
			for(uint8_t i = 0; i< numValsForSensor;i++){
				sdiValuesForSensor[i] = NAN;
			}

			struct SDI_transactionPacket transactionPacket;
			transactionPacket.address = loggerConfig.SDI12_SensorAddresses[sdiIndex];
			SDI12_RequestSensorReading(&transactionPacket);
			if(transactionPacket.transactionStatus == SDI12_STATUS_OK){
				//if the sensor asked us to wait for some time before reading, let's go into sleep mode for it.
				if(transactionPacket.waitTime > 0){
					//TimedSleepSeconds(&tcInstance, transactionPacket.waitTime);
					TimedSleepSeconds(&tcInstance, transactionPacket.waitTime + 1);
				}
				SDI12_GetSensedValues(&transactionPacket, sdiValuesForSensor);
			}
			
			for(uint8_t i=0; i< numValsForSensor;i++){
				//check for brownout before writing to SD card.
				WriteDataFileNanOrFloat(sdiValuesForSensor[i], dataFile);
			}
			//write all work so far to the datafile.
			f_sync(dataFile);
		}
	}
	
	else{
		//fill the spaces with NANs since we're not reading them.
		for(uint8_t sdiIndex = 0; sdiIndex < loggerConfig.numSdiSensors; sdiIndex++){
			for(uint8_t sdiValIndex = 0; sdiValIndex < loggerConfig.SDI12_SensorNumValues[sdiIndex];sdiValIndex++){
				f_puts(",NAN", dataFile);
			}
		}
	}
}

static inline void RunSapFluxSystem(void){
	if(loggerConfig.configFlags & CONFIG_FLAGS_ENABLE_SAP_FLUX){
		//read the starting values for the thermocouples
		ReadThermocouples(&(LogValues[LOG_VALUES_TC_BEFORE_INDEX]));
		
		//turn on heater, and turn off dendro/tc/dht. Then, sleep for the heater duration.
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | PWR_3V3_POWER_ENABLE;
		
		//go to sleep for the full heater period, but wake up a second beforehand to turn on the power to the amp
		TimedSleepSeconds(&tcInstance, HEATER_TIMED_SLEEP_SECONDS - 1);
		PORTA.OUTTGL.reg = PWR_3V3_POWER_ENABLE;
		TimedSleepSeconds(&tcInstance, 1);
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK;
		
		
		ReadThermocouples(&(LogValues[LOG_VALUES_TC_AFTER_INDEX]));
	}
	
	else{
		//if sap flux is disabled, just write NANs into the values
		for(uint8_t index = 0; index < 8; index++){
			LogValues[index] = NAN;
		}
	}
}

static inline void ReadThermocouples(float *tcValuesOut){
	
	//enable the spi module
	spi_enable(&spiMasterModule);
	//start by configuring the registers to the required values.
	Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, loggerConfig.thermocoupleType);
	
	//set both mux pins to low output
	PORTA.DIRSET.reg = TC_MUX_SELECT_ALL_PINMASK;
	PORTA.OUTCLR.reg = TC_MUX_SELECT_ALL_PINMASK;
	
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		enum Max31856_Status requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		if(requestStatus == MAX31856_OKAY){
			//enter standby mode until the reading has been prepared (a bit under 1s)
			TimedSleepSeconds(&tcInstance, 1);
			//if successful, Max31856GetTemp will set the out value to the temperature. Otherwise, it will be NAN.
			float tempValue;
			Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &tempValue);
			tcValuesOut[index] = tempValue;
		}
		else{
			tcValuesOut[index] = NAN;
		}
		PORTA.OUTTGL.reg = (index & 1)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
	//make sure the mux pins are low, and set to low power input.
	PORTA.OUTCLR.reg = TC_MUX_SELECT_ALL_PINMASK;
	PORTA.DIRCLR.reg = TC_MUX_SELECT_ALL_PINMASK;
	
	spi_disable(&spiMasterModule);
}

static inline void ReadDendrometers(void){
	if(loggerConfig.configFlags & CONFIG_FLAGS_ENABLE_DEND_1){
		LogValues[LOG_VALUES_DEND1_INDEX]	= ReadDendro(&adcModule, DEND_ANALOG_PIN_1);
	}
	else {
		LogValues[LOG_VALUES_DEND1_INDEX] = NAN;
	}
	
	if(loggerConfig.configFlags & CONFIG_FLAGS_ENABLE_DEND_2){
		LogValues[LOG_VALUES_DEND2_INDEX]	= ReadDendro(&adcModule, DEND_ANALOG_PIN_2);
	}
	else{
		LogValues[LOG_VALUES_DEND2_INDEX] = NAN;
	}
}

static inline void InitBodDetection(void){
	struct bod_config bodConfig;
	bod_get_config_defaults(&bodConfig);
	//level is set to 39, sampling is continuous, and hysteresis to true by default
	bodConfig.action = BOD_ACTION_NONE;
	bodConfig.run_in_standby = false;
	bod_set_config(BOD_BOD33, &bodConfig);
	bod_disable(BOD_BOD33);
}

static inline void WriteDataFileNanOrFloat(float value, FIL *datafile){
	if(!bod_is_detected(BOD_BOD33)){
		f_putc(',', datafile);
		if(isnan(value)){
			f_puts("NAN",datafile);
		}
		else{
			char parseBuffer[24];
			gcvtf(value, FLOAT_TO_STR_PRECISION, parseBuffer);
			f_puts(parseBuffer, datafile);
		}
	}
	else{
		f_close(datafile);
	}
}