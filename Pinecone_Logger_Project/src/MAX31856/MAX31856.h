/*
 * MAX31856.h
 *
 * Created: 10/19/2016 4:01:09 PM
 *  Author: tim.anderson
 */ 


#ifndef MAX31856_H_
#define MAX31856_H_

#include <stdint.h>
#include "spi/spi.h"


//macros for each supported thermocouple type.
//the value given is what will be set into the MAX31856 register.
#define MAX31856_THERMOCOUPLE_TYPE_B	0x00
#define MAX31856_THERMOCOUPLE_TYPE_E	0x01
#define MAX31856_THERMOCOUPLE_TYPE_J	0x02
#define MAX31856_THERMOCOUPLE_TYPE_K	0x03
#define MAX31856_THERMOCOUPLE_TYPE_N	0x04
#define MAX31856_THERMOCOUPLE_TYPE_R	0x05
#define MAX31856_THERMOCOUPLE_TYPE_S	0x06
#define MAX31856_THERMOCOUPLE_TYPE_T	0x07

enum Max31856_Status {MAX31856_OKAY, MAX31856_CONNECTION_ERROR, MAX31856_TEMP_TOO_HIGH, MAX31856_TEMP_TOO_LOW, MAX31856_TC_NOT_CONNECTED, MAX31856_FAULT_VOLTAGE, MAX31856_SPI_ERROR};

/*Max31856ConfigureSPI
Sets the SPI configuration settings for the Max31856.
Since different devices may operate at different SPI modes,
character sizes, call this again if you had to change the SPI master settings.*/
void Max31856ConfigureSPI(struct spi_module *spiMasterModule, struct spi_slave_inst *spiSlaveInstance);

/*Max31856CheckWrittenRegister
	protyping helper function to write to a register, and check success by reading the register.*/
enum Max31856_Status Max31856CheckWrittenRegister(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst);

/*Max31856ConfigureRegisters
	communicates on the SPI to set the registers on the MAX31856 to the correct configuration*/
enum Max31856_Status Max31856ConfigureRegisters(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint32_t thermocoupleType);

/*Max31856RequestReading
	Sends a message over SPI to request a reading from the Max31856. A delay of up to 800ms will be required before the reading is prepared. 
	It is recommended to just enter standby mode for 1s while the sensor takes the measurement.*/
enum Max31856_Status Max31856RequestReading(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst);

/*Max31856GetTemp
	retrieves the temperature value from the Max31856,, loading it into the outTemp pointer*/
enum Max31856_Status Max31856GetTemp(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, float *outTemp);

#endif /* MAX31856_H_ */