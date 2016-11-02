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
#define ADC_MAX_VALUE 0x1000

void ConfigureDendroADC(struct adc_module *adcModule, const uint32_t dendAnalogPin)
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

/*ReadDendro
uses the adc module defined to read a specific analog pin, and uses the value and dend travel distance to compute dendrometer value.
use adc_enable and disable before and after reading form the dendros.*/
double ReadDendro(struct adc_module *adcModule)
{
	adc_enable(adcModule);
	uint16_t adcReadingValue;
	adc_start_conversion(adcModule);
	//read until the result is finished
	while(adc_read(adcModule, &adcReadingValue) == STATUS_BUSY)
	{	;	}
	adc_disable(adcModule);
	double ratioTraveled = ((double)adcReadingValue / (double)ADC_MAX_VALUE);
	return ratioTraveled * DENDROMETER_TRAVEL_DISTANCE_MICROMETERS;
}