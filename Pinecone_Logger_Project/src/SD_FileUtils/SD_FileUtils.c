/*
* SD_FileUtils.c
*
* Created: 10/21/2016 2:30:59 PM
*  Author: tim.anderson
*/
#include "SD_FileUtils/SD_FileUtils.h"
#include "DS1302/DS1302.h"
#include "asf.h"
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
	char buf[20];
	char *bufferPointer = buf;
	f_gets(buf, 20, &fileObj);
	if(f_error(&fileObj) || f_eof(&fileObj)  ){
		return false;
	}
	dateTime.hours		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.minutes	= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.seconds	= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.date		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.month		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	dateTime.year		= (uint8_t)strtol(bufferPointer, &bufferPointer, 10);
	
	f_close(&fileObj);
	
	//if any of them were 0, consider the read to have been incorrect
	if(!(dateTime.hours && dateTime.minutes && dateTime.seconds && dateTime.date && dateTime.month && dateTime.year)){
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
	//create the header for the data file
	f_puts("Date, Time, TcPort1, TcPort2, TcPort3, TcPort4, Dht1Temp, Dht1Rh, Dht2Temp, Dht2Rh, ", &fileObj);
	//query the SDI sensors
	f_close(&fileObj);
	
	return true;
}


bool readConfigFile(struct LoggerConfig *config){
	FIL fileObj;
	if(f_open(&fileObj, SD_CONFIG_FILENAME, FA_READ) == FR_OK){
		
	}
}