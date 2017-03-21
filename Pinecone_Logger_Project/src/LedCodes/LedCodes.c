/*
* LedCodes.c
*
* Created: 3/21/2017 2:21:48 PM
*  Author: tim.anderson
*/

#include "LedCodes.h"
#include <asf.h>

#define NUM_LED_FLASHES_SUCCESS		3
#define NUM_LED_FLASHES_NO_SD		3
#define NUM_LED_FLASHES_CONFIG		4
#define NUM_LED_FLASHES_TIME_INIT	5
#define NUM_LED_FLASHES_GENERIC		6

#define LED_FLASH_TIME_SUCCESS		200
#define LED_FLASH_TIME_ERROR		800


void LedFlashStatusCode(const LedStatusCode statusCode){
	uint16_t ledFlashTime;
	uint8_t repetitions;
	
	switch(statusCode){
		case LED_CODE_START_SUCCESS:{
			ledFlashTime = LED_FLASH_TIME_SUCCESS;
			repetitions = NUM_LED_FLASHES_SUCCESS;
			break;
		}
		
		case LED_CODE_SD_CARD_NOT_FOUND:{
			ledFlashTime = LED_FLASH_TIME_ERROR;
			repetitions = NUM_LED_FLASHES_NO_SD;
			break;
		}
		
		case LED_CODE_CONFIG_MISSING:{
			ledFlashTime = LED_FLASH_TIME_ERROR;
			repetitions = NUM_LED_FLASHES_CONFIG;
			break;
		}
		
		case LED_CODE_TIME_NOT_INIT_MISSING_FILE:{
			ledFlashTime = LED_FLASH_TIME_ERROR;
			repetitions = NUM_LED_FLASHES_TIME_INIT;
			break;
		}
		
		case LED_CODE_GENERIC_ERROR:
		default:{
			ledFlashTime = LED_FLASH_TIME_ERROR;
			repetitions = NUM_LED_FLASHES_GENERIC;
			break;
		}
	}
	
	while(repetitions--){
		PORTA.OUTSET.reg = LED_PIN_PINMASK;
		delay_ms(ledFlashTime);
		PORTA.OUTCLR.reg = LED_PIN_PINMASK;
		delay_ms(ledFlashTime);
	}
}

void LedRepeatStatusCode(const LedStatusCode statusCode){
	while(1){
		LedFlashStatusCode(statusCode);
	}
}