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

void fileWriteHeader(FIL *fileObj, const struct LoggerConfig *loggerConf);

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


bool openDataFileOrCreateIfMissing(const struct LoggerConfig *loggerConf){
	FIL fileObj;
	FRESULT openExistingResult = f_open(&fileObj, SD_DATALOG_FILENAME, FA_OPEN_EXISTING);
	
	if(openExistingResult == FR_OK){
		//seek to end so we write append
		f_lseek(&fileObj, f_size(&fileObj));
		return true;
	}
	else if (openExistingResult == FR_NO_FILE){
		//file didn't exist, so let's create one and add the header!
		openExistingResult = f_open(&fileObj, SD_DATALOG_FILENAME, FA_READ | FA_WRITE);
		if(openExistingResult == FR_OK){
			fileWriteHeader(&fileObj, loggerConf);
			return true;
		}
	}
	
	//there was some other kind of error.
	//TODO: do something in this case, maybe?
	return false;
}

void fileWriteHeader(FIL *fileObj, const struct LoggerConfig *loggerConf){
	//create the header for the data file
	f_puts("Date,Time,TcPort1,TcPort2,TcPort3,TcPort4,Dht1Temp,Dht1Rh,Dht2Temp,Dht2Rh", fileObj);

	//query the SDI sensors, and add headers for each sensor, and each value per
	char sdiColumnHeader[9] = {',','S','D','I','_','A','.','0','0'};	//,SDI12_A.00
	const uint8_t sdiHeaderAddressIndex = 5;
	const uint8_t sdiHeaderOnesValueIndex = 8;
	
	for(uint8_t sdiCounter = 0; sdiCounter < loggerConf->numSdiSensors; sdiCounter++){
		//reset the value index in the header
		sdiColumnHeader[sdiHeaderOnesValueIndex] = '0';
		sdiColumnHeader[sdiHeaderOnesValueIndex - 1] = '0';
		//slot the address character into the header.
		sdiColumnHeader[sdiHeaderAddressIndex] = loggerConf->SDI12_SensorAddresses[sdiCounter];
		
		//set the value indexes, and write to the data file.
		for(uint8_t valueCounter = 0;valueCounter < loggerConf->SDI12_SensorNumValues[sdiCounter]; valueCounter++){
			//set the ones, then the tens.
			sdiColumnHeader[sdiHeaderOnesValueIndex] = '0' + (valueCounter % 10);
			sdiColumnHeader[sdiHeaderOnesValueIndex - 1] = '0' + (valueCounter / 10);
			f_puts(sdiColumnHeader, fileObj);
		}
	}
	f_sync(fileObj);
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
		
		//count up the number of addresses.
		uint8_t numSensors = 0;
		while(config->SDI12_SensorAddresses[(config->numSdiSensors)] != 0){
			config->numSdiSensors++;
		}
		
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

bool SD_UnitTest(void);