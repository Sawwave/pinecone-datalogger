/*
 * Dendro.h
 *
 * Created: 10/18/2016 5:30:58 PM
 *  Author: tim.anderson
 */ 

#ifndef DENDRO_H_
#define DENDRO_H_

#include <stdint.h>

/*ConfigureDendroADC
Configure the system ADC to read the dendrometer potentiometers.
Expected dendro values are 0v to 3.3v. However, there is no 3.3v reference for the adc.
Instead, we use the 1/2*Vcc reference, and use a 1/2 gain to divide the input between 0v and reference.*/
void ConfigureDendroADC(struct adc_module *adcModule);

/*ReadDendro
uses the adc module defined to read a specific analog pin, and uses the value and dend travel distance to compute dendrometer value.
use adc_enable and disable before and after reading form the dendros.*/
double ReadDendro(struct adc_module *const adcModule, const enum adc_positive_input dendAnalogPin);

#endif /* DENDRO_H_ */