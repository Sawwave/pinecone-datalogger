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

/*SDI12 PerformTransaction
The major workhorse of the SDI-12 library. performs a full transaction with an SDI-12 sensor.
In this transaction, the data logger sends a message addressed to a particular sensor, and the sensor response with its own message.
outBuffer should be large enough to accommodate the expected response. For example, an _M! message should be somewhere in the range of
12-16 characters to be safe.
Returns SDI12_STATUS_OK on success, but may SDI12_TRANSACTION_TIMEOUT, SDI12_BAD_RESPONSE, or simply SDI12_TRANSACTION_FAILURE.*/
enum SDI12_ReturnCode SDI12_PerformTransaction(const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen);

/*SDI12_RequestSensorReading
communicates with the sensor at the given address on the SDI12 bus, and requests a reading (M! command).
Will load SDI12_STATUS_OK into the packet on success, SDI12_BAD_RESPONSE on failure.
*/
bool SDI12_RequestSensorReading(struct SDI_transactionPacket *transactionPacket);

/*SDI12_GetSensedValues
After the sensor has had values requested with SDI12_RequestSensorReading, use this function to read the values as FixedPoint32's
The FixedPoint32's will be loaded into the outValues array. NOTE!!!! outValues array MUST have a number of indices >= the
number of expected values from the transaction packet. Otherwise, Undefined operation or bad juju may occur.*/
bool SDI12_GetSensedValues(struct SDI_transactionPacket *transactionPacket, struct FixedPoint32 *outValues);

enum SDI12_ReturnCode SDI12_PerformTransactionWithRetries(const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen);
#endif /* SDI12_H_ */