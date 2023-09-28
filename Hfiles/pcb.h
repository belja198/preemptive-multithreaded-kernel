/*
 * pcb.h
 *
 *  Created on: Apr 19, 2020
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "SHarray.h"

enum Status {
	READY, BLOCKED, FINISHED
};

struct ArrivalNode {
	ArrivalNode(SignalID sig): sign(sig), next(0) {}
	SignalID sign;
	ArrivalNode* next;
};

class PCB {
public:

	PCB(StackSize stackSize, Time timeSlice, Thread* thread = 0);
	~PCB();

	void start();
	void waitToComplete();

	static void wrapper();

	Thread* myThread;

	ID id;

	unsigned *stack;
	unsigned sp;
	unsigned ss;
	unsigned bp;

	Time timeSlice;
	Time timeToWait;
	int timerFlag;
	Status status;

	PCB *next;		//for linking in waitToComplete();
	PCB *nextSem;	//for linking in a semaphore
	PCB *parent;

	void signal(SignalID signal);

	void registerHandler(SignalID signal, SignalHandler handler);
	void unregisterAllHandlers(SignalID signal);
	void swap(SignalID signal, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalID signal);
	void unblockSignal(SignalID signal);

	void handleSignals();

	void finalize();

	SHarray *signals[16];
	ArrivalNode *front, *rear;	//queue for arrival of the signals
	int isBlocked[16];	//indicates if a signal is blocked

};

extern PCB *volatile running;
extern PCB *idlePCB;
extern PCB *mainPCB;
extern volatile int numOfActiveThreads;

#endif /* PCB_H_ */
