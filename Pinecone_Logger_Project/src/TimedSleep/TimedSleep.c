/*
* TimedSleep.c
*
* Created: 10/19/2016 3:49:42 PM
*  Author: tim.anderson
*/

#include <asf.h>
#include "TimedSleep/TimedSleep.h"
#include <tc.h>
#include <tc_interrupt.h>
#include <sleepmgr.h>
#include <power.h>

#define TIMED_SLEEP_TC_HARDWARE TC2
static void SleepEndFunction(struct tc_module *const module);

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
	tcConfig.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	tcConfig.oneshot = true;
	tcConfig.run_in_standby = true;
	tc_init(tc_instance, TIMED_SLEEP_TC_HARDWARE, &tcConfig);
	tc_register_callback(tc_instance, SleepEndFunction, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(tc_instance, TC_CALLBACK_CC_CHANNEL0);
}

/*sleepEndFunction
Function that will be run as a callback when the sleep TimerCounter finishes.
Implicitly, by running this as a callback, the device will leave whatever low power mode it was in.
Function will disable the TimerCounter.*/
static void SleepEndFunction(struct tc_module *const module){
	tc_disable(module);
}

/*TimedSleep_seconds
sets a timer-counter, and then enters standby sleep mode.
The timer-counter should wake the device from sleep mode upon completion.
NOTE: Generic Clock Generator 1 should be enabled, allowed to run in standby,
use the internal 32KHz oscillator, and have an internal prescaler of 512.
Internal 32KHz oscillator should also be enabled, and able to run in standby
*/
void TimedSleepSeconds(struct tc_module *tc_instance, const uint32_t seconds){
	tc_set_count_value(tc_instance, 0);
	tc_set_compare_value(tc_instance, TC_COMPARE_CAPTURE_CHANNEL_0, seconds);
	tc_enable(tc_instance);
	//enable and go to sleep mode!
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
}

