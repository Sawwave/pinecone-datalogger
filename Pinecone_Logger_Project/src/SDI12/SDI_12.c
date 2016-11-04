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

#define SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS 3

#define MARKING_DELAY_CYCLES 14000			// >= 12ms marking length
#define PARITY_I			201				//'I' is 73, 73+128= 201

#define SPACING_8330_DELAY_CYCLES 9520 		//8330us spacing delay
#define BIT_TIMING_DELAY_CYCLES 928			//833us bit timing			
#define BIT_TIMING_HALF_DELAY_CYCLES 400	//416.5us to get halfway into reading a bit 


char charAddParity(char address);
uint8_t SDI12_ParseNumValuesFromResult(char outBuffer[], uint8_t outBufferLen);
bool SDI12_GetTimeFromResponse(const char *response, uint16_t *outTime);
bool SDI12_TIME_FORMAT_UNIT_TEST(void);

//TODO: move this out and set up with rest of pins
void SDI12_Setup(void){
	PORTA.DIRSET.reg = SDI_PIN_PINMASK;
}

/*SDI12_RequestSensorReading
communicates with the sensor at the given address on the SDI12 bus, and requests a reading (M! command).
Will load SDI12_STATUS_OK into the packet on success, SDI12_BAD_RESPONSE on failure.
*/
void SDI12_RequestSensorReading(struct SDI_transactionPacket *transactionPacket){
	uint8_t tries = SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS;
	const uint8_t messageLen = 3;
	char message[3] = { charAddParity(transactionPacket->address), 'M', '!'};
	const uint8_t responseLength = 12;
	char response[12];
	memset(response, 0, sizeof(response));
	
	//try up to MAX NUMBER TRANSACTIONS to get a successful response.
	//loop will RETURN on success.
	while(tries--){
		transactionPacket->transactionStatus = SDI12_PerformTransaction(message, messageLen, response, responseLength);
		
		if(transactionPacket->transactionStatus == SDI12_STATUS_OK){
			//get time from response. only if it parsed successfully do we consider this a successful transaction
			if( SDI12_GetTimeFromResponse(response, &(transactionPacket->waitTime)) ){
				transactionPacket->numberOfValuesToReturn = SDI12_ParseNumValuesFromResult(response, responseLength);
			}
			else
			transactionPacket->transactionStatus = SDI12_BAD_RESPONSE;
		}
		else{
			transactionPacket->transactionStatus = SDI12_BAD_RESPONSE;
		}
	}
}

/*SDI12_GetSensedValues
After the sensor has had values requested with SDI12_RequestSensorReading, use this function to read the values as floats
The floats will be loaded into the outValues float array. NOTE!!!! outValues array MUST have a number of indices >= the
number of expected values from the transaction packet. Otherwise, Undefined operation or segfaults may occur.*/
bool SDI12_GetSensedValues(struct SDI_transactionPacket *transactionPacket, float *outValues){

	uint8_t numValuesExpected = transactionPacket->numberOfValuesToReturn;
	uint8_t dNumberChar = '0';
	uint8_t numValuesReceived = 0;
	const uint8_t messageLen = 4;
	char message[4] =
	{
		charAddParity(transactionPacket->address),
		'D',
		charAddParity(dNumberChar),
		'!'
	};
	char response[36];
	const uint8_t responseLen = 36;
	
	//load in NANs into all the outvalues, 
	for(uint8_t counter = 0; counter < numValuesExpected; counter++){
		outValues[counter] = NAN;
	}
	
	while(numValuesReceived < numValuesExpected){
		uint8_t tries = SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS;
		while(tries--){
			transactionPacket->transactionStatus = SDI12_PerformTransaction(message, messageLen, response, responseLen);
			if(transactionPacket->transactionStatus == SDI12_STATUS_OK){
				//star the float parsing after the address character
				char *floatParsePointer = &response[1];
				
				//first check that the query will actually get a value. if it won't, we should return false
				//instead of potentially looping forever trying to get values that don't exist
				if((*floatParsePointer != '-') && (*floatParsePointer != '+')){
					return false;
				}
				
				while((*floatParsePointer == '-') || (*floatParsePointer == '+')){
					//convert next value to float, save it in the outvalues, and move the pointer
					float parsedValue = strtof(floatParsePointer, &floatParsePointer);
					outValues[numValuesReceived++]  = parsedValue;
				}
				//stop trying for this D_! command, we succeeded.
				break;
			}
			//on failure
		}
		if(tries == 0){
			//these values couldn't be gathered in the give number of tries, so we'll call this sensor gathering a failure.
			return false;
		}
		message[1]++;	//increment the index of the D_! command, thus asking for the next values on the next transaction.
	}
	return true;
}


/* SDI12_GetTimeFromResponse
parses the response from a _M! transaction to find the number of seconds that need to be delayed, and returns it in the outTime pointer.
if the 2nd, 3rd, or 4th characters aren't numbers, will return false, otherwise true.
Function also returns false if the response string is less than 4 characters long.
If the function returns false that outTime will be invalid.

response MUST be at least 4 characters long
*/
bool SDI12_GetTimeFromResponse(const char response[], uint16_t *outTime){
	
	bool isProperlyFormatted = (response[1]>= '0' && response[1] <='9' &&
	response[2] >= '0' && response[2] <='9' &&
	response[3] >= '0' && response[3] <='9');
	
	*outTime = response[3] - '0';
	*outTime += (response[2]-'0') * 10;
	*outTime += (response[1]-'0') * 100;
	return isProperlyFormatted;
}

/*SDI12_PerformTransaction
Attempts to send the given message on the SDI-12 bus, and wait for a response.
Returns a SDI12_ReturnCode. If it returns SDI12_Status_OK, the transaction was successful,
and the response is valid.

When using this function, if it fails, attempt a number of times defined by SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS.
*/
enum SDI12_ReturnCode  SDI12_PerformTransaction(const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen){
	//clear out the output buffer
	memset(outBuffer, 0, sizeof(char)*outBufferLen);

	//disable all interrupts, since they can mess up the bit timings.
	system_interrupt_enter_critical_section();
	
	//set for 13 millis, ish
	PORTA.OUTSET.reg = SDI_PIN_PINMASK;
	portable_delay_cycles(MARKING_DELAY_CYCLES);
	
	//clear for 8330 micros
	PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
	portable_delay_cycles(SPACING_8330_DELAY_CYCLES);
	for(uint8_t byteNumber = 0; byteNumber < messageLen; byteNumber++){
		//start bit (always high)
		PORTA.OUTSET.reg = SDI_PIN_PINMASK;
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		for(uint8_t bitmask = 1; bitmask != 0; bitmask <<= 1){
			if((message[byteNumber] & bitmask) != 0){
				//0 on this bit, so send HIGH
				PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
			}
			else{
				PORTA.OUTSET.reg = SDI_PIN_PINMASK;
			}
			portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		}
		
		//end bit (always low)
		PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
	}
	
	//set SDI pin to input by changing DIR and setting INEN in WRCONFIG
	PORTA.DIRCLR.reg = SDI_PIN_PINMASK;
	#if SDI_PIN_PINMASK < (1 << 16)
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | SDI_PIN_PINMASK;
	#else
	PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (SDI_PIN_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
	#endif
	
	//wait for the timeout or for the data line to go LOW. A timeout value of 8000 gives us roughly >20ms until timeout
	uint16_t timeout = 8000;
	do{
		portable_delay_cycles(5);
	} while( (timeout--) && ((PORTA.IN.reg & SDI_PIN_PINMASK) != 0) );
	if(timeout == 0){
		//turn pin back into OUTPUT, and disable INEN
		PORTA.DIRSET.reg = SDI_PIN_PINMASK;
		PORTA.OUTCLR.reg = SDI_PIN_PINMASK;
		#if SDI_PIN_PINMASK < (1 << 16)
		PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG |  SDI_PIN_PINMASK;
		#else
		PORTA.WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (SDI_PIN_PINMASK >> 16) | PORT_WRCONFIG_HWSEL;
		#endif
		return SDI12_TRANSACTION_TIMEOUT;
	}
	
	uint8_t byteNumber = 0;
	do{
		//we've received the signal, now delay partway into the bits to read them
		portable_delay_cycles(BIT_TIMING_HALF_DELAY_CYCLES);
		
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		for(uint8_t bitmask = 1; bitmask != 0x80; bitmask <<= 1){
			if((PORTA.IN.reg & SDI_PIN_PINMASK) == 0){
				outBuffer[byteNumber] |= bitmask;
			}
			portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		}
		//delay through the parity bit and end bit
		portable_delay_cycles(BIT_TIMING_DELAY_CYCLES);
		timeout = 255;
		//loop-delay through the end bit (LOW) to realign with the byte frame
		do{
			portable_delay_cycles(5);
		}while( ( (PORTA.IN.reg & SDI_PIN_PINMASK) == 0) && (timeout--) != 0);
		byteNumber++;
	} while(((PORTA.IN.reg & SDI_PIN_PINMASK) != 0)		//we got the start bit!
	&& (byteNumber < (outBufferLen - 1))						//the buffer overflowed (overflew?)
	&& (outBuffer[byteNumber-1] != 10));						// the last byte was Line Feed

	//we're done receiving the message, so we can leave the interrupt critical section.
	//while we're at it, we can set the pin back to output, and put it in powersave mode
	system_interrupt_leave_critical_section();
	
	//turn pin back into OUTPUT, and disable INEN
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

/*SDI12_GetNumReadingsFromSensorMetadata
	queries the sensor at the given address with a _IM! command to retrieve its metadata.
	From this, the function parses the number of values to expect, and returns that value.*/
uint8_t SDI12_GetNumReadingsFromSensorMetadata(const char address){
	//ready the metadata command
	char message[4] = { charAddParity(address), PARITY_I,  'M', '!'};
	const uint8_t messageLen = 4;
	char outBuffer[12];
	uint8_t outBufferLen = 12;
	
	uint8_t tries = SDI12_MAX_NUMBER_TRANSACTION_ATTEMPTS;
	while(tries--){
		memset(outBuffer, 0, outBufferLen);

		SDI12_PerformTransaction(message, messageLen, outBuffer, outBufferLen);
		uint8_t numValues = SDI12_ParseNumValuesFromResult(outBuffer, outBufferLen);
		if(numValues != 0)
		{
			return SDI12_ParseNumValuesFromResult(outBuffer, outBufferLen);
		}
	}
	//if none of the tries succeeded, just return 0 in failure
	return 0;
}

/*SDI12_ParseNumValuesFromResult
reads the result from a aM or an aIM transaction,
and returns the number of values the sensor can return.
returns 0 in event of failure.*/
uint8_t SDI12_ParseNumValuesFromResult(char responseBuffer[], uint8_t responseBufferLen){
	uint8_t numValuesSensed = 0;
	uint8_t valuesIndex = 4;
	
	while((responseBuffer[valuesIndex] >= '0') && (responseBuffer[valuesIndex] <='9') && (valuesIndex != responseBufferLen)){
		numValuesSensed *= 10;
		numValuesSensed += responseBuffer[valuesIndex];
		valuesIndex++;
	}
	return numValuesSensed;
}

/*charAddParity
	takes a given character, and adds an even parity bit in the MSB.
*/
char charAddParity(char address){
	address &= 0x7F;
	for(uint8_t bitmask = 1; bitmask != 0x80; bitmask <<= 1){
		if(address & bitmask){
			//flip the parity bit
			address ^= 0x80;
		}
	}
	return address;
}


bool SDI12_TIME_FORMAT_UNIT_TEST(void){
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