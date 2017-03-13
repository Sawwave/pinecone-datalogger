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


struct dateTime{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
};

struct Ds3231_alarmTime{
	uint8_t minutes;
	uint8_t hours;
};


void DS3231_init(struct i2c_master_module *i2cMasterModule);

void DS3231_setTime(struct i2c_master_module *i2cMasterModule, const struct dateTime *datetime);

void DS3231_getTime(struct i2c_master_module *i2cMasterModule, struct dateTime *dateTime);

void DS3231_setAlarm(struct i2c_master_module *i2cMasterModule, const struct Ds3231_alarmTime *alarmTime);

void DS3231_disableAlarm(struct i2c_master_module *i2cMasterModule);



#endif /* DS3231_H_ */