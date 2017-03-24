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

static inline void DEBUG_LOOP(void);

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
	
	PORTA.DIRSET.reg = PWR_3V3_POWER_ENABLE;
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
	
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	PORTA.OUTCLR.reg = ALL_POWER_ENABLE;
	PORTA.DIRCLR.reg = ALL_POWER_ENABLE;
	
	ExternalInterruptInit();
	
	if(loggerConfig.configFlags & CONFIG_FLAGS_START_ON_HOUR){
		//flash success so that the user knows that, even though it's not logging now, it worked.
		LedFlashStatusCode(LED_CODE_START_SUCCESS);
		
		DS3231_setAlarm(&i2cMasterModule, NULL);
		ExternalInterruptSleep();
	}
	
	MainLoop();
}

static inline void MainLoop(void){
	while(1){
		
		//flash success to show now logging
		LedFlashStatusCode(LED_CODE_START_SUCCESS);
		
		//get the time string from the DS3231. Load it into the buffer, starting at the second index to ignore the starting newline.
		DS3231_getTimeToString(&i2cMasterModule, &dateTimeBuffer[1]);
		
		// run sap flux system, dendrometers
		PORTA.DIRSET.reg = PWR_3V3_POWER_ENABLE;
		PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;

		//DHT22 goes first because it has 2s built in delay, giving dend and sap flux time to init
		RunDht22System();
		ReadDendrometers();
		RunSapFluxSystem();
		
		//SD card requires 3v3, and SDI-12 requires 3v3 and 5v.
		PORTA.DIRSET.reg = PWR_5V_POWER_ENABLE | PWR_5V_POWER_ENABLE;
		PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE | PWR_5V_POWER_ENABLE;
		
		FIL dataFile;
		bod_enable(BOD_BOD33);
		delay_ms(10);
		f_open(&dataFile, SD_DATALOG_FILENAME, FA_WRITE);
		f_lseek(&dataFile, f_size(&dataFile));	//append to the end of the file.
		
		RecordDateTime(&dataFile);
		RecordNonSdiValues(&dataFile);
		QueryAndRecordSdiValues(&dataFile);
		f_close(&dataFile);
		
		//close everything down, and get ready to sleep.
		PORTA.OUTCLR.reg = ALL_POWER_ENABLE;
		PORTA.DIRCLR.reg = ALL_POWER_ENABLE;
		bod_disable(BOD_BOD33);
		bod_clear_detected(BOD_BOD33);
		
		struct Ds3231_alarmTime alarm;
		DS3231_createAlarmTime(&dateTimeBuffer[1], loggerConfig.loggingInterval, &alarm);
		DS3231_setAlarm(&i2cMasterModule, &alarm);
		PORTA.OUTCLR.reg = ALL_GPIO_PINMASK;
		PORTA.DIRCLR.reg = ALL_GPIO_PINMASK;
		ExternalInterruptSleep();
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
	}
	if(loggerConfig.configFlags & (CONFIG_FLAGS_ENABLE_DHT_2)){
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT2_INDEX]), &(LogValues[LOG_VALUES_DHT2_INDEX + 1]), DHT22_2_PINMASK);
	}
	else{
		LogValues[LOG_VALUES_DHT2_INDEX] = NAN;
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
			float sdiValuesForSensor[loggerConfig.SDI12_SensorNumValues[sdiIndex]];
			for(uint8_t i = 0; i< loggerConfig.SDI12_SensorNumValues[sdiIndex];i++){
				sdiValuesForSensor[i] = NAN;
			}
			
			struct SDI_transactionPacket transactionPacket;
			transactionPacket.address = loggerConfig.SDI12_SensorAddresses[sdiIndex];

			SDI12_RequestSensorReading(&transactionPacket);
			if(transactionPacket.transactionStatus == SDI12_STATUS_OK){
				//if the sensor asked us to wait for some time before reading, let's go into sleep mode for it.
				if(transactionPacket.waitTime > 0){
					TimedSleepSeconds(&tcInstance, transactionPacket.waitTime);
				}
				SDI12_GetSensedValues(&transactionPacket, sdiValuesForSensor);
			}
			
			for(uint8_t i=0; i< loggerConfig.SDI12_SensorNumValues[sdiIndex];i++){
				//check for brownout before writing to SD card.
				WriteDataFileNanOrFloat(sdiValuesForSensor[i], dataFile);
			}
			f_sync(dataFile);
		}
	}
	
	else{
		for(uint8_t sdiIndex = 0; sdiIndex < loggerConfig.numSdiSensors; sdiIndex++){
			f_puts(",NAN", dataFile);
		}
	}
}

static inline void RunSapFluxSystem(void){
	if(loggerConfig.configFlags & CONFIG_FLAGS_ENABLE_SAP_FLUX){
		//read the starting values for the thermocouples
		ReadThermocouples(&(LogValues[LOG_VALUES_TC_BEFORE_INDEX]));
		
		//turn on heater, and turn off dendro/tc/dht. Then, sleep for the heater duration.
		PORTA.DIRSET.reg = HEATER_MOSFET_PINMASK;
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | PWR_3V3_POWER_ENABLE;
		TimedSleepSeconds(&tcInstance, HEATER_TIMED_SLEEP_SECONDS);
		//turn heater off, and dendro/tc back on.
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | PWR_3V3_POWER_ENABLE;
		PORTA.DIRCLR.reg = HEATER_MOSFET_PINMASK;
		
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

static inline void DEBUG_LOOP(void){
	
	PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;
	while(1){
		ReadThermocouples(LogValues);
		FIL datafile;
		FRESULT status = f_open(&datafile, "tcvals.txt",  FA_OPEN_ALWAYS | FA_WRITE);
		f_lseek(&datafile, f_size(&datafile));	//append to the end of the file.
		static char parseBuffer[24];
		if(status == FR_OK){
			for(uint8_t i = 0;i<4;i++){
				if(isnan(LogValues[i])){
					f_puts("NAN, ",&datafile);
				}
				else{
					gcvtf(LogValues[i], FLOAT_TO_STR_PRECISION, parseBuffer);
					f_puts(parseBuffer, &datafile);
					f_puts(", ", &datafile);
				}
			}
			f_puts("\n", &datafile);
			f_close(&datafile);
		}
		delay_s(1);
	}
	PORTA.OUTCLR.reg = PWR_3V3_POWER_ENABLE;
	
}