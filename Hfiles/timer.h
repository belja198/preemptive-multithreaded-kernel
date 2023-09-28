/*
 * timer.h
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */

#ifndef TIMER_H_
#define TIMER_H_

extern volatile int context_switch;

extern volatile unsigned int lockFlag;

void interrupt timer(...);

#endif /* TIMER_H_ */
