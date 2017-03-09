/*
 * DS3231.h
 *
 * Created: 3/9/2017 2:29:34 PM
 *  Author: tim.anderson
 */ 


#ifndef DS3231_H_
#define DS3231_H_

typedef struct dateTime{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} dateTime;


void DS3231_Init(void);

void DS3231_SetTime(struct dateTime)


#endif /* DS3231_H_ */