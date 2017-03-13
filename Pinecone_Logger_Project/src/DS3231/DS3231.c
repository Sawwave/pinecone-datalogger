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
#define DS3231_CTRL_ALARM_2_ENALBE_BITMASK	1 << 1

#define DS3231_STATUS_OSCIL_STOP_BITMASK	1 << 7
#define DS3231_STATUS_ALARM2_ON_BITMASK		1 << 2

#define DS3231_MAX_NUM_I2C_ATTEMPTS			32

#define DS3231_SLAVE_ADDRESS			0x68 //binary code 1101000

static uint8_t DS3231_BCD_ToInt(const uint8_t registerValue){
	return (registerValue & 0xF) + (10 * (registerValue >> 4));
}
static uint8_t DS3231_intToBCD(const uint8_t inputInt){
	return inputInt % 10 | ((inputInt / 10) << 4);
}

void DS3231_init(struct i2c_master_module *i2cMasterModule){
	struct i2c_master_config i2cConfig;
	i2c_master_get_config_defaults(&i2cConfig);
	i2cConfig.pinmux_pad0 = DS3231_SERCOM_SDA_PIN;
	i2cConfig.pinmux_pad1 = DS3231_SERCOM_SCL_PIN;	
	i2c_master_init(i2cMasterModule, DS3231_SERCOM_MODULE, &i2cConfig);
}

void DS3231_setTime(struct i2c_master_module *i2cMasterModule, const struct dateTime *dateTime){
	i2c_master_enable(i2cMasterModule);
	
	uint8_t data[8];
	data[0] = DS3231_SECONDS_REG_ADDRESS;
	data[1] = DS3231_intToBCD(dateTime->seconds);
	data[2] = DS3231_intToBCD(dateTime->minutes);
	data[3] = DS3231_intToBCD(dateTime->hours);
	data[4] = 1;				//we don't care about day of week
	data[5] = DS3231_intToBCD(dateTime->date);
	
	uint8_t centuryBit = 0;
	uint8_t yearInt = dateTime->year;
	if(dateTime->year > 99){
		centuryBit = 0x80;
		yearInt = yearInt - 100;
		
	}
	
	data[6] = DS3231_intToBCD(dateTime->month) | centuryBit;
	data[7] = DS3231_intToBCD(yearInt);
	
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data = data;
	packet.data_length = 7;
	packet.high_speed = false;
	packet.ten_bit_address = false;
	
	i2c_master_enable(i2cMasterModule);
	
	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
}

void DS3231_getTime(struct i2c_master_module *i2cMasterModule, struct dateTime *dateTime){
	//set up the write to re-zero the address register
	uint8_t data[7];
	data[0] = DS3231_SECONDS_REG_ADDRESS;
	
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data = data;
	packet.data_length = 1;
	packet.high_speed = false;
	packet.ten_bit_address = false;
	
	i2c_master_enable(i2cMasterModule);
	
	i2c_master_write_packet_wait(i2cMasterModule, &packet);
	
	//now, re-use the same data packet and buffer for the read from the datetime registers.
	packet.data_length = 6;
	i2c_master_read_packet_wait(i2cMasterModule, &packet);
	
	i2c_master_disable(i2cMasterModule);
	
	//parse out the read buffer into the datetime struct
	dateTime->seconds = DS3231_BCD_ToInt(data[0]);
	dateTime->minutes = DS3231_BCD_ToInt(data[1]);
	dateTime->hours = DS3231_BCD_ToInt(data[2]);
	dateTime->date = DS3231_BCD_ToInt(data[4]);
	dateTime->month = DS3231_BCD_ToInt(data[5]& 0x7F);	//ignore the century bit (bit 7)
	dateTime->year = DS3231_BCD_ToInt(data[6])+ (100 * (data[5]>>7));	//add in the century bit from month reg, resulting in range of 0-199.
}

void DS3231_setAlarm(struct i2c_master_module *i2cMasterModule, const struct Ds3231_alarmTime *alarmTime){
	uint8_t sendBuffer[5] = {DS3231_ALARM2_START_REG,
		DS3231_intToBCD(alarmTime->minutes),
		DS3231_intToBCD(alarmTime->hours),
		DS3231_ALARM_SUPRESS_BITMASK,
		DS3231_CTRL_ALARM_INTERRUPT_BITMASK | DS3231_CTRL_ALARM_2_ENALBE_BITMASK
	};
	
	struct i2c_master_packet packet;
	packet.address = DS3231_SLAVE_ADDRESS;
	packet.data_length = 5;
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
	sendBuffer[1] = 0;
	
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