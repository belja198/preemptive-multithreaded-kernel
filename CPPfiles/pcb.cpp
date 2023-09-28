/*
 * pcb.cpp
 *
 *  Created on: Apr 20, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include <dos.h>
#include "pcb.h"
#include "thread.h"
#include "timer.h"
#include "PCBarray.h"
#include "schedule.h"
#include "SHarray.h"

PCB *volatile running;
PCB *idlePCB;
PCB *mainPCB;
volatile int numOfActiveThreads = 0;

void signal0() {
	running->finalize();
}

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* thread) {

	myThread = thread;
	parent = running;

	next = 0;
	id = 0;

	nextSem = 0; timeToWait = 0; timerFlag = 0;	//for semaphores

	ss = 0; sp = 0; bp = 0;
	this->timeSlice = timeSlice; status = BLOCKED;

	for (int i = 0; i < 16; i++) {
		signals[i] = 0;
		isBlocked[i] = 0;
	}
	registerHandler(0, signal0);
	front = 0; rear = 0;

	stackSize /= sizeof(unsigned);
	stack = new unsigned[stackSize];

#ifndef BCC_BLOCK_IGNORE	//initializing context for a new thread
	stack[stackSize - 1] = 0x200;
	stack[stackSize - 2] = FP_SEG(wrapper);
	stack[stackSize - 3] = FP_OFF(wrapper);

	ss = FP_SEG(stack + stackSize - 12);
	sp = FP_OFF(stack + stackSize - 12);
	bp = sp;
#endif

	if (!pcbArray)
		pcbArray = new PCBarray();	//array for ID's of PCB's
	pcbArray->put(this);
}

PCB::~PCB() {
	pcbArray->remove(this);
	delete[] stack;
	ArrivalNode *curr = front;

	for (int i = 0; i < 16; i++)
		if (signals[i])
			delete signals[i];

	while (curr) {
		ArrivalNode *old = curr;
		curr = curr->next;
		old->next = 0;
		delete old;
	}
	front = 0; rear = 0;
}

void PCB::finalize() {
	lockFlag = 0;	//lock, nobody can interfere while the running thread is finishing

	numOfActiveThreads--;
	status = FINISHED;

	PCB *temp = next, *old = running;	//put everyone who was waiting for me in the Scheduler
	while (temp) {								// and set their status on READY
		temp->status = READY;
		Scheduler::put(temp);
		old->next = 0;
		old = temp;
		temp = temp->next;
	}

	if (signals[2])
		signals[2]->execute();		//*maybe add cli after*
	parent->signal(1);	//will set lockFlag to 1 and maybe call dispatch()
	lockFlag = 1;		//unlock
	dispatch();		//if called from wrapper() there is nowhere to return, if called from
					//signal[0] via handleSignals() via timer() there is no returning on purpose
}

void PCB::wrapper() {
	running->myThread->run();	//starting myThread
								//returns here after the thread finishes
	running->finalize();	//context switch inside, there is nowhere to return with '}'
}

void PCB::start() {
	if (status == FINISHED)
		return;

	lockFlag = 0;	//lock
	status = READY;
	numOfActiveThreads++;
	Scheduler::put(this);
	lockFlag = 1;
	if (context_switch == 1)	//change the context if it were
		dispatch();				//supposed to happen during the critical section
}

void PCB::waitToComplete() {
	if (status != FINISHED) {
		lockFlag = 0;	//lock
		running->status = BLOCKED;	//the waiting thread musn't go back in the Scheduler
		PCB *temp = this;		//link the running thread's PCB in a waiting list
		while(temp->next)	//of the thread's PCB which is being waited for
			temp = temp->next;
		temp->next = running;
		lockFlag = 1;		//unlock
		dispatch();		//can't return while waiting, so call dispatch anyway
	}
}

void PCB::signal(SignalID signal) {
	if (!signals[signal])
		return;

	lockFlag = 0;

	ArrivalNode *node = new ArrivalNode(signal);
	if (rear)
		rear->next = node;
	else
		front = node;
	rear = node;

	lockFlag = 1;
	if (context_switch == 1)
		dispatch();

}

void PCB::registerHandler(SignalID signal, SignalHandler handler) {
	lockFlag = 0;
	if (!signals[signal])
		signals[signal] = new SHarray();
	signals[signal]->put(handler);
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCB::unregisterAllHandlers(SignalID signal) {
	lockFlag = 0;
	delete signals[signal];
	signals[signal] = 0;
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCB::swap(SignalID signal, SignalHandler hand1, SignalHandler hand2) {
	lockFlag = 0;	//lock
	if (!signals[signal])
		return;
	signals[signal]->swap(hand1, hand2);
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCB::blockSignal(SignalID signal) {
	lockFlag = 0;
	isBlocked[signal] = 1;
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCB::unblockSignal(SignalID signal) {
	lockFlag = 0;
	isBlocked[signal] = 0;
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCB::handleSignals() {		//called from timer();
	lockFlag = 0;	//context switch is forbidden, lock
#ifndef BCC_BLOCK_IGNORE
	asm sti		//interrupts are allowed
#endif
	ArrivalNode *curr = front, *prev = 0;

	while (curr) {
		if (!isBlocked[curr->sign]) {
			ArrivalNode *old = curr;	//not blocked, execute handlers, remove from the queue
			curr = curr->next;
			if (!prev) {
				front = curr;
				if (!front)
					rear = 0;
			}
			else
				prev->next = curr;
			old->next = 0;

			signals[old->sign]->execute();
			delete old;
		}
		else {
			prev = curr;	//blocked,  just go to the next one
			curr = curr->next;
		}
	}
#ifndef BCC_BLOCK_IGNORE
	asm cli		//forbid interrupts again because you're returning to timer(); which is a interrupt routine
#endif
	lockFlag = 1;
}
