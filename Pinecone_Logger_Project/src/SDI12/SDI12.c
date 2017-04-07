/*
* SDI_12.c
*
* Created: 10/18/2016 4:25:57 PM
*  Author: tim.anderson
*/
#include <asf.h>
#include <string.h>
#include <math.h>
#include "SDI12/SDI12.h"
#include "TimedSleep/TimedSleep.h"

#ifdef SDI_DEBUG
#define SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS		5
#define BREAK_DELAY_CYCLES							20000	// >= 12ms marking length
#define MARKING_8330_DELAY_CYCLES					16080 	//8330us spacing delay
#define BIT_TIMING_DELAY_CYCLES						940		//833us bit timing
#define BIT_TIMING_HALF_DELAY_CYCLES				400		//416.5us to get halfway into reading a bit

#else
#define SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS		5
#define BREAK_DELAY_CYCLES							30000	// >= 12ms marking length
#define MARKING_8330_DELAY_CYCLES					30080 	// >= 8330us spacing delay
#define BIT_TIMING_DELAY_CYCLES						980		//833us bit timing
#define BIT_TIMING_HALF_DELAY_CYCLES				200		//416.5us to get halfway into reading a bit
#endif



#define BAUD_1200_SLEEP_TIMING 6667

static char CharAddParity(char address);
static uint8_t SDI12_ParseNumValuesFromResponse(char outBuffer[], uint8_t outBufferLen);
static bool SDI12_GetTimeFromResponse(const char *response, uint16_t *outTime);

#ifdef SDI12_UNIT_TESTING
static bool SDI12_TIME_FORMAT_UNIT_TEST(void);
#endif

/*SDI12 PerformTransaction
The major workhorse of the SDI-12 library. performs a full transaction with an SDI-12 sensor.
In this transaction, the data logger sends a message addressed to a particular sensor, and the sensor response with its own message.
outBuffer should be large enough to accommodate the expected response. For example, an _M! message should be somewhere in the range of
12-16 characters to be safe.
Returns SDI12_STATUS_OK on success, but may SDI12_TRANSACTION_TIMEOUT, SDI12_BAD_RESPONSE, or simply SDI12_TRANSACTION_FAILURE.*/
enum SDI12_ReturnCode  SDI12_PerformTransaction( const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen){
	//clear out the output buffer
	memset(outBuffer, 0, sizeof(char) * outBufferLen);
	
	//clear for at least 8330 micros
	PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
	portable_delay_cycles(MARKING_8330_DELAY_CYCLES);
	
	for(uint8_t byteNumber = 0; byteNumber < messageLen; byteNumber++){
		//send the start bit (always HIGH)
		PORTA.OUTSET.reg = SDI_PIN_PINMASK;
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		
		//send the data byte, with parity bit already included
		uint8_t bitmask = 0x1;
		while(bitmask){
			if(message[byteNumber] & bitmask){
				PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
			}
			else{
				PORTA.OUTSET.reg = SDI_PIN_PINMASK;
			}
			portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
			
			bitmask <<= 1;
		}
		
		//send stop bit (always LOW);
		PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		
	}
	
	//set SDI pin to input by changing DIR and setting INEN in WRCONFIG
	PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
	#if SDI_PIN_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | SDI_PIN_PINMASK | PORT_WRCONFIG_PULLEN;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (SDI_PIN_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	PORTA.DIRCLR.reg = SDI_PIN_PINMASK;
	

	//wait for the timeout to be LOW at least once, then delay through the start bit (HIGH)
	uint16_t timeout = 10000;
	
	do{
		portable_delay_cycles(10);
	} while( (--timeout) && (PORTA.IN.reg & SDI_PIN_PINMASK));
	do{
		portable_delay_cycles(10);
	} while( (--timeout) && !(PORTA.IN.reg & SDI_PIN_PINMASK));
	
	if(timeout == 0){
		PORTA.DIRSET.reg = SDI_PIN_PINMASK;
		PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
		#if SDI_PIN_PINMASK < (1 << 16)
		PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |  SDI_PIN_PINMASK;
		#else
		PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (SDI_PIN_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
		#endif
		return SDI12_TRANSACTION_TIMEOUT;
	}
	
	
	//if we got here, we've just gone through the first start bit (HIGH)
	uint8_t byteNumber = 0;
	do{
		//delay halfway into the bit
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		portable_delay_cycles(BIT_TIMING_HALF_DELAY_CYCLES);
		
		uint8_t bitmask = 0x1;
		while(bitmask < 0x80){
			if((PORTA.IN.reg & SDI_PIN_PINMASK) == 0){
				outBuffer[byteNumber] |= bitmask;
			}
			portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
			
			bitmask <<= 1;
		}
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		
		
		timeout = 255;
		//loop-delay through the stop bit (LOW) to realign with the byte frame
		do{
			portable_delay_cycles(5);
		}while( ( (PORTA.IN.reg & SDI_PIN_PINMASK) == 0) && (timeout--) != 0);
		
		byteNumber++;
		
	} while(
	(timeout)	&& 												//we got the start bit!
	(byteNumber < (outBufferLen - 1)) && 						//the buffer overflowed (overflew?)
	(outBuffer[byteNumber-1] != 10)								// the last byte was Line Feed
	);

	//disable INEN
	PORTA.DIRSET.reg = SDI_PIN_PINMASK;
	PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
	#if SDI_PIN_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |  SDI_PIN_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (SDI_PIN_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif

	//null terminate the rxMessage.
	outBuffer[byteNumber]= 0;

	//the transaction was successful if the message starts with the address, and ends with a Line Feed (ascii value 10)
	if( (outBuffer[0] == (message[0] & 0x7F )) && (outBuffer[byteNumber - 1] == 10) ){
		return SDI12_STATUS_OK;
	}
	else{
		return SDI12_BAD_RESPONSE;
	}
}

/*SDI12_RequestSensorReading
communicates with the sensor at the given address on the SDI12 bus, and requests a reading (M! command).
Will load SDI12_STATUS_OK into the packet on success, SDI12_BAD_RESPONSE on failure.
*/
void SDI12_RequestSensorReading(struct SDI_transactionPacket *transactionPacket){
	uint8_t tries = SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS;
	const uint8_t messageLen = 3;
	char message[3] = { CharAddParity(transactionPacket->address), 'M', '!'};
	const uint8_t responseLength = 12;
	char response[12];
	memset(response, 0, sizeof(response));
	
	// send a break to get the sensor ready, or to abort a previous measurement.
	PORTA.DIRSET.reg = SDI_PIN_PINMASK;
	PORTA.OUTSET.reg = SDI_PIN_PINMASK;
	portable_delay_cycles(BREAK_DELAY_CYCLES);
	
	//try up to MAX NUMBER TRANSACTIONS to get a successful response.
	//loop will RETURN on success.
	while(tries--){
		transactionPacket->transactionStatus = SDI12_PerformTransaction(message, messageLen, response, responseLength);
		
		if(transactionPacket->transactionStatus == SDI12_STATUS_OK){
			//get time from response. only if it parsed successfully do we consider this a successful transaction
			if(SDI12_GetTimeFromResponse(response, &(transactionPacket->waitTime)) ){
				transactionPacket->numberOfValuesToReturn = SDI12_ParseNumValuesFromResponse(response, responseLength);
				return;
			}
		}
		else{
			//retries must wait between 16.67ms and 87ms
			delay_ms(20);
		}
	}
}

/*SDI12_GetSensedValues
After the sensor has had values requested with SDI12_RequestSensorReading, use this function to read the values as floats
The floats will be loaded into the outValues float array. NOTE!!!! outValues array MUST have a number of indices >= the
number of expected values from the transaction packet. Otherwise, Undefined operation or segfaults may occur.*/
bool SDI12_GetSensedValues(struct SDI_transactionPacket *transactionPacket, float *outValues){
	uint8_t dNumberChar = '0';
	uint8_t numValuesReceived = 0;
	const uint8_t messageLen = 4;
	char message[4] =
	{
		CharAddParity(transactionPacket->address),
		'D',
		CharAddParity(dNumberChar),
		'!'
	};
	const uint8_t responseLen = 36;
	char response[36];
	
	//load in NANs into all the outvalues,
	for(uint8_t counter = 0; counter < transactionPacket->numberOfValuesToReturn; counter++){
		outValues[counter] = NAN;
	}
	
	// send a break to get the sensor ready, or to abort a previous measurement.
	PORTA.DIRSET.reg = SDI_PIN_PINMASK;
	PORTA.OUTSET.reg = SDI_PIN_PINMASK;
	portable_delay_cycles(BREAK_DELAY_CYCLES);
	
	//TODO: make the num tries actually work, and exit out when needed.
	while(numValuesReceived < transactionPacket->numberOfValuesToReturn){
		uint8_t tries = SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS;
		while(tries){
			tries--;
			transactionPacket->transactionStatus = SDI12_PerformTransaction(message, messageLen, response, responseLen);
			if(transactionPacket->transactionStatus == SDI12_STATUS_OK){
				//star the float parsing after the address character
				char *floatParsePointer = &response[1];
				
				//first check that the query actually starts with a value.
				//if it doesn't, just try again. otherwise, let's parse them.
				if((*floatParsePointer == '-') || (*floatParsePointer == '+')){
					while((*floatParsePointer == '-') || (*floatParsePointer == '+')){
						//convert next value to float, save it in the outvalues, and move the pointer
						float parsedValue = strtof(floatParsePointer, &floatParsePointer);
						outValues[numValuesReceived++]  = parsedValue;
					}
					//stop trying for this D_! command, we succeeded.
					break;
				}
				else{
					//if the response was good, but didn't seem to have any valid data, return with what we have.
					return false;
				}
			}
			else{
				//retries must wait between 16.67ms and 87ms
				delay_ms(20);
			}
		}
		if(tries == 0){
			//these values couldn't be gathered in the give number of tries, so we'll call this sensor gathering a failure.
			return false;
		}
		message[1] = CharAddParity(message[1] +1);	//increment the index of the D_! command, thus asking for the next values on the next transaction.
	}
	return true;
}

/*charAddParity
takes a given character, and adds an even parity bit in the MSB.
*/
static char CharAddParity(char address){
	address &= 0x7F;	//make sure that the MSB starts cleared
	for(uint8_t bitmask = 1; bitmask != 0x80; bitmask <<= 1){
		if(address & bitmask){
			address ^= 0x80; //flip the parity bit
		}
	}
	return address;
}

/* SDI12_GetTimeFromResponse
parses the response from a _M! transaction to find the number of seconds that need to be delayed, and returns it in the outTime pointer.
if the 2nd, 3rd, or 4th characters aren't numbers, will return false, otherwise true.
Function also returns false if the response string is less than 4 characters long.
If the function returns false that outTime will be invalid.
response MUST be at least 4 characters long
*/
static bool SDI12_GetTimeFromResponse(const char response[], uint16_t *outTime){
	
	bool isProperlyFormatted = (response[1]>= '0' && response[1] <='9' &&
	response[2] >= '0' && response[2] <='9' &&
	response[3] >= '0' && response[3] <='9');
	
	*outTime = response[3] - '0';
	*outTime += (response[2]-'0') * 10;
	*outTime += (response[1]-'0') * 100;
	return isProperlyFormatted;
}

/*SDI12_ParseNumValuesFromResult
reads the result from a aM or an aIM transaction,
and returns the number of values the sensor can return.
returns 0 in event of failure.*/
static uint8_t SDI12_ParseNumValuesFromResponse(char responseBuffer[], uint8_t responseBufferLen){
	uint8_t numValuesSensed = 0;
	uint8_t valuesIndex = 4;
	
	while((responseBuffer[valuesIndex] >= '0') && (responseBuffer[valuesIndex] <='9') && (valuesIndex != responseBufferLen)){
		numValuesSensed *= 10;
		numValuesSensed += responseBuffer[valuesIndex] - '0';
		valuesIndex++;
	}
	return numValuesSensed;
}


#ifdef SDI12_UNIT_TESTING
static bool SDI12_TIME_FORMAT_UNIT_TEST(void){
	//success conditions
	uint16_t time;
	bool test[42];
	memset(test, false, sizeof(bool)*42);
	test[0] = (SDI12_GetTimeFromResponse("A001", &time));
	test[1] = (time == 1);
	test[2] = (SDI12_GetTimeFromResponse("z001", &time));
	test[3] = (time == 1);
	test[4] = (SDI12_GetTimeFromResponse("9001", &time));
	test[5] = (time == 1);
	test[6] = (SDI12_GetTimeFromResponse("A000", &time));
	test[7] = (time == 0);
	test[8] = (SDI12_GetTimeFromResponse("A999", &time));
	test[9] = (time == 999);
	test[10] = (SDI12_GetTimeFromResponse("A020", &time));
	test[11] = (time == 20);
	test[12] = (SDI12_GetTimeFromResponse("0000", &time));
	test[13] = (time == 0);
	test[14] = (SDI12_GetTimeFromResponse("A010", &time));
	test[15] = (time == 10);
	test[16] = (SDI12_GetTimeFromResponse("A001", &time));
	test[17] = (time == 1);
	test[18] = (SDI12_GetTimeFromResponse("A001", &time));
	test[19] = (time == 1);
	test[20] = (SDI12_GetTimeFromResponse("A231", &time));
	test[21] = (time == 231);
	
	//intentional failure conditions
	test[22] = (! SDI12_GetTimeFromResponse("A00a", &time));
	test[23] = (! SDI12_GetTimeFromResponse("A00A", &time));
	test[24] = (! SDI12_GetTimeFromResponse("A00/", &time));
	test[25] = (! SDI12_GetTimeFromResponse("A00Z", &time));
	test[26] = (! SDI12_GetTimeFromResponse("A00:", &time));
	test[27] = (! SDI12_GetTimeFromResponse("A0a0", &time));
	test[28] = (! SDI12_GetTimeFromResponse("A0A0", &time));
	test[29] = (! SDI12_GetTimeFromResponse("A0/0", &time));
	test[30] = (! SDI12_GetTimeFromResponse("A0Z0", &time));
	test[31] = (! SDI12_GetTimeFromResponse("A0:0", &time));
	test[32] = (! SDI12_GetTimeFromResponse("Aa00", &time));
	test[33] = (! SDI12_GetTimeFromResponse("AA00", &time));
	test[34] = (! SDI12_GetTimeFromResponse("A/00", &time));
	test[35] = (! SDI12_GetTimeFromResponse("AZ00", &time));
	test[36] = (! SDI12_GetTimeFromResponse("A:00", &time));
	test[37] = (! SDI12_GetTimeFromResponse("AB00", &time));
	test[38] = (! SDI12_GetTimeFromResponse("AB00A", &time));
	test[39] = (! SDI12_GetTimeFromResponse("AB00/", &time));
	test[40] = (! SDI12_GetTimeFromResponse("AB00Z", &time));
	test[41] = (! SDI12_GetTimeFromResponse("AB00:", &time));
	return true;
}
#endif