/*
 * DS3231.h
 *
 * Created: 3/9/2017 2:29:34 PM
 *  Author: tim.anderson
 */ 


#ifndef DS3231_H_
#define DS3231_H_

#include <asf.h>
#include <i2c_master.h>

struct Ds3231_alarmTime{
	uint8_t minutes;
	uint8_t hours;
};


void DS3231_init_i2c(struct i2c_master_module *i2cMasterModule);

void DS3231_setTimeFromString(struct i2c_master_module *i2cMasterModule, const char timeBuffer[19]);

void DS3231_getTimeToString(struct i2c_master_module *i2cMasterModule, char timeBuffer[19]);

void DS3231_setAlarm(struct i2c_master_module *i2cMasterModule, const struct Ds3231_alarmTime *alarmTime);

void DS3231_disableAlarm(struct i2c_master_module *i2cMasterModule);

void DS3231_createAlarmTime(const char *dateTimeString, const uint16_t alarmTimeInMinutes, struct Ds3231_alarmTime *alarmTime);

void DS3231_alarmOnSecond(struct i2c_master_module *i2cMasterModule);


#endif /* DS3231_H_ */