/**
* \file
*
* \brief User board configuration template
*
* Copyright (C) 2012-2015 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel micro-controller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/
/*
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/
#include "MAX31856/MAX31856.h"
#include <asf.h>
#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define SDI12_MAX_SUPPORTED_SENSORS 25


struct LoggerConfig{
	char SDI12_SensorAddresses[SDI12_MAX_SUPPORTED_SENSORS + 1];	//extra char on the end for ease of f_gets function.
	uint8_t SDI12_SensorNumValues[SDI12_MAX_SUPPORTED_SENSORS];
	uint8_t numSdiSensors;
	uint16_t loggingInterval;
	bool logImmediately;
	bool checkFileIntegrity;
};


#define SD_VOLUME_NUMBER 0
#define SD_TIME_FILENAME "0:time.txt"
#define SD_DATALOG_FILENAME "0:data.csv"
#define SD_READING_IN_PROGRESS_FILENAME "0:a"
#define SD_CONFIG_FILENAME "0:lgr.cfg"

#define HEATER_TIMED_SLEEP_SECONDS 60

#define SDI_PIN PIN_PA09
#define SDI_PIN_PORT						PORTA
#define SDI_PIN_PINMASK						1<<9

#define DEND_ANALOG_PIN_1					ADC_POSITIVE_INPUT_PIN0		//PA03
#define DEND_ANALOG_PIN_2					ADC_POSITIVE_INPUT_PIN1		//PIN_PA03
#define DENDROMETER_TRAVEL_DISTANCE_MICROMETERS 12700.0	//12.7 millimeters

#define MAX31856_THERMOCOUPLE_TYPE_USED		MAX31856_THERMOCOUPLE_TYPE_T

#define DS1302_DATA_PIN						PIN_PA12
#define DS1302_CLOCK_PIN					PIN_PA13
#define DS1302_ENABLE_PIN					PIN_PA14

#define MAX31856_SPI_SERCOM_MODULE			SERCOM1
#define MAX31856_SPI_PINMUX_SETTING			SPI_SIGNAL_MUX_SETTING_E
#define MAX31856_SPI_PAD0					PINMUX_PA16C_SERCOM1_PAD0
#define MAX31856_SPI_PAD1					PINMUX_PA17C_SERCOM1_PAD1
#define MAX31856_SPI_PAD2					PINMUX_PA18C_SERCOM1_PAD2
#define MAX31856_SPI_PAD3					PINMUX_PA19C_SERCOM1_PAD3
#define MAX31856_CS_PIN						PIN_PA17

#define SD_SERCOM_MODULE					SERCOM0
#define SD_PINMUX_SETTING					SPI_SIGNAL_MUX_SETTING_E
#define SD_SPI_PIN0							PINMUX_PA04D_SERCOM0_PAD0
#define SD_SPI_PIN1							PINMUX_PA05D_SERCOM0_PAD1
#define SD_SPI_PIN2							PINMUX_PA06D_SERCOM0_PAD2
#define SD_SPI_PIN3							PINMUX_PA07D_SERCOM0_PAD3
#define SD_CS_PIN							PIN_PA05
#define SD_PIN_DETECT						PIN_PB05
#define SD_PIN_DETECT_LEVEL					0

//Mosfet PINS
#define MOSFET_PORT							PORTA
#define SDI_DHT22_POWER_MOSFET_PINMASK		1 << 0	//PIN_PA00
#define HEATER_MOSFET_PINAMSK				1 << 1	//PIN_PA01
#define DENDRO_TC_AMP_MOSFET_PINMASK		1 << 10	//PIN_PA10
#define SD_CARD_MOSFET_PINMASK				1 << 11	//PIN_PA11
#define ALL_MOSFET_PINMASK					SDI_DHT22_POWER_MOSFET_PINMASK | HEATER_MOSFET_PINAMSK | DENDRO_TC_AMP_MOSFET_PINMASK | SD_CARD_MOSFET_PINMASK

//TC Mux select
#define TC_MUX_SELECT_PORT					PORTA
#define TC_MUX_SELECT_A_PINMASK				1 << 20 //PIN_PA20
#define TC_MUX_SELECT_B_PINMASK				1 << 21 //PIN_PA21
#define TC_MUX_SELECT_ALL_PINMASK			TC_MUX_SELECT_A_PINMASK | TC_MUX_SELECT_B_PINMASK

#endif // CONF_BOARD_H
