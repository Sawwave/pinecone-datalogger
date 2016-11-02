/*
* Dendro.c
*
* Created: 10/18/2016 5:31:07 PM
*  Author: tim.anderson
*/

#include <asf.h>
#include "Dendro/Dendro.h"
#include <adc.h>
#include <adc_feature.h>
#include <math.h>

#define ADC_SAMPLE_LENGTH 32


void ConfigureDendroADC(struct adc_module *adcModule, uint32_t dendAnalogPin)
{
	struct adc_config adcConfig;
	adc_get_config_defaults(&adcConfig);
	//use 3.3v as reference.
	adcConfig.reference =		ADC_REFERENCE_AREFA;
	adcConfig.sample_length =	ADC_CTRLB_RESSEL_16BIT;
	adcConfig.positive_input =	dendAnalogPin;
	adcConfig.sample_length =	ADC_SAMPLE_LENGTH;
	
	adc_init(adcModule, ADC, &adcConfig);
}

double ReadDendro(struct adc_module *adcModule)
{
	uint16_t adcReadingValue;
	//read until the result is finished
	while(adc_read(adcModule, &adcReadingValue) == STATUS_BUSY)
	{	;	}
	double ratioTraveled = ((double)adcReadingValue / (double)0x1000);
	double currentPosition = ratioTraveled * DENDROMETER_TRAVEL_DISTANCE_MICROMETERS;
	
	return currentPosition;
}