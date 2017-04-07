/*
* TimedSleep.c
*
* Created: 10/19/2016 3:49:42 PM
*  Author: tim.anderson
*/

#include <asf.h>
#include "TimedSleep/TimedSleep.h"
#include "LedCodes/LedCodes.h"
#include <tc.h>
#include <tc_interrupt.h>
#include <sleepmgr.h>
#include <power.h>
#include <extint.h>


static void TimerCounterSleepEndFunc(struct tc_module *const module);

static void ExtintCallbackFunc(void);


/*initSleepTimerCounter
Sets up TimerCounters 4 and 5 to work together as a 32-bit timer.
Timer will be set to only run once once enabled, and run through standby to wake device up when needed.
Upon finishing, the TimerCounter will be disabled as device leaves sleep mode.
*/
void InitSleepTimerCounter(struct tc_module *tc_instance){
	struct tc_config tcConfig;
	tc_get_config_defaults(&tcConfig);
	tcConfig.counter_size = TC_COUNTER_SIZE_32BIT;
	tcConfig.clock_source = GCLK_GENERATOR_1;
	tcConfig.clock_prescaler = TC_CLOCK_PRESCALER_DIV4;
	tcConfig.oneshot = true;
	tcConfig.run_in_standby = true;
	tc_init(tc_instance, TIMED_SLEEP_TC_HARDWARE, &tcConfig);
	tc_register_callback(tc_instance, TimerCounterSleepEndFunc, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

/*sleepEndFunction
Function that will be run as a callback when the sleep TimerCounter finishes.
Implicitly, by running this as a callback, the device will leave whatever low power mode it was in.
Function will disable the TimerCounter.*/
static void TimerCounterSleepEndFunc(struct tc_module *const module){
	tc_disable(module);
}


static void ExtintCallbackFunc(void){
	extint_chan_disable_callback(DS3231_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
};

/*TimedSleep_seconds
sets a timer-counter, and then enters standby sleep mode.
The timer-counter should wake the device from sleep mode upon completion.
NOTE: Generic Clock Generator 1 should be enabled, allowed to run in standby,
use the internal 32KHz oscillator, and have an internal prescaler of 512.
Internal 32KHz oscillator should also be enabled, and able to run in standby

This function should also be used to sleep when wake on external interrupt is expected.
Set the time to be greater than extint wakeup timer, so that internal timer is a backup.
*/
void TimedSleepSeconds(struct tc_module *tc_instance, const uint32_t seconds){
	tc_set_count_value(tc_instance, 0);
	tc_set_compare_value(tc_instance, TC_COMPARE_CAPTURE_CHANNEL_0, seconds * 250L);
	tc_enable(tc_instance);
	//enable and go to sleep mode!
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
}


void ExternalInterruptInit(){
	struct extint_chan_conf extintConfig;
	extint_chan_get_config_defaults(&extintConfig);
	extintConfig.gpio_pin_pull = EXTINT_PULL_NONE;
	extintConfig.gpio_pin = PIN_PA28A_EIC_EXTINT8;
	extintConfig.gpio_pin_mux = MUX_PA28A_EIC_EXTINT8;
	extintConfig.detection_criteria = EXTINT_DETECT_FALLING;
	extint_chan_set_config(DS3231_EIC_LINE, &extintConfig);
	
	extint_register_callback(ExtintCallbackFunc, DS3231_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}

/*Set device to sleep, using DS3231 extint as a wakeup alarm*/
void ExternalInterruptSleep(void){
	system_interrupt_enable_global();
	extint_chan_enable_callback(DS3231_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
}

/*enable DS3231 extint wakeup alarm, but don't go to sleep.*/
void EnableExtintWakeup(void){
	system_interrupt_enable_global();
	extint_chan_enable_callback(DS3231_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}