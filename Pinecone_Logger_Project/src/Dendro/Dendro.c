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

/*ConfigureDendroADC
Configure the system ADC to read the dendrometer potentiometers.
Expected dendro values are 0v to 3.3v. However, there is no 3.3v reference for the adc.
Instead, we use the 1/2*Vcc reference, and use a 1/2 gain to divide the input between 0v and reference.*/
void ConfigureDendroADC(struct adc_module *adcModule)
{
	struct adc_config adcConfig;
	adc_get_config_defaults(&adcConfig);
	adcConfig.reference =		ADC_REFERENCE_INTVCC1;
	adcConfig.sample_length =	ADC_CTRLB_RESSEL_16BIT;
	adcConfig.positive_input =	ADC_POSITIVE_INPUT_PIN6;
	adcConfig.sample_length =	ADC_SAMPLE_LENGTH;
	adcConfig.gain_factor = ADC_GAIN_FACTOR_DIV2;
	
	adc_init(adcModule, ADC, &adcConfig);
}

/*ReadDendro
uses the adc module defined to read a specific analog pin, and uses the value and dend travel distance to compute dendrometer value.
use adc_enable and disable before and after reading form the dendros.*/
double ReadDendro(struct adc_module *adcModule, const enum adc_positive_input dendAnalogPin)
{
	uint16_t adcReadingValue;
	adc_set_positive_input(&adcModule, dendAnalogPin);
	adc_start_conversion(adcModule);
	//read until the result is finished
	while(adc_read(adcModule, &adcReadingValue) == STATUS_BUSY)
	{	;	}
	double ratioTraveled = ((double)adcReadingValue / (double)ADC_MAX_VALUE);
	return ratioTraveled * DENDROMETER_TRAVEL_DISTANCE_MICROMETERS;
}