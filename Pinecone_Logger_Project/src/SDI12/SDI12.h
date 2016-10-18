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




#endif /* SDI12_H_ */