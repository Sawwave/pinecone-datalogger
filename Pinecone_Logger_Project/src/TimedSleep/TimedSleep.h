/*
 * TimedSleep.h
 *
 * Created: 10/19/2016 3:48:58 PM
 *  Author: tim.anderson
 */ 

#ifndef TIMED_SLEEP_H_
#define TIMED_SLEEP_H_

#define TIMED_SLEEP_1_HOUR 3600

void timedSleep_seconds(struct tc_module *tc_instance, const uint32_t seconds);
void initSleepTimerCounter(struct tc_module *tc_instance);

#endif /* TIMEDSLEEP_H_ */