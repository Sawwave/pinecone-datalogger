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
#include "DHT22/DHT22.h"
#include "DS1302/DS1302.h"
#include "MAX31856/MAX31856.h"
#include "TimedSleep/TimedSleep.h"
#include "SDI12/SDI12.h"
#include "Dendro/Dendro.h"
#include "SD_FileUtils/SD_FileUtils.h"



void componentInit(void);
bool MAX31856_VOLATILE_REGISTERS_TEST(void);

struct spi_module spiMasterModule;
struct spi_slave_inst spiSlaveInstance;
struct adc_module adcModule;

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
	MOSFET_PORT.DIRSET.reg = ALL_MOSFET_PINMASK;
	MOSFET_PORT.OUTCLR.reg = ALL_MOSFET_PINMASK;
	
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
		Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, MAX31856_THERMOCOUPLE_TYPE_USED);
		
	}
}

void componentInit(void)
{
	initSleepTimerCounter(&tcInstance);
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	SDI12_Setup();
	DS1302Init();
	ConfigureDendroADC(&adcModule, DEND_ANALOG_PIN_1);
	ConfigureDendroADC(&adcModule, DEND_ANALOG_PIN_2);
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