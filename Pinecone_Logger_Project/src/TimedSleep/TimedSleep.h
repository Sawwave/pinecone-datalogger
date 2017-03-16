/*
* TimedSleep.h
*
* Created: 10/19/2016 3:48:58 PM
*  Author: tim.anderson
*/

#ifndef TIMED_SLEEP_H_
#define TIMED_SLEEP_H_

#define TIMED_SLEEP_1_HOUR 3600

/*initSleepTimerCounter
Sets up TimerCounters 4 and 5 to work together as a 32-bit timer.
Timer will be set to only run once once enabled, and run through standby to wake device up when needed.
Upon finishing, the TimerCounter will be disabled as device leaves sleep mode.
*/
void InitSleepTimerCounter(struct tc_module *tc_instance);

/*TimedSleep_seconds
sets a timer-counter, and then enters standby sleep mode.
The timer-counter should wake the device from sleep mode upon completion.
NOTE: Generic Clock Generator 1 should be enabled, allowed to run in standby,
use the internal 32KHz oscillator, and have an internal prescaler of 512.
Internal 32KHz oscillator should also be enabled, and able to run in standby
*/
void TimedSleepSeconds(struct tc_module *tc_instance, const uint32_t seconds);

void ExternalInterruptSleep(void);

#endif /* TIMEDSLEEP_H_ */