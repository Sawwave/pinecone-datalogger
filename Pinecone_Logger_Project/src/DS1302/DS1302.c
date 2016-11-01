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
	struct port_config cfg;
	port_get_config_defaults(&cfg);
	cfg.direction = PORT_PIN_DIR_OUTPUT;
	cfg.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(DS1302_CLOCK_PIN, &cfg);
	port_pin_set_config(DS1302_ENABLE_PIN, &cfg);
	port_pin_set_config(DS1302_DATA_PIN, &cfg);
	port_pin_set_output_level(DS1302_CLOCK_PIN, LOW);
	port_pin_set_output_level(DS1302_ENABLE_PIN, LOW);
	port_pin_set_output_level(DS1302_DATA_PIN, LOW);
	
}

void Ds1302SetDateTime(const Ds1302DateTime *dateTime){
	//set the data pin for output
	struct port_config config;
	port_get_config_defaults(&config);
	config.direction = PORT_PIN_DIR_OUTPUT;
	config.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(DS1302_DATA_PIN, &config);
	
	//ready the clock
	port_pin_set_output_level(DS1302_CLOCK_PIN, LOW);
	port_pin_set_output_level(DS1302_ENABLE_PIN, HIGH);
	delay_us(4);
	
	//disable writeProtect
	Ds1302WriteByte( DS1302_CONTROL_WRITE_REGISTER);
	Ds1302WriteByte( DS1302_DISABLE_WRITE_PROTECT_VALUE);
	
	port_pin_set_output_level(DS1302_ENABLE_PIN,LOW);
	delay_us(4);
	port_pin_set_output_level(DS1302_ENABLE_PIN,HIGH);
	delay_us(4);
	
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
	
	port_pin_set_output_level(DS1302_ENABLE_PIN, LOW);
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
	
	//set the data pin for output
	struct port_config config;
	port_get_config_defaults(&config);
	config.direction = PORT_PIN_DIR_OUTPUT;
	config.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(DS1302_DATA_PIN, &config);
	
	port_pin_set_output_level(DS1302_CLOCK_PIN, LOW);
	port_pin_set_output_level(DS1302_ENABLE_PIN, HIGH);
	delay_us(4);
	Ds1302WriteByte(DS1302_CLOCK_BURST_READ_REGISTER);
	
	//set the pin for input
	config.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(DS1302_DATA_PIN, &config);
	
	uint8_t counter;
	for(counter = 0; counter < 10; counter++){
		rxBuffer[counter] = Ds1302ReadByte();
	}
	port_pin_set_output_level(DS1302_ENABLE_PIN, LOW);
	
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

uint8_t Ds1302ReadByte(void){
	uint8_t outByte = 0;
	uint8_t bitCounter;
	for(bitCounter = 0; bitCounter < 8; bitCounter++)
	{
		int pinLevel = port_pin_get_input_level(DS1302_DATA_PIN);
		outByte |= pinLevel << bitCounter;
		delay_us(1);
		port_pin_set_output_level(DS1302_CLOCK_PIN, HIGH);
		delay_us(1);
		port_pin_set_output_level(DS1302_CLOCK_PIN, LOW);
	}
	return outByte;
}

void Ds1302WriteByte(uint8_t byte){
	uint8_t bitCounter;
	for(bitCounter = 0; bitCounter < 8; bitCounter++)
	{
		int8_t pinState = byte & 0x1;
		byte >>= 1;
		port_pin_set_output_level(DS1302_DATA_PIN, pinState);
		delay_us(1);
		port_pin_toggle_output_level(DS1302_CLOCK_PIN);
		delay_us(1);
		port_pin_toggle_output_level(DS1302_CLOCK_PIN);
		
	}
}

uint8_t Ds1302ByteEncode(uint8_t input){
	return (input % 10) + ( (input / 10) << 4);
}
