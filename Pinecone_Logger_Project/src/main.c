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



void ReadThermocouples(double *tcValuesOut);
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
	PORTA.DIRSET.reg = ALL_MOSFET_PINMASK | TC_MUX_SELECT_ALL_PINMASK | DHT22_1_PINMASK | DHT22_2_PINMASK;
	PORTA.OUTCLR.reg = ALL_MOSFET_PINMASK | TC_MUX_SELECT_ALL_PINMASK;
	PORTA.
	
	struct Ds1302DateTime dateTime;
	
	//wake up the SD card
	PORTA.OUTSET.reg = SD_CARD_MOSFET_PINMASK;
	
	SdCardInit(&mountingResult);
	bool timeFileFound = tryReadTimeFile(&dateTime);
	readConfigFile(&loggerConfig);
	SD_CheckIntegrityOrCreateIfMissing(&loggerConfig);
	
	/*remove power to the SD/MMC card, we'll re enable it when it's time to write the reading.*/
	PORTA.OUTCLR.reg = SD_CARD_MOSFET_PINMASK;
	
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
		double dendroValues[2];
		double tcTempBeforeHeater[4];
		double tcTempAfterHeater[4];
		PORTA.OUTSET.reg = DENDRO_TC_AMP_MOSFET_PINMASK;
		dendroValues[0] = ReadDendro(&adcModule1);
		dendroValues[1] = ReadDendro(&adcModule2);
				
		ReadThermocouples(tcTempBeforeHeater);
				
		//turn on heater, and turn off dendro/tc. Then, sleep for the heater duration.
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | DENDRO_TC_AMP_MOSFET_PINMASK;
		timedSleep_seconds(&tcInstance,HEATER_TIMED_SLEEP_SECONDS);
		//turn heater off, and dendro/tc back on.
		PORTA.OUTTGL.reg = HEATER_MOSFET_PINMASK | DENDRO_TC_AMP_MOSFET_PINMASK;
		
		ReadThermocouples(tcTempAfterHeater);
		
		//turn of dendr/tc, and turn on SDI-12 bus and DHT22s
		PORTA.OUTTGL.reg = DENDRO_TC_AMP_MOSFET_PINMASK | SDI_DHT22_POWER_MOSFET_PINMASK;
		
		
		
		
	}
}

void componentInit(void)
{
	initSleepTimerCounter(&tcInstance);
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	SDI12_Setup();
	DS1302Init();
	Dht22Setup(DHT22_PIN)
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
	Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, MAX31856_THERMOCOUPLE_TYPE_USED);
	PORTA.OUTCLR.reg = TC_MUX_SELECT_ALL_PINMASK;
	
	for(uint8_t index = 0; index < 4; index++){
		//request the reading.
		enum Max31856_Status requestStatus = Max31856RequestReading(&spiMasterModule, &spiSlaveInstance);
		//delay until it is ready, about 200ms maximum
		portable_delay_cycles(200*1000);
		enum Max31856_Status tempStatus = Max31856GetTemp(&spiMasterModule, &spiSlaveInstance, &(tcValuesOut[index]));
		if(requestStatus != MAX31856_OKAY || tempStatus != MAX31856_OKAY){
			tcValuesOut[index] = NAN;
		}
		PORTA.OUTTGL.reg = ((index & 1) != 0)? TC_MUX_SELECT_ALL_PINMASK : TC_MUX_SELECT_A_PINMASK;
	}
}