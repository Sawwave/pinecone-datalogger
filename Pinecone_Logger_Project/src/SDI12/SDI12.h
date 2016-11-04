/*
 * SDI12.h
 *
 * Created: 10/18/2016 4:26:07 PM
 *  Author: tim.anderson
 */ 


#ifndef SDI12_H_
#define SDI12_H_


enum SDI12_ReturnCode {SDI12_STATUS_OK, SDI12_TRANSACTION_TIMEOUT, SDI12_BAD_RESPONSE, SDI12_TRANSACTION_FAILURE};

struct SDI_transactionPacket{
	char address;
	uint16_t waitTime;
	uint8_t numberOfValuesToReturn;
	enum SDI12_ReturnCode transactionStatus;
};

/*SDI12_Setup
	Configures the Data pin for the SDI-12 bus.
	TODO: refactor this out and do this on board setup*/
void SDI12_Setup(void);

/*SDI12 PerformTransaction
	The major workhorse of the SDI-12 library. performs a full transaction with an SDI-12 sensor.
	In this transaction, the data logger sends a message addressed to a particular sensor, and the sensor response with its own message.
	outBuffer should be large enough to accommodate the expected response. For example, an _M! message should be somewhere in the range of
	12-16 characters to be safe.
	Returns SDI12_STATUS_OK on success, but may SDI12_TRANSACTION_TIMEOUT, SDI12_BAD_RESPONSE, or simply SDI12_TRANSACTION_FAILURE.*/
enum SDI12_ReturnCode SDI12_PerformTransaction(const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen);

/*SDI12_RequestSensorReading
	 takes a Transaction Packet struct with the address loaded into it, and makes an _M! transaction.
	 Upon completion, loads the return code, wait time, and number of values expected into the packet for further computation.*/
void SDI12_RequestSensorReading(struct SDI_transactionPacket *transactionPacket);

/*SDI12_GetSensedValues
	Takes a transaction packet, and a float array to load the values into.
	Performs a transaction with the sensor to retrieve the sensed values. Will return true on success, false on failure.*/
bool SDI12_GetSensedValues(struct SDI_transactionPacket *transactionPacket, float *outValues);

/*SDI12_GetNumReadingsFromSensorMetadata
	queries the sensor at the given address with a _IM! command to retrieve its metadata.
	From this, the function parses the number of values to expect, and returns that value.*/
uint8_t SDI12_GetNumReadingsFromSensorMetadata(const char address);

#endif /* SDI12_H_ */