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

#define MAX31856_THERMOCOUPLE_TYPE_B	0x00
#define MAX31856_THERMOCOUPLE_TYPE_E	0x01
#define MAX31856_THERMOCOUPLE_TYPE_J	0x02
#define MAX31856_THERMOCOUPLE_TYPE_K	0x03
#define MAX31856_THERMOCOUPLE_TYPE_N	0x04
#define MAX31856_THERMOCOUPLE_TYPE_R	0x05
#define MAX31856_THERMOCOUPLE_TYPE_S	0x06
#define MAX31856_THERMOCOUPLE_TYPE_T	0x07

enum Max31856_Status {MAX31856_OKAY, MAX31856_CONNECTION_ERROR, MAX31856_TEMP_TOO_HIGH, MAX31856_TEMP_TOO_LOW, MAX31856_TC_NOT_CONNECTED, MAX31856_FAULT_VOLTAGE, MAX31856_SPI_ERROR};

void Max31856ConfigureSPI(struct spi_module *spiMasterModule, struct spi_slave_inst *spiSlaveInstance);

enum Max31856_Status Max31856CheckWrittenRegister(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst);

enum Max31856_Status Max31856ConfigureRegisters(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, uint32_t thermocoupleType);

enum Max31856_Status Max31856RequestReading(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst);

enum Max31856_Status Max31856GetTemp(struct spi_module *spiMasterModule, struct spi_slave_inst *slaveInst, double *outTemp);

#endif /* MAX31856_H_ */