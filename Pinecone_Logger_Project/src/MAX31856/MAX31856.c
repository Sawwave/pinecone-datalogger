/*
* MAX31856.c
*
* Created: 10/19/2016 4:01:00 PM
*  Author: tim.anderson
*/

#include "Max31856/Max31856.h"
#include "conf_board.h"
#include <math.h>

#define MAX31856_REG0_REQUEST_READING	0x40
#define MAX31856_REG0_VAL				0x00
#define MAX31856_REG1_VAL				0x40
#define MAX31856_WRITE_REGISTER_MASK	0x80
#define MAX31856_TEMP_START_REG			0x0C
#define MAX31856_TIMEOUT_COUNTER		200

static enum Max31856_Status Max31856WriteSpi(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint8_t xferBuffer[], const uint8_t xferBufferLen);

/*Max31856ConfigureSPI
Sets the SPI configuration settings for the Max31856.
Since different devices may operate at different SPI modes,
character sizes, call this again if you had to change the SPI master settings.*/
void Max31856ConfigureSPI(struct spi_module *spiMasterModule, struct spi_slave_inst *spiSlaveInstance){
	struct spi_config spiMasterConfig;
	spi_get_config_defaults(&spiMasterConfig);
	spiMasterConfig.mux_setting = MAX31856_SPI_PINMUX_SETTING;
	spiMasterConfig.pinmux_pad0 = MAX31856_SPI_PAD0;
	spiMasterConfig.pinmux_pad1 = MAX31856_SPI_PAD1;
	spiMasterConfig.pinmux_pad2 = MAX31856_SPI_PAD2;
	spiMasterConfig.pinmux_pad3 = MAX31856_SPI_PAD3;
	//xfers in MSB order by default.
	//since CPHA must be 1 for the Max31856, use xfer mode 1 or 3.
	spiMasterConfig.transfer_mode = SPI_TRANSFER_MODE_3;
	
	spi_init(spiMasterModule,  MAX31856_SPI_SERCOM_MODULE, &spiMasterConfig);
	
	struct spi_slave_inst_config slaveConfig;
	spi_slave_inst_get_config_defaults(&slaveConfig);
	slaveConfig.ss_pin = MAX31856_CS_PIN;
	spi_attach_slave(spiSlaveInstance, &slaveConfig);
}

/*Max31856CheckWrittenRegister
protyping helper function to write to a register, and check success by reading the register.*/
enum Max31856_Status Max31856CheckWrittenRegister(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst){
	uint16_t spiLockAttempts = 50000;
	while(spi_lock(spiMasterModule) == STATUS_BUSY){
		if(spiLockAttempts-- == 0){
			return MAX31856_CONNECTION_ERROR;
		}
	}
	spi_select_slave(spiMasterModule, slaveInst, true);
	
	//init the I/O buffers
	uint8_t sendBuffer[10] = {0, 0, 0, 0, 0,0,0,0,0,0};
	uint8_t receiveBuffer[10] = {0, 0, 0, 0, 0,0,0,0,0,0};
	enum status_code status = spi_transceive_buffer_wait(spiMasterModule, sendBuffer, receiveBuffer, 10);
	while(!spi_is_write_complete(spiMasterModule));
	spi_select_slave(spiMasterModule, slaveInst, false);
	spi_unlock(spiMasterModule);
	
	return status == STATUS_OK ?  MAX31856_OKAY : MAX31856_SPI_ERROR;
};

/*Max31856ConfigureRegisters
communicates on the SPI to set the registers on the MAX31856 to the correct configuration*/
enum Max31856_Status Max31856ConfigureRegisters(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint32_t thermocoupleType){
	uint8_t xferBuffer[3] = {MAX31856_WRITE_REGISTER_MASK, MAX31856_REG0_VAL, MAX31856_REG1_VAL | thermocoupleType};
	uint8_t xferBufferLen = 3;
	return Max31856WriteSpi(spiMasterModule, slaveInst, xferBuffer, xferBufferLen);
}

/*Max31856RequestReading
Sends a message over SPI to request a reading from the Max31856. A delay of up to 800ms will be required before the reading is prepared.
It is recommended to just enter standby mode for 1s while the sensor takes the measurement.*/
enum Max31856_Status Max31856RequestReading(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst){
	uint8_t xferBuffer[2] = {MAX31856_WRITE_REGISTER_MASK, MAX31856_REG0_REQUEST_READING};
	uint8_t xferBufferLen = 2;
	return Max31856WriteSpi(spiMasterModule, slaveInst, xferBuffer, xferBufferLen);
}

/*Max31856GetTemp
retrieves the temperature value from the Max31856,, loading it into the outTemp pointer*/
enum Max31856_Status Max31856GetTemp(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, float *outTemp){
	uint8_t timeout = 200;
	//wait until the spi bus is free
	while(spi_is_syncing(spiMasterModule)){
		if(!timeout--) {
			*outTemp = NAN;
			return MAX31856_SPI_ERROR;
		}
	}
	
	enum status_code status;
	uint16_t spiLockAttempts = 50000;
	while(spi_lock(spiMasterModule) == STATUS_BUSY){
		if(spiLockAttempts-- == 0){
			*outTemp = NAN;
			return MAX31856_CONNECTION_ERROR;
		}
	}
	spi_select_slave(spiMasterModule, slaveInst, true);
	
	//init the I/O buffers
	uint8_t sendBuffer[5] = {MAX31856_TEMP_START_REG, 0, 0, 0, 0};
	uint8_t receiveBuffer[5] = {0, 0, 0, 0, 0};
	status = spi_transceive_buffer_wait(spiMasterModule, sendBuffer, receiveBuffer, 5);
	
	spi_select_slave(spiMasterModule, slaveInst, false);
	spi_unlock(spiMasterModule);
	//if we got the temp, translate it to a double.
	if(status == STATUS_OK){
		uint32_t intTemp = ((uint32_t)receiveBuffer[1] << 16) | ((uint32_t)receiveBuffer[2] << 8) | ((uint32_t)receiveBuffer[3]);
		//get the temp sign, and cast it away.
		register uint32_t sign = receiveBuffer[1] >> 7;
		//mask away the highest byte, and the sign bit
		intTemp >>= 5;

		*outTemp = (double) intTemp;
		*outTemp /= 128;
		if(sign){
			*outTemp *= -1;
		}
		
		//check the fault byte
		uint8_t faultByte = receiveBuffer[4];
		if(faultByte & 0x01){
			*outTemp = NAN;
			return MAX31856_TC_NOT_CONNECTED;
		}
		else if (faultByte & 0x02){
			*outTemp = NAN;
			return MAX31856_FAULT_VOLTAGE;
		}
		else if (faultByte & 0x14){
			*outTemp = NAN;
			return MAX31856_TEMP_TOO_LOW;
		}
		else if (faultByte & 0x28){
			*outTemp = NAN;
			return MAX31856_TEMP_TOO_HIGH;
		}
		else return MAX31856_OKAY;
	}
	*outTemp = NAN;
	return MAX31856_SPI_ERROR;
}

/*Max31856WriteSpi
Using on-chip SPI hardware, performs an SPI transaction with the spiMasterModule.
function will write bytes equal to xferBufferLen from xferBuffer to the SPI bus.
returns MAX31856_OKAY on success, MAX31856_CONNECTION_ERROR on spi error.
*/
static enum Max31856_Status Max31856WriteSpi(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint8_t xferBuffer[], const uint8_t xferBufferLen){
	while(spi_is_syncing(spiMasterModule));
	uint16_t spiLockAttempts = 50000;
	while(spi_lock(spiMasterModule) == STATUS_BUSY){
		if(spiLockAttempts-- == 0){
			return MAX31856_CONNECTION_ERROR;
		}
	}
	//wait until the spi bus is free
	while(spi_is_syncing(spiMasterModule));
	while(!spi_is_ready_to_write(spiMasterModule));
	
	spi_select_slave(spiMasterModule, slaveInst, true);
	//write the default values to the first 2 registers to the amplifier
	status = spi_write_buffer_wait(spiMasterModule, xferBuffer, xferBufferLen);
	while(!spi_is_write_complete(spiMasterModule));
	spi_select_slave(spiMasterModule, slaveInst, false);
	spi_unlock(spiMasterModule);
	if(status == STATUS_OK){
		return MAX31856_OKAY;
	}
	return MAX31856_CONNECTION_ERROR;
}