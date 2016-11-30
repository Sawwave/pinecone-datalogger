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


static void MainLoop(void);
static void runSapFluxSystem(void);

static void ReadThermocouples(float *tcValuesOut);
static void ReadDendrometers(void);


static struct spi_module spiMasterModule;
static struct spi_slave_inst spiSlaveInstance;
static struct adc_module adcModule1;
static struct adc_module adcModule2;
static struct tc_module tcInstance;
static FATFS fatFileSys;

static float LogValues[NUM_LOG_VALUES];
#define dateTimeBufferLen  21			//defined as to not variably modify length at file scope.
static char dateTimeBuffer[dateTimeBufferLen] = "\n00/00/2000,00:00:00";	//buffer starts with \n since this always starts a measurement.

static struct LoggerConfig loggerConfig;

int main (void)
{
	//Initialize SAM D20 on-chip hardware
	system_init();
	delay_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	initSleepTimerCounter(&tcInstance);
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	ConfigureDendroADC(&adcModule1, DEND_ANALOG_PIN_1);
	ConfigureDendroADC(&adcModule2, DEND_ANALOG_PIN_2);

	//wake up the SD card
	PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;
	SdCardInit(&fatFileSys);
	tryReadTimeFile();
	readConfigFile(&loggerConfig);
	SD_CreateWithHeaderIfMissing(&loggerConfig);
	
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	PORTA.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;

	/*If the configuration is set to defer logging for one sleep cycle, accomplish that sleep here.*/
	if(!loggerConfig.logImmediately){
		timedSleep_seconds(&tcInstance, loggerConfig.loggingInterval);
	}

	MainLoop();
}

static void MainLoop(void){

	FIL dataFile;
	f_open(&dataFile, SD_DATALOG_FILENAME, FA_WRITE);
	
	/*All initialization has been done, so enter the loop!*/
	while(1){
		PORTA.OUTSET.reg = DENDRO_TC_AMP_MOSFET_PINMASK;
		runSapFluxSystem();
		ReadDendrometers();
		
		//turn of dendro/tc, and turn on SDI-12 bus and DHT22s. mark the DHT22 data pins as HIGH to start, too.
		PORTA.OUTTGL.reg = DENDRO_TC_AMP_MOSFET_PINMASK | SDI_DHT22_POWER_MOSFET_PINMASK | DHT22_ALL_PINMASK;
		//sleep 2s, required for the DHT22 to function properly.
		timedSleep_seconds(&tcInstance, 2);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX]), &(LogValues[LOG_VALUES_DHT_INDEX + 1]), DHT22_1_PINMASK);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX + 2] ), &(LogValues[LOG_VALUES_DHT_INDEX + 3]), DHT22_2_PINMASK);

		//turn off the power to the SDI12 bus, the DHT22s, and stop sending HIGH on the DHT22 data lines.
		PORTA.OUTCLR.reg = SDI_DHT22_POWER_MOSFET_PINMASK | DHT22_ALL_PINMASK;
		
		Ds1302GetDateTime(dateTimeBuffer);
		
		PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;

		UINT bytesWritten;
		f_open(&dataFile,SD_DATALOG_FILENAME, FA_WRITE);
		f_write(&dataFile, dateTimeBuffer, dateTimeBufferLen, &bytesWritten);
		for(uint8_t logValueIndex = 0; logValueIndex < NUM_LOG_VALUES; logValueIndex++){
			f_printf(&dataFile, ",%f",LogValues[logValueIndex]);
		}
		f_sync(&dataFile);
		
		for(uint8_t sdiIndex = 0; sdiIndex< loggerConfig.numSdiSensors; sdiIndex++){
			bool success = false;
			float sdiValuesForSensor[loggerConfig.SDI12_SensorNumValues[sdiIndex]];
			struct SDI_transactionPacket transactionPacket;
			transactionPacket.address = loggerConfig.SDI12_SensorAddresses[sdiIndex];

			SDI12_RequestSensorReading(&transactionPacket);
			if(transactionPacket.transactionStatus == SDI12_STATUS_OK){
				//if the sensor asked us to wait for some time before reading, let's go into sleep mode for it.
				if(transactionPacket.waitTime > 0){
					timedSleep_seconds(&tcInstance, transactionPacket.waitTime);
				}
				success = SDI12_GetSensedValues(&transactionPacket, sdiValuesForSensor);
			}
			for(uint8_t i=0; i< loggerConfig.SDI12_SensorNumValues[sdiIndex];i++){
				if(success)
				f_write(&dataFile, ",NAN", 4, &bytesWritten);
				else
				f_printf(&dataFile, ",%f", sdiValuesForSensor[i]);
			}
			f_sync(&dataFile);
		}
		PORTA.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;

		timedSleep_seconds(&tcInstance, loggerConfig.loggingInterval);
	}
}

static void runSapFluxSystem(void){
	//read the starting values for the thermocouples
	ReadThermocouples(&(LogValues[LOG_VALUES_TC_BEFORE_INDEX]));
	
	//turn on heater, and turn off dendro/tc. Then, sleep for the heater duration.
	PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | DENDRO_TC_AMP_MOSFET_PINMASK;
	timedSleep_seconds(&tcInstance, HEATER_TIMED_SLEEP_SECONDS);
	//turn heater off, and dendro/tc back on.
	PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | DENDRO_TC_AMP_MOSFET_PINMASK;
	
	ReadThermocouples(&(LogValues[LOG_VALUES_TC_AFTER_INDEX]));
}

static void ReadThermocouples(float *tcValuesOut){
	//start by configuring the registers to the required values.
	Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, MAX31856_THERMOCOUPLE_TYPE_USED);
	
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		enum Max31856_Status requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		if(requestStatus == MAX31856_OKAY){
			//enter standby mode until the reading has been prepared (a bit under 1s)
			timedSleep_seconds(&tcInstance, 1);
			//if successful, Max31856GetTemp will set the out value to the temperature. Otherwise, it will be NAN.
			Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &(tcValuesOut[index]));
		}
		else{
			tcValuesOut[index] = NAN;
		}
		PORTA.OUTTGL.reg = ((index & 1) != 0)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
}

static void ReadDendrometers(void){
	LogValues[LOG_VALUES_DEND_INDEX]		= ReadDendro(&adcModule1);
	LogValues[LOG_VALUES_DEND_INDEX + 1]	= ReadDendro(&adcModule2);
}