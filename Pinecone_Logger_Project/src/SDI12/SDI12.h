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


void SDI12_Setup(void);
enum SDI12_ReturnCode SDI12_PerformTransaction(const char *message, const uint8_t messageLen, char *outBuffer, const uint8_t outBufferLen);
void SDI12_RequestSensorReading(struct SDI_transactionPacket *transactionPacket);
bool SDI12_GetSensedValues(struct SDI_transactionPacket *transactionPacket, float *outValues);
bool SDI12_GetTimeFromResponse(const char *response, uint16_t *outTime);
uint8_t SDI12_GetNumReadingsFromSensorMetadata(char address);

#endif /* SDI12_H_ */