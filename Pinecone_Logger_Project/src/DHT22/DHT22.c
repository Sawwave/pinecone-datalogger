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


#include "Dht22/Dht22.h"
#include <math.h>
#include <asf.h>

//timing values for the interface. All times are in microseconds
#define HOST_SIGNAL_INIT_LOW_CYCLES			1143
#define HOST_SIGNAL_INIT_HIGH_CYCLES		8
#define DHT_SIGNAL_TIMEOUT					200
#define DHT_SIGNAL_DATA_HIGH_1_LENGTH		20

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
enum Dht22Status GetDht22Reading(struct FixedPoint32 *temp, struct FixedPoint32 *relativeHumidity, const uint32_t dhtPinmask)
{
	//NOTE! this is probably unnecessary, just ignore for now.
	//put the pinmask in the correct bits, enable IN, write the mux, and write the pincfg
	const uint32_t wrConfigInValue = (dhtPinmask << PORT_WRCONFIG_PINMASK_Pos) | PORT_WRCONFIG_INEN  | PORT_WRCONFIG_WRPINCFG;
	const uint32_t wrConfigPowerSave = (dhtPinmask << PORT_WRCONFIG_PINMASK_Pos)  | PORT_WRCONFIG_WRPINCFG;
	uint32_t timeTaken = 0;
	temp->data = 0;
	relativeHumidity->data = 0;
	uint8_t rxBuffer[5];
	memset(rxBuffer, 0, 5);

	//start by holding the data pin low for at least 800us
	PORTA.OUTCLR.reg = dhtPinmask;
	portable_delay_cycles(HOST_SIGNAL_INIT_LOW_CYCLES);
	PORTA.OUTSET.reg = dhtPinmask;
	portable_delay_cycles(HOST_SIGNAL_INIT_HIGH_CYCLES);
	//set pin to input
	PORTA.DIRCLR.reg = dhtPinmask;
	PORTA.WRCONFIG.reg = wrConfigInValue;
	//switch to input to receive confirmation
	
	uint8_t rxBit = 8;
	uint8_t rxByte = 0;
	//now, let's start shifting bits!
	for(int8_t bitCounter = -3; bitCounter < 80; bitCounter++){
		//loop until state has been reached, or timeout
		timeTaken = 0;
		do{
			timeTaken +=2;
			if(timeTaken > DHT_SIGNAL_TIMEOUT){
				//we timed out, so stop listening to the data pin.
				PORTA.WRCONFIG.reg = wrConfigPowerSave;
				PORTA.OUTSET.reg = dhtPinmask;
				
				temp->isValid = false;
				relativeHumidity->isValid = false;;
				return DHT_STATUS_TIMEOUT;
			}
		}while(!!(PORTA.IN.reg & dhtPinmask) == (bitCounter & 1));
		
		if((bitCounter & 1) && (bitCounter > 0)){
			rxBuffer[rxByte] |= (timeTaken > DHT_SIGNAL_DATA_HIGH_1_LENGTH? 1 : 0) << --rxBit;
			if(rxBit == 0){
				rxBit = 8;
				rxByte++;
			}
		}
	}
	//change the pin to output HIGH to put DHT in sleep mode.
	PORTA.DIRSET.reg = dhtPinmask;
	PORTA.WRCONFIG.reg = wrConfigPowerSave;
	PORTA.OUTSET.reg = dhtPinmask;
	
	//now that we have our buffer filled, check the parity byte
	uint8_t parity = rxBuffer[0] + rxBuffer[1] + rxBuffer[2] + rxBuffer[3];
	if(parity != rxBuffer[4]){
		temp->isValid = false;
		relativeHumidity->isValid = false;
		
		return DHT_STATUS_CHECKSUM_ERROR;
	}
	
	//we passed the checksum, now we can parse the rx buffer into output values.
	relativeHumidity->data =	(rxBuffer[0] << 8) + rxBuffer[1];
	relativeHumidity->decimalDigits = 1;
	relativeHumidity->isValid = true;
	temp->data =	(rxBuffer[2] << 8) + rxBuffer[3];
	temp->decimalDigits = 1;
	temp->isValid = true;
	return DHT_STATUS_OKAY;
}
