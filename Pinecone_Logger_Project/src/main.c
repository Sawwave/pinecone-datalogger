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
static void LoggerInit(void);
static void runSapFluxSystem(void);

static void ReadThermocouples(double *tcValuesOut);
//static bool MAX31856_VOLATILE_REGISTERS_TEST(void);

static void ReadDendrometers(void);
static void LogAllSdiSensors(float *sdiValuesArray);
static void WriteValuesToSD(float *sdiValuesArray);


struct spi_module spiMasterModule;
struct spi_slave_inst spiSlaveInstance;
struct adc_module adcModule1;
struct adc_module adcModule2;
struct tc_module tcInstance;
FATFS fatFileSys;

static double LogValues[NUM_LOG_VALUES];

static char dateTimeBuffer[20] = "00/00/2000,00:00:00";

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
	SdCardInit();
	tryReadTimeFile();
	readConfigFile(&loggerConfig);
	SD_CreateWithHeaderIfMissing(&loggerConfig);
	
	while(1);
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	PORTA.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;

	LoggerInit();

	MainLoop();
}

static void MainLoop(void){
	//sdiValues is defined here because the length of the array determined by config file, not a compile time constant.
	float sdiValues[loggerConfig.totalSDI_12Values];
	/*All initialization has been done, so enter the loop!*/
	while(1){
		PORTA.OUTSET.reg = DENDRO_TC_AMP_MOSFET_PINMASK;
		runSapFluxSystem();
		ReadDendrometers();
		
		//turn of dendr/tc, and turn on SDI-12 bus and DHT22s. mark the DHT22 data pins as HIGH to start, too.
		PORTA.OUTTGL.reg = DENDRO_TC_AMP_MOSFET_PINMASK | SDI_DHT22_POWER_MOSFET_PINMASK | DHT22_ALL_PINMASK;
		//sleep 2s, required for the DHT22 to function properly.
		timedSleep_seconds(&tcInstance, 2);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX]), &(LogValues[LOG_VALUES_DHT_INDEX + 1]), DHT22_1_PINMASK);
		GetDht22Reading(&(LogValues[LOG_VALUES_DHT_INDEX + 2] ), &(LogValues[LOG_VALUES_DHT_INDEX + 3]), DHT22_2_PINMASK);

		LogAllSdiSensors(sdiValues);
		
		//turn off the power to the SDI12 bus, the DHT22s, and stop sending HIGH on the DHT22 data lines.
		PORTA.OUTCLR.reg = SDI_DHT22_POWER_MOSFET_PINMASK | DHT22_ALL_PINMASK;
		
		Ds1302GetDateTime(dateTimeBuffer);
		
		WriteValuesToSD(sdiValues);
		
		timedSleep_seconds(&tcInstance, loggerConfig.loggingInterval);
	}
}

/*LoggerInit
Main initialization of board state and external hardware components before the main loop begins.*/
static void LoggerInit(void){
	
	//count up the number of addresses in the loggerConfig's SDI_12 address list. Consider a null terminator, CR, or LF to be terminating.
	loggerConfig.numSdiSensors = 0;
	loggerConfig.totalSDI_12Values = 0;
	while(loggerConfig.SDI12_SensorAddresses[(loggerConfig.numSdiSensors)] != 0 &&
	loggerConfig.SDI12_SensorAddresses[(loggerConfig.numSdiSensors)] != 10 &&
	loggerConfig.SDI12_SensorAddresses[(loggerConfig.numSdiSensors)] != 13){
		//query the sensor for the num values it has
		loggerConfig.SDI12_SensorNumValues[loggerConfig.numSdiSensors] = SDI12_GetNumReadingsFromSensorMetadata(loggerConfig.SDI12_SensorAddresses[loggerConfig.numSdiSensors]);
		loggerConfig.totalSDI_12Values += loggerConfig.SDI12_SensorNumValues[loggerConfig.numSdiSensors];
		loggerConfig.numSdiSensors++;
	}
	
	//check the DS1302 general purpose register to see if we might need to fix CSV integrity.
	uint8_t Ds1302StoredRegister = Ds1302GetBatteryBackedRegister(DS1302_GENERAL_PURPOSE_DATA_REGISTER_0);
	if(Ds1302StoredRegister & 0x1){
		SD_CheckIntegrity(&loggerConfig);
		//clear out the value in the Ds1302 register
		Ds1302SetBatteryBackedRegister(DS1302_GENERAL_PURPOSE_DATA_REGISTER_0, 0);
	}

	/*If the configuration is set to defer logging for one sleep cycle, accomplish that sleep here.*/
	if(!loggerConfig.logImmediately){
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

static void ReadThermocouples(double *tcValuesOut){
	//start by configuring the registers to the required values.
	Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, MAX31856_THERMOCOUPLE_TYPE_USED);
	
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		enum Max31856_Status requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		//enter standby mode until the reading has been prepared (a bit under 1s)
		timedSleep_seconds(&tcInstance, 1);
		enum Max31856_Status tempStatus = Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &(tcValuesOut[index]));
		if(requestStatus != MAX31856_OKAY || tempStatus != MAX31856_OKAY){
			tcValuesOut[index] = NAN;
		}
		PORTA.OUTTGL.reg = ((index & 1) != 0)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
}

static void ReadDendrometers(void){
	LogValues[LOG_VALUES_DEND_INDEX]		= ReadDendro(&adcModule1);
	LogValues[LOG_VALUES_DEND_INDEX + 1]	= ReadDendro(&adcModule2);
}

static void LogAllSdiSensors(float *sdiValuesArray){
	uint16_t sdiValueStartIndex = 0;
	//query and read all values from all the sdi12 sensors
	for(uint8_t sdiSensorIndex = 0; sdiSensorIndex < loggerConfig.numSdiSensors; sdiSensorIndex++){
		struct SDI_transactionPacket transactionPacket;
		transactionPacket.address = loggerConfig.SDI12_SensorAddresses[sdiSensorIndex];
		SDI12_RequestSensorReading(&transactionPacket);
		
		//if the request was faulty (sensor missing, not received or badly parsed, for example), skip to the next sensor
		if(transactionPacket.transactionStatus != SDI12_STATUS_OK){
			continue;
		}
		//if the sensor asked us to wait for some time before reading, let's go into sleep mode for it.
		if(transactionPacket.waitTime > 0){
			timedSleep_seconds(&tcInstance, transactionPacket.waitTime);
		}
		bool success = SDI12_GetSensedValues(&transactionPacket, &(sdiValuesArray[sdiValueStartIndex]));
		if(!success){
			//TODO: if success was false, put NANs in the values.
		}

		//TODO: change sdi to use doubles instead of floats.
		//move the index of sdiValues so the next transaction will write to the correct place in the array.
		sdiValueStartIndex += loggerConfig.SDI12_SensorNumValues[sdiSensorIndex];
	}
}

static void WriteValuesToSD(float *sdiValuesArray){
	//write a 1 into the DS1302 General Purpose Register
	Ds1302SetBatteryBackedRegister(DS1302_GENERAL_PURPOSE_DATA_REGISTER_0, 0x1);
	//log all values to dataFile
	FIL file;
	
	PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;

	char floatingPointConversionBuffer[16];

	f_open(&file,SD_DATALOG_FILENAME, FA_OPEN_ALWAYS);
	f_puts(dateTimeBuffer, &file);
	for(uint8_t logValueIndex = 0; logValueIndex < NUM_LOG_VALUES; logValueIndex++){
		snprintf(floatingPointConversionBuffer, 16, ",%f", LogValues[logValueIndex]);
		f_puts(floatingPointConversionBuffer, &file);
	}
	
	for(uint8_t sdiCounter = 0; sdiCounter < loggerConfig.totalSDI_12Values; sdiCounter++){
		snprintf(floatingPointConversionBuffer, 16, ",%f", sdiValuesArray[sdiCounter]);
		f_puts(floatingPointConversionBuffer, &file);
	}
	f_close(&file);
	
	PORTA.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;
	
	//write a 0 into the DS1302 General Purpose Register
	Ds1302SetBatteryBackedRegister(DS1302_GENERAL_PURPOSE_DATA_REGISTER_0, 0);
}