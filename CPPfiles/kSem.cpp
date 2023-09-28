/*
 * kSem.cpp
 *
 *  Created on: May 6, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include "kSem.h"
#include "semaphor.h"
#include "KSarray.h"
#include "timer.h"
#include "schedule.h"

volatile int semUpdateCnt = 0;

KernelSem::KernelSem(int init) {
	value = init;
	blockedHead = 0;
	blockedTail = 0;
	updateCnt = 0;
	id = 0;

	if (!kSemArray)
		kSemArray = new KernelSemArray();
	kSemArray->put(this);
}

KernelSem::~KernelSem() {}

int KernelSem::val() const {
	return value;
}

int KernelSem::wait(Time maxTimeToWait) {
	lockFlag = 0;

	int result;
	if (--value < 0) result = block(maxTimeToWait);

	lockFlag = 1;
	if (context_switch)
		dispatch();

	return result;
}

int KernelSem::block(Time maxTimeToWait) {
	running->status = BLOCKED;

	if (!blockedTail)
		blockedHead = running;
	else
		blockedTail->nextSem = running;
	blockedTail = running;

	if (maxTimeToWait) {
		running->timeToWait = maxTimeToWait;
		updateCnt++;
		semUpdateCnt++;
	}

	lockFlag = 1;
	dispatch();		//returns here after deblocking

	if (running->timerFlag) {	//checks if deblocking happend due to timeout
		running->timerFlag = 0;
		return 0;
	}
	return 1;
}

int KernelSem::signal(int n) {
	if (n < 0)
		return n;

	lockFlag = 0;

	int k = 0;
	do {
		if (value++ < 0) {
			deblock();
			k++;
		}
	} while (--n > 0);
	lockFlag = 1;
	if (context_switch)
		dispatch();
	return k;
}

void KernelSem::deblock() {
	PCB* temp = blockedHead;
	blockedHead = temp->nextSem;
	if (!blockedHead)
		blockedTail = 0;
	temp->nextSem = 0;

	if (temp->timeToWait > 0) {
		temp->timeToWait = 0;
		updateCnt--;
		semUpdateCnt--;
	}

	temp->status = READY;
	Scheduler::put(temp);
}

void KernelSem::update() {		//called from timer() interrupt routine, it's protected
	PCB *prev = 0;
	PCB *curr = blockedHead;

	while (curr && updateCnt)
		if (curr->timeToWait ? !(--curr->timeToWait) : 0) {
			updateCnt--;
			semUpdateCnt--;

			PCB *old = curr;
			curr = curr->nextSem;
			if (!prev) {
				blockedHead = curr;
				if (!blockedHead)
					blockedTail = 0;
			}
			else
				prev->nextSem = curr;
			old->nextSem = 0;

			old->status = READY;
			old->timerFlag = 1;
			Scheduler::put(old);
			if (running == idlePCB && !context_switch)
				context_switch = 1;
		}
		else {
			prev = curr;
			curr = curr->nextSem;
		}
}
