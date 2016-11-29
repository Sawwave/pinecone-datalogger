/*
 * DHT22.h
 *
 * Created: 10/18/2016 5:47:02 PM
 *  Author: tim.anderson
 */ 

#ifndef DHT22_H_
#define DHT22_H_


#include <stdint.h>

enum Dht22Status{ DHT_STATUS_OKAY, DHT_STATUS_TIMEOUT, DHT_STATUS_CHECKSUM_ERROR };

/*GetDht22Reading
	Sends and receives confirmation signals with the DHT-22 sensor
	Function will return temp and RH in the variables passed in the arg list.
	dhtPin should be the GPIO pin the DHT data is connected to.
	
	NOTE: make sure the DHT22 has been powered for at LEAST 2 second before using this function.
	If this is not done, the sensor will have undefined behavior. */
enum Dht22Status GetDht22Reading(float *temp, float *relativeHumidity, const uint32_t dhtPinmask);


#endif /* DHT22_H_ */