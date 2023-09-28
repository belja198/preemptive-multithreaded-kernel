/*
 * kernelEv.cpp
 *
 *  Created on: Jun 25, 2020
 *      Author: OS1
 */

#include "kernelEv.h"
#include "event.h"
#include "timer.h"
#include "schedule.h"
#include "ivtEntry.h"

KernelEv::KernelEv(int ivtNo) {
	owner = running;
	blocked = 0;
	val = 0;

	ivtArray[ivtNo]->myKernEvent = this;
}

KernelEv::~KernelEv() {

}

void KernelEv::wait() {
	if (owner != running)
		return;

	if (!val) {		//block
		lockFlag = 0;

		running->status = BLOCKED;
		blocked = running;

		lockFlag = 1;
		dispatch();
	}
	else
		val = 0;
}

void KernelEv::signal() {
	if (!blocked)
		val = 1;
	else {
		lockFlag = 0;	//if called from interrupt routine locking is not needed,
						//if signal() is called explicitly from Event type object locking is necessery
		blocked->status = READY;
		Scheduler::put(blocked);
		blocked = 0;

		lockFlag = 1;
		if (context_switch == 1)
			dispatch();		//will not happen if signal() is called from the interrupt routine
	}
}
