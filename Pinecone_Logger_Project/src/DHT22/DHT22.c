/*
* Dht22.c
*
* Created: 6/7/2016 3:22:22 PM
*  Author: tim.anderson
*
*
*			Implements functionality to connect and read the Dht-22 Temperature and Humidity Sensor
*			Temperature range between -40c to 80c.
*			Relative Humidity range between 0% and 100%
*			Both Temp and RH have resolution of .1c and .1% respectively.
*
*			Note! Allow at LEAST 2 seconds between readings, or between power-up and first reading
*/



/*		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING
*
*			Because of the fast serial i/o communication protocol, and the low clock rate of 8Mhz,
*			this may not function with optomization flag -O0.
*								Use -O1 or higher for correct functionality.
*
*		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING		WARNING
*/






														/* TODO: FIX THIS CODE*/
#include "Dht22/Dht22.h"
#include <asf.h>
#include <stdbool.h>

//timing values for the interface. All times are in microseconds
#define HOST_SIGNAL_INIT_LOW_MICROS			1000
#define HOST_SIGNAL_INIT_HIGH				10
#define DHT_SIGNAL_TIMEOUT					200
#define DHT_SIGNAL_DATA_LOW_DEFAULT_WAIT	40
#define DHT_SIGNAL_DATA_HIGH_DEFAULT_WAIT	20
#define DHT_SIGNAL_DATA_HIGH_1_LENGTH		45

/*GetDht22Reading
Sends and receives confirmation signals with the DHT-22 sensor

Internal functionality
MCU pulls data line low for >=1ms, then high for 20-40us
Sensor then pulls line low for 80us, high for 80us
DHT22 then proceeds to pull low and high to send each bit of signal
low is always 50us.
IF high is kept for between 26us-28us, interpret as 0 bit
IF high is kept for 70us, interpret as 1 bit.

Data is sent MSB first
first byte: integral RH data
second byte: decimal RH data
third byte: integral temp data
fourth byte: decimal temp data
fifth: 8 bit checksum
checksum should equal the addition of the first 4 bytes */
enum Dht22Status GetDht22Reading(double *temp, double *relativeHumidity, uint8_t dhtPin)
{
	//const pointer to a const register address, but volatile since it will change when the GPIO pin does.
	const volatile uint32_t * const inRegister = &(port_get_group_from_gpio_pin(dhtPin)->IN.reg);
	const uint32_t inRegisterPinmask  = (1UL << (dhtPin % 32));
	
	uint32_t timeTaken = 0;
	*temp = 0;
	*relativeHumidity = 0;
	uint8_t rxBuffer[5];
	memset(rxBuffer, 0, 5);
	
	//ready port config for i/o
	struct port_config config;
	port_get_config_defaults(&config);
	config.direction = PORT_PIN_DIR_OUTPUT;
	config.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(dhtPin, &config);


	//start by holding the data pin low for at least 800us
	port_pin_set_output_level(dhtPin, LOW);
	delay_us(HOST_SIGNAL_INIT_LOW_MICROS);
	port_pin_set_output_level(dhtPin, HIGH);
	//delay_us(HOST_SIGNAL_INIT_HIGH);

	//switch to input to receive confirmation
	config.direction = PORT_PIN_DIR_INPUT;
	config.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(dhtPin, &config);
	
	uint8_t rxBit = 8;
	uint8_t rxByte = 0;
	//now, let's start shifting bits!
	for(int8_t bitCounter = -3; bitCounter < 80; bitCounter++){
		//loop until state has been reached, or timeout
		timeTaken = 0;
		do{
			timeTaken +=2;
			if(timeTaken > DHT_SIGNAL_TIMEOUT){
				return DHT_STATUS_TIMEOUT;
			}
		}while(!!((*inRegister) & inRegisterPinmask) == (bitCounter & 1));

		if((bitCounter & 1) && (bitCounter > 0)){
			rxBuffer[rxByte] |= (timeTaken > DHT_SIGNAL_DATA_HIGH_1_LENGTH? 1 : 0) << --rxBit;
			if(rxBit == 0){
				rxBit = 8;
				rxByte++;
			}
		}
	}
	
	//now that we have our buffer filled, check the parity byte
	uint8_t parity = rxBuffer[0] + rxBuffer[1] + rxBuffer[2] + rxBuffer[3];
	if(parity != rxBuffer[4]){
		return DHT_STATUS_CHECKSUM_ERROR;
	}
	
	//we passed the checksum, now we can parse the rx buffer into output values.
	uint16_t rawHumidity =		(rxBuffer[0] << 8) + rxBuffer[1];
	uint16_t rawTemperature =	(rxBuffer[2] << 8) + rxBuffer[3];
	*relativeHumidity = (double)rawHumidity / 10.0f;
	bool negativeTemp = (rawTemperature & 0x8000) != 0;
	rawTemperature &= 0x7FFF;	//AND away the sign bit
	*temp = (double)rawTemperature / 10.0f;
	if(negativeTemp){
		*temp *= -1;
	}
	return DHT_STATUS_OKAY;
}