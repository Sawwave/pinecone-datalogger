/*
 * LedCodes.h
 *
 * Created: 3/21/2017 2:21:03 PM
 *  Author: tim.anderson
 */ 


#ifndef LEDCODES_H_
#define LEDCODES_H_

typedef enum LedStatusCode {
	LED_CODE_START_SUCCESS,
	LED_CODE_GENERIC_ERROR,
	LED_CODE_SD_CARD_NOT_FOUND,
	LED_CODE_CONFIG_MISSING,
	LED_CODE_TIME_NOT_INIT_MISSING_FILE
} LedStatusCode;

void LedFlashStatusCode(const LedStatusCode statusCode);

void LedRepeatStatusCode(const LedStatusCode statusCode);

#endif /* LEDCODES_H_ */