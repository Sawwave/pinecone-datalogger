/*
 * TimedSleep.h
 *
 * Created: 10/19/2016 3:48:58 PM
 *  Author: tim.anderson
 */ 

#ifndef TIMED_SLEEP_H_
#define TIMED_SLEEP_H_

#define TIMED_SLEEP_1_HOUR 3600

/*timedSleep_seconds
sets a timer-counter, and then enters standby sleep mode.
The timer-counter should wake the device from sleep mode upon completion.
NOTE: Generic Clock Generator 1 should be enabled, allowed to run in standby,
use the internal 32KHz oscillator, and have an internal prescaler of 512.
Internal 32KHz oscillator should also be enabled, and able to run in standby
*/
void timedSleep_seconds(struct tc_module *tc_instance, const uint32_t seconds);
void initSleepTimerCounter(struct tc_module *tc_instance);

#endif /* TIMEDSLEEP_H_ */