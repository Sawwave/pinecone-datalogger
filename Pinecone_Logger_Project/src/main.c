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
#include "DS1302/DS1302.h"
#include "DHT22/DHT22.h"
#include "MAX31856/MAX31856.h"
#include "TimedSleep/TimedSleep.h"
#include "SDI12/SDI12.h"
#include "Dendro/Dendro.h"
#include "SD_FileUtils/SD_FileUtils.h"

//number of loggable values, outside of datetime and sdi-12 values
#define NUM_LOG_VALUES						14
#define LOG_VALUES_TC_BEFORE_INDEX			0
#define LOG_VALUES_TC_AFTER_INDEX			4
#define LOG_VALUES_DHT_INDEX				8
#define LOG_VALUES_DEND_INDEX				12

#define FLOAT_TO_STR_PRECISION				9

static inline void MainLoop(void);
static inline void RunSapFluxSystem(void);
static inline void ReadThermocouples(float *tcValuesOut);
static inline void ReadDendrometers(void);
static inline void RecordDateTime(FIL *dataFile);
static inline void RecordNonSdiValues(FIL *dataFile);
static inline void QueryAndRecordSdiValues(FIL *dataFile);
static inline void InitBodDetection(void);
static inline void WriteDataFileNanOrFloat(float value, FIL *datafile);

static struct spi_module spiMasterModule;
static struct spi_slave_inst spiSlaveInstance;
static struct adc_module adcModule;
static struct tc_module tcInstance;
static struct LoggerConfig loggerConfig;
static FATFS fatFileSys;

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
	
	//if we just woke up, and we're in brownout, wait 10 minutes until we try to start. This allows
	//a solar panel to gather some energy
	while(bod_is_detected(BOD_BOD33)){
		TimedSleepSeconds(600);
	}
	
	InitSleepTimerCounter(&tcInstance);
	InitBodDetection();
	ConfigureDendroADC(&adcModule);
	
	//wake up the SD card
	PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;
	SdCardInit(&fatFileSys);
	TryReadTimeFile();
	ReadConfigFile(&loggerConfig);
	
	/*If the configuration is set to defer logging for one sleep cycle, accomplish that sleep here.*/
	if(!loggerConfig.logImmediately){
		PORTA.OUTCLR.reg = ALL_MOSFET_PINMASK;
		TimedSleepSeconds(&tcInstance, loggerConfig.loggingInterval);
	}
	
	
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	
	
	PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;
	SD_CreateWithHeaderIfMissing(&loggerConfig);
	
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	PORTA.OUTCLR.reg = ALL_MOSFET_PINMASK;
	
	MainLoop();
}

static inline void MainLoop(void){
	while(1){		
		PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;
		RunSapFluxSystem();
		ReadDendrometers();
		PORTA.OUTTGL.reg = PWR_3V3_POWER_ENABLE | PWR_5V_POWER_ENABLE;
		//sleep 2s, required for the DHT22 to function properly.
		TimedSleepSeconds(&tcInstance, 2);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX]), &(LogValues[LOG_VALUES_DHT_INDEX + 1]), DHT22_1_PINMASK);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX + 2] ), &(LogValues[LOG_VALUES_DHT_INDEX + 3]), DHT22_2_PINMASK);

		//turn off the power to the SDI12 bus, the DHT22s, and stop sending HIGH on the DHT22 data lines.
		PORTA.OUTCLR.reg = ALL_POWER_ENALBE;
		Ds1302GetDateTime(dateTimeBuffer);
		PORTA.OUTSET.reg = PWR_3V3_POWER_ENABLE;
		
		FIL dataFile;
		bod_enable(BOD_BOD33);
		f_open(&dataFile, SD_DATALOG_FILENAME, FA_WRITE);
		f_lseek(&dataFile, f_size(&dataFile));	//append to the end of the file.
		
		RecordDateTime(&dataFile);
		RecordNonSdiValues(&dataFile);
		QueryAndRecordSdiValues(&dataFile);
		f_close(&dataFile);
		
		bod_disable(BOD_BOD33);
		bod_clear_detected(BOD_BOD33);
		
		PORTA.OUTCLR.reg = ALL_POWER_ENALBE;
		
		TimedSleepSeconds(&tcInstance, loggerConfig.loggingInterval);
	}
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

static inline void RunSapFluxSystem(void){
	//read the starting values for the thermocouples
	ReadThermocouples(&(LogValues[LOG_VALUES_TC_BEFORE_INDEX]));
	
	//turn on heater, and turn off dendro/tc. Then, sleep for the heater duration.
	PORTA.OUTSET.reg = HEATER_MOSFET_PINMASK;
	TimedSleepSeconds(&tcInstance, HEATER_TIMED_SLEEP_SECONDS);
	//turn heater off, and dendro/tc back on.
	PORTA.OUTCLR.reg = HEATER_MOSFET_PINMASK;
	
	ReadThermocouples(&(LogValues[LOG_VALUES_TC_AFTER_INDEX]));
}

static inline void ReadThermocouples(float *tcValuesOut){
	
	//enable the spi module
	spi_enable(&spiMasterModule);
	//start by configuring the registers to the required values.
	Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, loggerConfig.thermocoupleType);
	
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		enum Max31856_Status requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		if(requestStatus == MAX31856_OKAY){
			//enter standby mode until the reading has been prepared (a bit under 1s)
			TimedSleepSeconds(&tcInstance, 1);
			//if successful, Max31856GetTemp will set the out value to the temperature. Otherwise, it will be NAN.
			Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &(tcValuesOut[index]));
		}
		else{
			tcValuesOut[index] = NAN;
		}
		PORTA.OUTTGL.reg = ((index & 1) != 0)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
	spi_disable(&spiMasterModule);
}

static inline void ReadDendrometers(void){
	LogValues[LOG_VALUES_DEND_INDEX]		= ReadDendro(&adcModule, DEND_ANALOG_PIN_1);
	adc_flush(&adcModule);
	LogValues[LOG_VALUES_DEND_INDEX + 1]	= ReadDendro(&adcModule, DEND_ANALOG_PIN_2);
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
			//parse buffer is static so we don't have to realloc every time. It's gonna get overwritten by gcvtf anyway.
			static char parseBuffer[24];
			gcvtf(value, FLOAT_TO_STR_PRECISION, parseBuffer);
			f_puts(parseBuffer, datafile);
		}
	}
	else{
		f_close(datafile);
	}
}