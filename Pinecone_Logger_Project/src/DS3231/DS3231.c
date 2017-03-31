/*
* DS3231.c
*
* Created: 3/9/2017 2:29:18 PM
*  Author: tim.anderson
*/
#include "DS3231.h"

#define DS3231_SECONDS_REG_ADDRESS			0x00
#define DS3231_MINUTES_REG_ADDRESS			0x01
#define DS3231_HOUR_REG_ADDRESS				0x02
#define DS3231_DAY_OF_MONTH_REG_ADDRESS		0x03
#define DS3231_DAY_REG_ADDRESS				0x04
#define DS3231_MONTH_REG_ADDRESSS			0x05
#define DS3231_YEAR_REG_ADDRESS				0x06

#define DS3231_DATE_TIME_START_REG			0x00
#define DS3231_ALARM2_START_REG				0x0B

#define DS3231_CONTROL_REG_ADDRESS			0x0E
#define DS3231_STATUS_REG_ADDRESS			0x0F
#define DS3231_AGING_OFFSET_REG_ADDRESS		0x10
#define DS3231_TEMP1_REG_ADDRESS			0x11
#define DS3231_TEMP2_REG_ADDRESS			0x12

#define DS3231_12_HOUR_PINMASK				1 << 6
#define DS3231_MONTH_CENTURY_PINMASK		1 << 7
#define DS3231_ALARM_SUPRESS_BITMASK		1 << 7

#define DS3231_CTRL_CONVERT_TEMP_BITMASK	1 << 6
#define DS3231_CTRL_ALARM_INTERRUPT_BITMASK	1 << 2
#define DS3231_CTRL_ALARM_2_ENABLE_BITMASK	1 << 1
#define DS3231_CTRL_ALARM_1_ENABLE_BITMASK	1 << 0

#define DS3231_STATUS_OSCIL_STOP_BITMASK	1 << 7
#define DS3231_STATUS_ALARM2_ON_BITMASK		1 << 2

#define DS3231_MAX_NUM_I2C_ATTEMPTS			32

#define DS3231_SLAVE_ADDRESS				0x68 //binary code 1101000

//time buffer we will write into looks like this "mm/dd/2yyy,hh:mm:ss"
#define DS3231_TIME_BUFFER_MONTH_INDEX		0
#define DS3231_TIME_BUFFER_DAY_INDEX		3
#define DS3231_TIME_BUFFER_YEAR_INDEX		8
#define DS3231_TIME_BUFFER_HOUR_INDEX		11
#define DS3231_TIME_BUFFER_MINUTE_INDEX		14
#define DS3231_TIME_BUFFER_SECOND_INDEX		17
#define DS3231_TIME_BUFFER_CENTURY_INDEX	7


static uint8_t intToBCD(const uint8_t intValue){
	return (intValue % 10) | ((intValue / 10) << 4);
}

static void writeBCD_regToString(char stringPtr[2], const uint8_t regValue){
	stringPtr[0] = (regValue >> 4) + '0';
	stringPtr[1] = (regValue & 0x0F) + '0';
}

static uint8_t charArrayToBCD(const char tensDigitPtr[2]){
	uint8_t tensChar = tensDigitPtr[0];
	uint8_t onesChar = tensDigitPtr[1];
	return (onesChar -'0') | ((tensChar - '0') << 4);
}

static uint8_t charArrayToInt(const char *str){
	return ((str[0] - '0') * 10) + (str[1] - '0');
}

void DS3231_init_i2c(struct i2c_master_module *i2cMasterModule){
	struct i2c_master_config i2cConfig;
	i2c_master_get_config_defaults(&i2cConfig);
	i2cConfig.pinmux_pad0 = DS3231_SERCOM_SDA_PIN;
	i2cConfig.pinmux_pad1 = DS3231_SERCOM_SCL_PIN;
	i2c_master_init(i2cMasterModule, DS3231_SERCOM_MODULE, &i2cConfig);
}

void DS3231_setTimeFromString(struct i2c_master_module *i2cMasterModule, const char timeBuffer[19]){
	//find the character for century c in (2cXX) year format, and turn it into a bit 7 set or clear
	uint8_t centuryBit = (timeBuffer[DS3231_TIME_BUFFER_CENTURY_INDEX] - '0') << 7;
	uint8_t data[8];
	data[0] = DS3231_SECONDS_REG_ADDRESS;
	data[1] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_SECOND_INDEX]);
	data[2] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_MINUTE_INDEX]);
	data[3] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_HOUR_INDEX]);
	data[4] = 1;				//we don't care about day of week, so just set it to monday.
	data[5] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_DAY_INDEX]);
	data[6] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_MONTH_INDEX]) | centuryBit;
	data[7] = charArrayToBCD(&timeBuffer[DS3231_TIME_BUFFER_YEAR_INDEX]);
	
	//send the packet
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data = data;
	packet.data_length = 8;
	packet.high_speed = false;
	packet.ten_bit_address = false;
	
	i2c_master_enable(i2cMasterModule);
	
	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
}



void DS3231_getTimeToString(struct i2c_master_module *i2cMasterModule, char timeBuffer[19]){
	//set up the write to re-zero the address register
	uint8_t data[18];
	data[0] = DS3231_SECONDS_REG_ADDRESS;
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data = data;
	packet.data_length = 1;
	packet.high_speed = false;
	packet.ten_bit_address = false;
	
	//send the first packet to set the register address to read from to 0.
	i2c_master_enable(i2cMasterModule);
	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	//now, re-use the same data packet and buffer for the read from the date-time registers.
	packet.data_length = 18;
	i2c_master_read_packet_wait(i2cMasterModule, &packet);
	i2c_master_disable(i2cMasterModule);
	
	//save the century bit, and strip it off the month.
	uint8_t centuryChar = (data[5] >> 7) + '0';
	data[5] &= 0x7F;
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_SECOND_INDEX], data[0]);
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_MINUTE_INDEX], data[1]);
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_HOUR_INDEX], data[2]);
	//data[3] is day of week, and we don't care about it, so skip it.
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_DAY_INDEX], data[4]);
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_MONTH_INDEX], data[5]);
	//write the century Char
	timeBuffer[DS3231_TIME_BUFFER_CENTURY_INDEX] = centuryChar;
	writeBCD_regToString(&timeBuffer[DS3231_TIME_BUFFER_YEAR_INDEX], data[6]);
}

void DS3231_setAlarm(struct i2c_master_module *i2cMasterModule, const struct Ds3231_alarmTime *alarmTime){
	uint8_t sendBuffer[6];
	sendBuffer[0] = DS3231_ALARM2_START_REG;
	//if alarm time is a null pointer, that means we're starting on the next hour (next time minutes is 00).
	if(alarmTime){
		sendBuffer[1] = intToBCD(alarmTime->minutes) | DS3231_ALARM_SUPRESS_BITMASK;
		sendBuffer[2] = intToBCD(alarmTime->hours) | DS3231_ALARM_SUPRESS_BITMASK;
	}
	else{
		sendBuffer[1] = 0;
		sendBuffer[2] = DS3231_ALARM_SUPRESS_BITMASK;
	}
	sendBuffer[3] = DS3231_ALARM_SUPRESS_BITMASK;
	sendBuffer[4] =	DS3231_CTRL_ALARM_INTERRUPT_BITMASK | DS3231_CTRL_ALARM_2_ENABLE_BITMASK;
	sendBuffer[5] = 0;


	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data_length = 6;
	packet.data = sendBuffer;
	packet.high_speed = false;
	packet.ten_bit_address = false;

	i2c_master_enable(i2cMasterModule);

	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
}

void DS3231_disableAlarm(struct i2c_master_module *i2cMasterModule){
	uint8_t sendBuffer[2];
	sendBuffer[0] = DS3231_CONTROL_REG_ADDRESS;
	sendBuffer[1] = DS3231_CTRL_ALARM_INTERRUPT_BITMASK;
	
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data_length = 2;
	packet.data = sendBuffer;
	packet.high_speed = false;
	packet.ten_bit_address = false;
	
	i2c_master_enable(i2cMasterModule);

	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
}


//set the next DS3231 alarm
void DS3231_setAlarmFromTime(struct i2c_master_module *i2cMasterModule, const uint16_t loggingInterval, const char timeBuffer[19]){
	
	//TODO: perform better optomization here
	//calculate the alarm minute and hour
	uint32_t minutesIntoDay =charArrayToInt(&timeBuffer[DS3231_TIME_BUFFER_MINUTE_INDEX]) + 
	(charArrayToInt(&timeBuffer[DS3231_TIME_BUFFER_HOUR_INDEX + 1])*60)+
	loggingInterval;
	
	minutesIntoDay %= (24*60);
	
	
	uint8_t sendBuffer[6];
	sendBuffer[0] = DS3231_ALARM2_START_REG;
	sendBuffer[1] = intToBCD(minutesIntoDay % 60)| DS3231_ALARM_SUPRESS_BITMASK;
	sendBuffer[2] = intToBCD(minutesIntoDay / 60) | DS3231_ALARM_SUPRESS_BITMASK;
	sendBuffer[3] = DS3231_ALARM_SUPRESS_BITMASK;
	sendBuffer[4] =	DS3231_CTRL_ALARM_INTERRUPT_BITMASK | DS3231_CTRL_ALARM_2_ENABLE_BITMASK;
	sendBuffer[5] = 0;


	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data_length = 6;
	packet.data = sendBuffer;
	packet.high_speed = false;
	packet.ten_bit_address = false;

	i2c_master_enable(i2cMasterModule);

	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
}