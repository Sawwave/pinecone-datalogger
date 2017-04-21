/*
 * SD_FileUtils.h
 *
 * Created: 10/21/2016 2:31:14 PM
 *  Author: tim.anderson
 */ 


#ifndef SD_FILEUTILS_H_
#define SD_FILEUTILS_H_

#include "asf.h"
#include "DS1302/DS1302.h"
#include "SDI12/SDI12.h"

/*SdCardInit
Initializes the sd mmc connection, and attempts to mount the fileSystem.
system will be mounted into first arg, fatFilesys.
second argument, fileResult, will show the result state of the attempted mount.

Returns true on success, false if sd card setup failed (Is there a card in the slot?).
*/
bool SdCardInit(FATFS *fatFileSys);

/* tryReadTimeFile
attempts to read the time file, whose filename is defined in boardconf.h.
File should be in format hh,mm,ss,mm,dd,yyyy
comma is suggested, but any non-numeric, non +- non \n, character should work as a separator.

returns false if file didn't exist, or there was an error.
*/
bool SD_TryReadTimeFile(char timeBuffer[19]);

/* SD_CreateWithHeaderIfMissing
Function determines if the datalog file exists. If it doesn't,
the function calls to SD_FileCreatWithHeader
return true if header was created or file already existed.*/
void SD_CreateWithHeaderIfMissing(const struct LoggerConfig *loggerConfig);

/*readConfigFile
Reads the Configuration file, and stores the configuration in the given struct.
Config file is formated as defined below:

0000
EEEEEEET
ABC...
9,9,9...

first line:
0000 is the number of minutes between readings that the sensor will sleep

Second line:
specifies a series of boolean configuration flags:

Flag 1: Start On Hour, values (E = wait for top of hour, D = log immediately)
Determines if the logger should attempt to log immediately when waking up, or if it should wait until the top of the next hour.
Flag 2: Enable Dendrometer 1, values (E = enable, D = disable)
Determines if dendrometer 1 should be logged, or skipped
Flag 3: Enable Dendrometer 2, values (E = enable, D = disable)
Determines if dendrometer 2 should be logged, or skipped
Flag 4: Enable DHT 1, values (E = enable, D = disable)
Determines if DHT-22 1 should be logged, or skipped
Flag 5: Enable DHT 2, values (E = enable, D = disable)
Determines if DHt-22 2 should be logged, or skipped
Flag 6: Enable SDI, values (E = enable, D = disable)
Determines if SDI-sensors should be logged, or skipped
Flag 7: Enable Sap flux, values (E = enable, D = disable)
Determines if sap flux system should be logged, or skipped
Flag 8	Thermocouple Type:
specifies the type of thermocouples used. Acceptable characters are any of the following: BEJKNRST .
This character is not case sensitive.

Third Line
ABC.. specifies the SDI12 addresses of the sensors. Thus, if logger is connected to 3 sensors, with addresses 0,7, and B, line 2 may read
07B

Fourth line:
9,9,9,... specifies the SDI12 number of values for each of the sdi sensors addressed in the Third line.


returns true if config file was found, false otherwise.
*/
bool ReadConfigFile(struct LoggerConfig *config);

bool SD_CheckIntegrity(const struct LoggerConfig *loggerConfig);


#ifdef SD_FILE_UTILS_UNIT_TEST
int8_t SD_UnitTest(void);
#endif

#endif /* SD_FILEUTILS_H_ */