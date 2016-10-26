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

void SdCardInit(FATFS *fatFileSys, FRESULT *mountingResult);


bool tryReadTimeFile(void);

bool openDataFileOrCreateIfMissing(FIL *fileObj, const struct LoggerConfig *loggerConf);

bool readConfigFile(struct LoggerConfig *config);

bool logToDataFile(Ds1302DateTime *dateTime, float *thermocoupleReadings);

int8_t SD_UnitTest(FATFS *fatfs);

#endif /* SD_FILEUTILS_H_ */