/*
* DS1302.c
*
* Created: 10/19/2016 4:10:40 PM
*  Author: tim.anderson
*/

#include "Ds1302/Ds1302.h"
#include <asf.h>


//TODO: remove all PORT ASF fucntions, use registers.

#define DS1302_CLOCK_BURST_WRITE_REGISTER 0xBE
#define DS1302_CLOCK_BURST_READ_REGISTER 0xBF
#define DS1302_CONTROL_WRITE_REGISTER 0x8E
#define DS1302_DISABLE_WRITE_PROTECT_VALUE 0
#define DS1302_ENABLE_WRITE_PROTECT_VALUE 1 << 7
#define OUT_BUFFER_SECOND_INDEX 17 
#define OUT_BUFFER_MINUTE_INDEX 14
#define OUT_BUFFER_HOUR_INDEX 11
#define OUT_BUFFER_DATE_INDEX 3
#define OUT_BUFFER_MONTH_INDEX 0
#define OUT_BUFFER_YEAR_INDEX  8

static uint8_t Ds1302ByteEncode(uint8_t input);
static void Ds1302WriteByte(uint8_t byte);
static uint8_t Ds1302ReadByte(void);


void DS1302Init(void){
	PORTA.DIRSET.reg = DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK | DS1302_ENABLE_PINMASK;
	PORTA.OUTCLR.reg = DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK | DS1302_ENABLE_PINMASK;
}

void Ds1302SetDateTime(const Ds1302DateTime *dateTime){

	//enable the clock
	PORTA.OUTSET.reg = DS1302_ENABLE_PIN_INDEX;
	
	//disable writeProtect
	Ds1302WriteByte(DS1302_CONTROL_WRITE_REGISTER);
	Ds1302WriteByte(DS1302_DISABLE_WRITE_PROTECT_VALUE);
	
	PORTA.OUTCLR.reg = DS1302_ENABLE_PINMASK;
	//delay a few cycles just to make sure the DS1302 sees the change
	portable_delay_cycles(8);
	//re enable the DS1302
	PORTA.OUTSET.reg = DS1302_ENABLE_PINMASK;
	
	
	Ds1302WriteByte(DS1302_CLOCK_BURST_WRITE_REGISTER);
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->seconds));
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->minutes));
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->hours | (1 << 7)));	//makes sure to set clock to 24 hr mode
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->date));
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->month));
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->dayOfWeek));
	Ds1302WriteByte(Ds1302ByteEncode(dateTime->year));
	//re-enable write protect
	Ds1302WriteByte(DS1302_ENABLE_WRITE_PROTECT_VALUE);
	
	//disable the DS1302, and make sure the clock and data pins are low while we're at it.
	PORTA.OUTCLR.reg = DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK | DS1302_ENABLE_PINMASK;
}

//outBuffer required to be at least 19 bytes
//datetime output will look like mm/dd/yyyy hh:mm:ss
void Ds1302GetDateTime(char *outBuffer){
	outBuffer[2] = '/';
	outBuffer[5] = '/';
	outBuffer[6] = '2';
	outBuffer[7] = '0';
	outBuffer[10] = ' ';
	outBuffer[13] = ':';
	outBuffer[16] = ':';
	uint8_t rxBuffer[10];
	
	PORTA.OUTSET.reg = DS1302_ENABLE_PINMASK;
	Ds1302WriteByte(DS1302_CLOCK_BURST_READ_REGISTER);
	
	//set the Data pin for input, and enable input in the wrconfig
	PORTA.DIRCLR.reg = DS1302_DATA_PINMASK;
	#if DS1302_DATA_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | DS1302_DATA_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (DS1302_DATA_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	
	uint8_t counter;
	for(counter = 0; counter < 10; counter++){
		rxBuffer[counter] = Ds1302ReadByte();
	}
	
	//turn the data pin back to output mode
	#if DS1302_DATA_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |  DS1302_DATA_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (DS1302_DATA_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	PORTA.DIRCLR.reg = DS1302_DATA_PINMASK;
	
	//disable the ds1302
	PORTA.OUTCLR.reg = DS1302_ENABLE_PINMASK | DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK;
	
	outBuffer[OUT_BUFFER_MINUTE_INDEX] = (rxBuffer[1] >> 4) + '0';
	outBuffer[OUT_BUFFER_MINUTE_INDEX + 1] = (rxBuffer[1] & 0x0F) + '0';
	
	outBuffer[OUT_BUFFER_HOUR_INDEX] = ((rxBuffer[2] >> 4) & 0x7) + '0';//remove the 12hr/24hr flag with the bitmask
	outBuffer[OUT_BUFFER_HOUR_INDEX + 1] = (rxBuffer[2] & 0x0F) + '0';
	
	outBuffer[OUT_BUFFER_DATE_INDEX] = (rxBuffer[3] >> 4)  + '0';
	outBuffer[OUT_BUFFER_DATE_INDEX + 1] = (rxBuffer[3] & 0x0F) + '0';
	
	outBuffer[OUT_BUFFER_MONTH_INDEX] = (rxBuffer[4] >> 4)  + '0';
	outBuffer[OUT_BUFFER_MONTH_INDEX + 1] = (rxBuffer[4] & 0x0F) + '0';
	
	outBuffer[OUT_BUFFER_YEAR_INDEX] = (rxBuffer[6] >> 4)  + '0';
	outBuffer[OUT_BUFFER_YEAR_INDEX + 1] = (rxBuffer[6] & 0x0F) + '0';
	
	outBuffer[OUT_BUFFER_SECOND_INDEX] = (rxBuffer[0] >> 4) + '0';
	outBuffer[OUT_BUFFER_SECOND_INDEX + 1] = (rxBuffer[0] & 0x0F) + '0';
}

//note! registerIndex must be less than 31
uint8_t Ds1302GetBatteryBackedRegister(const uint8_t registerAddress){
	//enable the ds1302
	PORTA.OUTSET.reg = DS1302_ENABLE_PIN_INDEX;
	
	Ds1302WriteByte(registerAddress| 1);	// OR 1 makes it a read
	
	//set the Data pin for input, and enable input in the wrconfig
	PORTA.DIRCLR.reg = DS1302_DATA_PINMASK;
	#if DS1302_DATA_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | DS1302_DATA_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (DS1302_DATA_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	
	//get the byte
	uint8_t storedByte = Ds1302ReadByte();
	
	//set the Data pin for input, and enable input in the wrconfig
	PORTA.DIRCLR.reg = DS1302_DATA_PINMASK;
	#if DS1302_DATA_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | DS1302_DATA_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (DS1302_DATA_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	
	//set all pins to low.
	PORTA.OUTCLR.reg = DS1302_ENABLE_PINMASK | DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK;
	
	return storedByte;
}

void Ds1302SetBatteryBackedRegister(const uint8_t registerAddress, const uint8_t value){
	//enable the ds1302
	PORTA.OUTSET.reg = DS1302_ENABLE_PIN_INDEX;
	
	Ds1302WriteByte(registerAddress);
	Ds1302WriteByte(value);
	
	//set all pins to low.
	PORTA.OUTCLR.reg = DS1302_ENABLE_PINMASK | DS1302_DATA_PINMASK | DS1302_CLOCK_PINMASK;
}

uint8_t Ds1302ReadByte(void){
	uint8_t outByte = 0;
	uint8_t bitCounter;
	for(bitCounter = 0; bitCounter < 8; bitCounter++)
	{
		int pinLevel = (PORTA.IN.reg >> DS1302_DATA_PIN_INDEX) & 1;
		outByte |= pinLevel << bitCounter;
		portable_delay_cycles(8);
		PORTA.OUTTGL.reg = DS1302_CLOCK_PINMASK;
		portable_delay_cycles(8);
		PORTA.OUTTGL.reg = DS1302_CLOCK_PINMASK;
	}
	return outByte;
}

void Ds1302WriteByte(uint8_t byte){
	uint16_t bitmask;
	for(bitmask = 1; bitmask < 256; bitmask <<= 1)
	{
		if(byte & bitmask){
			PORTA.OUTSET.reg = DS1302_DATA_PINMASK;
		}
		else{
			PORTA.OUTCLR.reg = DS1302_DATA_PINMASK;
		}
		portable_delay_cycles(8);
		PORTA.OUTTGL.reg = DS1302_CLOCK_PINMASK;
		portable_delay_cycles(8);
		PORTA.OUTTGL.reg = DS1302_CLOCK_PINMASK;
	}
}

uint8_t Ds1302ByteEncode(uint8_t input){
	return (input % 10) + ( (input / 10) << 4);
}
