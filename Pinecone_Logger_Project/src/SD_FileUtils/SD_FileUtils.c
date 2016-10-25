/*
* SD_FileUtils.c
*
* Created: 10/21/2016 2:30:59 PM
*  Author: tim.anderson
*/
#include "SD_FileUtils/SD_FileUtils.h"
#include "DS1302/DS1302.h"
#include <asf.h>
#include "ff.h"

/*SdCardInit
Initializes the sd mmc connection, and attempts to mount the fileSystem.
system will be mounted into first arg, fatFilesys.
second argument, fileResult, will show the result state of the attempted mount.
Possible FRESULT returns here are:
FR_OK on success
FR_INVALID_DRIVE if volumeNumber is greater than number of volumes. If in doubt, use 0 here.
FR_INT_ERR if assertion failed for some unknown reason (MORE DOCUMENTATION PLEASE, ASF!)
*/
void SdCardInit(FATFS *fatFileSys, FRESULT *mountingResult){
	sd_mmc_init();
	
	memset(fatFileSys, 0, sizeof(FATFS));
	*mountingResult = f_mount(SD_VOLUME_NUMBER, fatFileSys);
}

/* tryReadTimeFile
//attempts to read the time file, whose filename is defined in boardconf.h.
File should be in format hh,mm,ss,dd,mm,yyyy\n\n
comma is suggested, but any non-numeric, non +- non \n, character should work as a seperater.


*/
bool tryReadTimeFile(void){
	FIL fileObj;
	//see if we can talk to the SD card
	FRESULT status = f_open(&fileObj, SD_TIME_FILENAME, FA_READ);
	if(status != FR_OK){
		return false;
	}
	struct Ds1302DateTime dateTime;
	//reads until \n in found, or until buff is filled
	char buf[22];
	char *bufferPointer = &(buf[0]);
	f_gets(buf, 22, &fileObj);
	f_close(&fileObj);
	if(f_error(&fileObj) || f_eof(&fileObj)  ){
		return false;
	}
	dateTime.hours		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.minutes	= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.seconds	= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.date		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.month		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.year		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	
	
	//if any of the date were 0,  consider the read to have been incorrect
	if(!(dateTime.date && dateTime.month && dateTime.year)){
		return false;
	}
	
	Ds1302SetDateTime(dateTime);

	return true;
}


bool createDataFileIfMissing(char sdiAddresses[], uint8_t numAddresses){
	FIL fileObj;
	//if the file already exists, leave it be!
	if(f_open(&fileObj, SD_DATALOG_FILENAME, FA_CREATE_NEW) == FR_EXIST){
		return false;
	}
	
	//query the SDI sensors
	
	//create the header for the data file
	f_puts("Date,Time,TcPort1,TcPort2,TcPort3,TcPort4,Dht1Temp,Dht1Rh,Dht2Temp,Dht2Rh", &fileObj);

	f_close(&fileObj);
	
	//query the SDI sensors, and add headers for each sensor, and each value per
	char *sdiColumnHeader = ",SDI_A.00";
	const uint8_t sdiHeaderAddressIndex = 5;
	const uint8_t sdiHeaderValueIndex = 8;
	
	for(uint8_t sdiCounter = 0; sdiCounter < numAddresses; sdiCounter++){
		//reset the value index in the header
		sdiColumnHeader[sdiHeaderValueIndex] = '0';
		sdiColumnHeader[sdiHeaderValueIndex - 1] = '0';
		uint8_t valuesFromSensor = SDI12_GetNumReadingsFromSensorMetadata(sdiAddresses[sdiCounter]);
		//slot the address character into the header.
		sdiColumnHeader[sdiHeaderAddressIndex] = sdiAddresses[sdiCounter];
		
		//reopen the file
		f_open(&fileObj, SD_DATALOG_FILENAME, FA_CREATE_NEW);

		//set the value indexes, and write to the data file.
		for(uint8_t valueCounter = 0;valueCounter < valuesFromSensor;valueCounter++){
			//set the ones, then the tens.
			sdiColumnHeader[sdiHeaderValueIndex] = '0' + (valueCounter % 10);
			sdiColumnHeader[sdiHeaderValueIndex - 1] = '0' + (valueCounter / 10);
			f_puts(sdiColumnHeader, &fileObj);
		}
		
		f_close(&fileObj);
	}
	
	
	
	return true;
}

/*readConfigFile

Reads the Configuration file, and stores the configuration in the given struct.
Config file is formmated as defined below:

ABC...
0000
i/d


first line:
ABC.. specifies the SDI12 addresses of the sensors. Thus, if logger is connected to 3 sensors, with addresses 0,7, and B, line 2 may read
07B

second line:
0000 is the number of minutes between readings that the sensor will sleep

third line:
i/d specifies if the sensor takes the reading immediately on waking up, or defers logging until after the sleep interval.


*/
bool readConfigFile(struct LoggerConfig *config){
	FIL fileObj;
	char intervalBuffer[5];
	char flagBuffer[2];
	
	if(f_open(&fileObj, SD_CONFIG_FILENAME, FA_READ) == FR_OK){
		f_gets(config->SDI12_SensorAddresses, SDI12_MAX_SUPPORTED_SENSORS + 1, &fileObj);
		f_gets(intervalBuffer, 5, &fileObj);
		f_gets(flagBuffer, 2, &fileObj);
		f_close(&fileObj);
		
		char *ptrToIntervalBuffer = &(intervalBuffer[0]);
		config->loggingInterval = strtol(ptrToIntervalBuffer, &ptrToIntervalBuffer, 10);
		if(flagBuffer[0] == 'd'){
			config->logImmediately = false;
		}
		else{
			config->logImmediately = true;
		}
		
	}
	
	
	
	
	return true;
}