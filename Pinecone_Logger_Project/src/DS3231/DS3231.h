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

typedef enum DS3231_packet_direction {PACKET_READ, PACKET_WRITE} packet_direction;

/*DS3231_init_i2c
initialized the i2c hardware for the DS3231. This function should be done after power-on, before use of the RTC.
*/
void DS3231_init_i2c(struct i2c_master_module *i2cMasterModule);

/*DS3231_setTimeFromString
Takes a date time string as used in the logger natively, parses the ASCII into BCD for the DS3231 register,
and attempts to set the time in the IC.
*/
void DS3231_setTimeFromString(struct i2c_master_module *i2cMasterModule, const char timeBuffer[19]);

/*DS3231_getTimeToString
Queries the DS3231 RTC for the current time, Parses the BCD register values to ASCII, and writes the received time to the string.
String format is the same as the logger natively saves, in mm/dd/2yyy,hh:mm:ss
*/
void DS3231_getTimeToString(struct i2c_master_module *i2cMasterModule, char timeBuffer[19]);

/*DS3231_setAlarmOnHour
Writes to the DS3231 Alarm2 registers, setting the next alarm to trigger on the next hour change (minute 00, second 00)
*/
void DS3231_setAlarmOnHour(struct i2c_master_module *i2cMasterModule);

/*DS3231_disableAlarm
Writes the the DS3231, disabling the alarm
*/
void DS3231_disableAlarm(struct i2c_master_module *i2cMasterModule);

/*DS3231_setAlarmFromTime
Function takes the logging interval in minutes, as well as the stored time buffer, and computes the next hh:mm that an alarm would be scheduled for
Then, taking that value, the RTC sets ALARM2 for that time.
*/
void DS3231_setAlarmFromTime(struct i2c_master_module *i2cMasterModule, const uint16_t loggingInterval, const char timeBuffer[19]);


#endif /* DS3231_H_ */