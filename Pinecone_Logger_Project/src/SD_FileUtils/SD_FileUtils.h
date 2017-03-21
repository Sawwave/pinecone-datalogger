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
bool TryReadTimeFile(char timeBuffer[19]);

/* SD_CreateWithHeaderIfMissing
Function determines if the datalog file exists. If it doesn't,
the function calls to SD_FileCreatWithHeader
return true if header was created or file already existed.*/
void SD_CreateWithHeaderIfMissing(const struct LoggerConfig *loggerConfig);

/*readConfigFile

Reads the Configuration file, and stores the configuration in the given struct.
Config file is formated as defined below:

ABC...
9,9,9...
0000
i

first line:
ABC.. specifies the SDI12 addresses of the sensors. Thus, if logger is connected to 3 sensors, with addresses 0,7, and B, line 2 may read
07B
second line:
9,9,9,... specifies the SDI12 number of values for each of the sdi sensors addressed in the first line.
third line:
0000 is the number of minutes between readings that the sensor will sleep
fouth line:
i may be letter i or d specifies if the sensor takes the reading immediately on waking up(i), or defers logging until after the sleep interval(d).
*/
bool ReadConfigFile(struct LoggerConfig *config);

bool SD_CheckIntegrity(const struct LoggerConfig *loggerConfig);


#ifdef SD_FILE_UTILS_UNIT_TEST
int8_t SD_UnitTest(void);
#endif

#endif /* SD_FILEUTILS_H_ */