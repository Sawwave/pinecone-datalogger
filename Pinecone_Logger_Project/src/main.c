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
#include <asf.h>
#include <math.h>
#include "DHT22/DHT22.h"
#include "DS1302/DS1302.h"
#include "MAX31856/MAX31856.h"
#include "TimedSleep/TimedSleep.h"
#include "SDI12/SDI12.h"
#include "Dendro/Dendro.h"
#include "SD_FileUtils/SD_FileUtils.h"



void ReadThermocouples(double *tcValuesOut)
void componentInit(void);
bool MAX31856_VOLATILE_REGISTERS_TEST(void);

struct spi_module spiMasterModule;
struct spi_slave_inst spiSlaveInstance;
struct adc_module adcModule1;
struct adc_module adcModule2;


struct tc_module tcInstance;

int main (void)
{
	struct LoggerConfig loggerConfig;
	FRESULT mountingResult;
	
	system_init();
	delay_init();
	
	#ifdef PINECONE_LOGGER_DEBUG_UNIT_TESTS
	//SD_UnitTest(&fileSystem);
	#endif
	
	//start with all power mosfets off
	MOSFET_PORT.DIRSET.reg = ALL_MOSFET_PINMASK | TC_MUX_SELECT_ALL_PINMASK;
	MOSFET_PORT.OUTCLR.reg = ALL_MOSFET_PINMASK | TC_MUX_SELECT_ALL_PINMASK;
	
	struct Ds1302DateTime dateTime;
	
	//wake up the SD card
	MOSFET_PORT.OUTSET.reg = SD_CARD_MOSFET_PINMASK;
	
	SdCardInit(&mountingResult);
	bool timeFileFound = tryReadTimeFile(&dateTime);
	readConfigFile(&loggerConfig);
	SD_CheckIntegrityOrCreateIfMissing(&loggerConfig);
	
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	MOSFET_PORT.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;
	
	if(timeFileFound){
		Ds1302SetDateTime(&dateTime);
	}
	
	componentInit();
	
	/*If the configuration is set to defer logging for one sleep cycle, accomplish that sleep here.*/
	if(!loggerConfig.logImmediately){
		timedSleep_seconds(&tcInstance, loggerConfig.loggingInterval);
	}

	/*All initialization has been done, so enter the loop!*/
	while(1){
		
		MOSFET_PORT.OUTSET.reg = DENDRO_TC_AMP_MOSFET_PINMASK;
		//read dendro values
		double dendroValues[2];
		//8 thermocouple values, 4 for before the heater, 4 for afterwards
		double tcTempBeforeHeater[4];
		double tcTempAfterHeater[4];
		dendroValues[0] = ReadDendro(&adcModule1);
		dendroValues[1] = ReadDendro(&adcModule2);
				
		ReadThermocouples(tcTempBeforeHeater);
		//turn off select pins AND the dendro/tc mosfet
		TC_MUX_SELECT_PORT.OUTCLR.reg = DENDRO_TC_AMP_MOSFET_PINMASK | TC_MUX_SELECT_ALL_PINMASK;
		
		//turn on heater, and sleep for its duration
		MOSFET_PORT.OUTSET.reg = HEATER_MOSFET_PINAMSK;
		timedSleep_seconds(&tcInstance,HEATER_TIMED_SLEEP_SECONDS);
		
		
		
		
	}
}

void componentInit(void)
{
	initSleepTimerCounter(&tcInstance);
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	SDI12_Setup();
	DS1302Init();
	ConfigureDendroADC(&adcModule1, DEND_ANALOG_PIN_1);
	ConfigureDendroADC(&adcModule2, DEND_ANALOG_PIN_2);
}

bool MAX31856_VOLATILE_REGISTERS_TEST(void){
	//turn MAX31856 on
	delay_s(1);
	//write register
	//turn MAX31856 off
	delay_s(1);
	//turn it on
	//read register
	//see if equal.
	return false;
}

void ReadThermocouples(double *tcValuesOut){
	//start by configuring the registers to the required values.
	Max31856ConfigureRegisters(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint32_t thermocoupleType);
	TC_MUX_SELECT_PORT.OUTCLR.reg = TC_MUX_SELECT_ALL_PINMASK;
	uint32_t pinmask = 0;
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		//delay until it is ready, about 200ms maximum
		portable_delay_cycles(200*1000);
		tempStatus = Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &(tcValuesOut[index]));
		if(requestStatus != MAX31856_OKAY || tempStatus != MAX31856_OKAY){
			tcValuesOut[index] = NAN;
		}
		TC_MUX_SELECT_PORT.OUTTGL.reg = ((index & 1) != 0)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
}