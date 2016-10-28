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

#define SD_DEBUG_FILE "0:debug.txt"

void fileWriteHeader(FIL *fileObj, const struct LoggerConfig *loggerConf);

FRESULT SD_UnitTestRemoveFile(void);
FRESULT SD_UnitTestCreateDebugFile(FIL *file);
FRESULT SD_UnitTestReadFile(FIL *file);
int SD_UnitTestLoadAndCheckDebugFile(const char *addresses, uint8_t numAddresses, const char *interval, bool defer);

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
	Ctrl_status checkStatus;
	do{
		checkStatus = sd_mmc_test_unit_ready(0);
	} while (checkStatus != CTRL_GOOD);
	
	
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
	
	
	//if any of the date were 0, consider the read to have been incorrect
	if(!(dateTime.date && dateTime.month && dateTime.year)){
		return false;
	}
	
	Ds1302SetDateTime(dateTime);

	return true;
}


bool openDataFileOrCreateIfMissing(FIL *fileObj, const struct LoggerConfig *loggerConf){
	FRESULT openExistingResult = f_open(fileObj, SD_DATALOG_FILENAME, FA_OPEN_EXISTING);
	
	if(openExistingResult == FR_OK){
		//seek to end so we write append
		f_lseek(fileObj, f_size(fileObj));
		return true;
	}
	else if (openExistingResult == FR_NO_FILE){
		//file didn't exist, so let's create one and add the header!
		openExistingResult = f_open(fileObj, SD_DATALOG_FILENAME, FA_READ | FA_WRITE);
		if(openExistingResult == FR_OK){
			fileWriteHeader(fileObj, loggerConf);
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
	char flagBuffer[4];
	memset(intervalBuffer, 0, 5 * sizeof(char));
	memset(flagBuffer, 0, 10 * sizeof(char));
	memset(config->SDI12_SensorAddresses, 0, sizeof(char) * (SDI12_MAX_SUPPORTED_SENSORS + 1));
	
	if(f_open(&fileObj, SD_CONFIG_FILENAME, FA_READ) == FR_OK){
		f_gets(config->SDI12_SensorAddresses, SDI12_MAX_SUPPORTED_SENSORS + 1, &fileObj);
		f_gets(intervalBuffer, 6, &fileObj);
		f_gets(flagBuffer, 4, &fileObj);
		f_close(&fileObj);
		
		//count up the number of addresses. Consider a null terminator, CR, or LF to be terminating.
		config->numSdiSensors = 0;
		while(config->SDI12_SensorAddresses[(config->numSdiSensors)] != 0 && 
		config->SDI12_SensorAddresses[(config->numSdiSensors)] != 10 &&
		config->SDI12_SensorAddresses[(config->numSdiSensors)] != 13){
			//query the sensor for the num values it has
			config->SDI12_SensorNumValues[config->numSdiSensors] = SDI12_GetNumReadingsFromSensorMetadata(config->SDI12_SensorAddresses[config->numSdiSensors]);
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

int8_t SD_UnitTest(FATFS *fatfs){
	FIL file;
	FRESULT result;
	
	do{
		SdCardInit(fatfs, &result);
	}while(result == FR_NOT_READY);
	
	if(result != FR_OK){
		return result * -1;
	}
	int returnCode;
	returnCode = SD_UnitTestLoadAndCheckDebugFile("asdfASDF",8, "0000", true);
	if(returnCode < 0){
		return -100;
	}
	returnCode = SD_UnitTestLoadAndCheckDebugFile("asdfASDF",8, "1000", false);
	if(returnCode < 0){
		return -101;
	}
	returnCode = SD_UnitTestLoadAndCheckDebugFile("asdfASDF",8, "0990", false);
	if(returnCode < 0){
		return -102;
	}
	returnCode = SD_UnitTestLoadAndCheckDebugFile("asdfASDF",8, "5523", false);
	if(returnCode < 0){
		return -103;
	}
	returnCode = SD_UnitTestLoadAndCheckDebugFile("asdfASDF",8, "0000", true);
	if(returnCode < 0){
		return -104;
	}
	returnCode = SD_UnitTestLoadAndCheckDebugFile("a",1, "0000", false);
	if(returnCode < 0){
		return -105;
	}
	return 0;
	returnCode = SD_UnitTestLoadAndCheckDebugFile("0",1, "0000", false);
	if(returnCode < 0){
		return -106;
	}
	return 0;
	returnCode = SD_UnitTestLoadAndCheckDebugFile("96",2, "0000", false);
	if(returnCode < 0){
		return -107;
	}
	return 0;
	returnCode = SD_UnitTestLoadAndCheckDebugFile("1NbB",4, "0900", false);
	if(returnCode < 0){
		return -108;
	}
	return 0;
}

int SD_UnitTestLoadAndCheckDebugFile(const char *addresses, uint8_t numAddresses, const char *interval, bool defer){
	FIL file;
	FRESULT result;
	result = f_open(&file,SD_CONFIG_FILENAME, FA_CREATE_ALWAYS | FA_WRITE);
	if(result != FR_OK){
		return -1;
	}
	
	f_puts(addresses, &file);
	f_putc('\n', &file);
	f_puts(interval, &file);
	f_putc('\n', &file);
	if(defer){
		f_puts("d", &file);
	}
	else{
		f_puts("i", &file);
	}
	f_putc('\n',&file);
	f_close(&file);

	struct LoggerConfig config;
	bool success = readConfigFile(&config);
	if(!success)
	{
		return -4;
	}
	
	if(config.numSdiSensors != numAddresses){
		return -5;
	}
	for(int x = 0; x < numAddresses;x++){
		if(config.SDI12_SensorAddresses[x] != addresses[x]){
			return -6;
		}
		if(config.SDI12_SensorNumValues[x] != 0){
			return -7;
		}
	}
	int intParse = atoi(interval);
	if(config.loggingInterval != intParse){
		return -8;
	}
	if(config.logImmediately == defer){
		return -9;
	}
	return 1;
}


FRESULT SD_UnitTestCreateDebugFile(FIL *file){
	FRESULT res;
	do{
		res = f_open(file, SD_DEBUG_FILE, FA_CREATE_ALWAYS | FA_WRITE);
	}while(res == FR_NOT_READY);
	if(res != FR_OK){
		return res;
	}
	const char *message = "debug message\n w space start\n\n!@#%%^*(%^";
	size_t messageLen = strlen(message);
	uint16_t charsWritten = 0;
	charsWritten = f_puts("debug message\n w space start\n\n!@#%%^*(%^", file);
	f_sync(file);
	if(charsWritten < messageLen - 1){
		return res;
	}
	res = f_close(file);
	if(res != FR_OK){
		return res;
	}
	
	return res;
}


FRESULT SD_UnitTestReadFile(FIL *file){
	FRESULT res = f_open(file, SD_DEBUG_FILE,FA_READ);
	if(res != FR_OK){
		return res;
	}
	char buffer[256];
	memset(buffer, 0, 256);
	f_gets(&buffer[0],256, file);
	uint8_t len = 0;
	while(buffer[len] != 0){
		len++;
	}
	if(len != 14){
		return FR_TOO_MANY_OPEN_FILES;
	}
	
	memset(buffer, 0, 256);
	f_gets(&buffer[0], 256, file);
	len = 0;
	while(buffer[len] != 0){
		len++;
	}
	if(len != 15){
		return FR_TOO_MANY_OPEN_FILES;
	}
	
	memset(buffer, 0, 256);
	f_gets(&buffer[0], 256, file);
	len = 0;
	while(buffer[len] != 0){
		len++;
	}
	if(len != 1)
	{
		return FR_TOO_MANY_OPEN_FILES;
	}
	
	memset(buffer, 0, 256);
	f_gets(&buffer[0], 256, file);
	len = 0;
	while(buffer[len] != 0){
		len++;
	}
	if(len != 10){
		return FR_TOO_MANY_OPEN_FILES;
	}
	
	res = f_close(file);
	if(res != FR_OK){
		return res;
	}
	
	return res;
}

FRESULT SD_UnitTestRemoveFile(void){
	FILINFO info;
	FRESULT res = f_stat(SD_DEBUG_FILE, &info);
	if(res == FR_NO_FILE){
		return res;
	}
	
	res = f_unlink(SD_DEBUG_FILE);
	if(res == FR_OK){
		return res;
	}
	
	
	res = f_stat(SD_DEBUG_FILE, &info);
	if(res != FR_NO_FILE){
		return res;
	}
	return res;
}