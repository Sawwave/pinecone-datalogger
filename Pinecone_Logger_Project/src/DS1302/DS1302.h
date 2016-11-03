/*
 * Ds1302.h
 *
 * Created: 10/19/2016 4:10:27 PM
 *  Author: tim.anderson
 */ 


#ifndef DS1302_H_
#define DS1302_H_
#include <stdint.h>

#define DS1302_GENERAL_PURPOSE_DATA_REGISTER_0 0xC0
#define DS1302_GENERAL_PURPOSE_DATA_REGISTER_1 0xC2

//day of week is not necessary in this implementation, but if you wish to use it,
// 1 = Sunday, 2 = Monday, etc.
// Year expected to be the last two digits of 20xx (2053 would be the value 53)
typedef struct Ds1302DateTime{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t dayOfWeek;
} Ds1302DateTime;

void DS1302Init(void);

//Ds1302SetDatetime()
//	Sets the dateTime of a Ds1302, using the given pin configurations, and a datetime as array
void Ds1302SetDateTime(const Ds1302DateTime *dateTime);

//outBuffer required to be at least 17 bytes
//datetime output will look like mm/dd/yyyy hh:mm
void Ds1302GetDateTime(char *outBuffer);

//Ds1302GetBatteryBackedRegister
//retrieves the data stored at the given register in the Ds1302.
uint8_t Ds1302GetBatteryBackedRegister(const uint8_t registerAddress);

//Ds1302SetBatteryBackedRegister
//stores the given byte in the battery backed RAM of the DS1302
void Ds1302SetBatteryBackedRegister(const uint8_t registerAddress, const uint8_t value);


#endif /* DS1302_H_ */