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



void componentInit(FATFS *fileSystem);
bool MAX31856_VOLATILE_REGISTERS_TEST(void);

struct spi_module spiMasterModule;
struct spi_slave_inst spiSlaveInstance;
struct adc_module adcModule;







int main (void)
{
	struct LoggerConfig loggerConfig;
	
	FATFS fileSystem;
	FIL fileObj;
	
	system_init();
	delay_init();
	irq_initialize_vectors();
	
	SD_UnitTest(&fileSystem);
	
	//WAKE UP DS1302, SD card,
	//componentInit(&fileSystem);
	
	//tryReadTimeFile();
	
	//readConfigFile(&loggerConfig);
	
	//QUERY SDI SENSORS FOR METADATA
	
	//OPEN THE DATA FILE
	
	while(1){
		
	}
}

void componentInit(FATFS *fatFileSystem){
	FRESULT mountingResult;
	enum Max31856_Status amplifierStatus;
	
	Max31856ConfigureSPI(&spiMasterModule, &spiSlaveInstance);
	SDI12_Setup();
	//wake up DS1302
	DS1302Init();
	//wake up SD card
	SdCardInit(fatFileSystem, &mountingResult);
	//wake up MAX31856
	amplifierStatus =  Max31856ConfigureRegisters(&spiMasterModule, &spiSlaveInstance, MAX31856_THERMOCOUPLE_TYPE_USED);
	ConfigureDendroADC(&adcModule);
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