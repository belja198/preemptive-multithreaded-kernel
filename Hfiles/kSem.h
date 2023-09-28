/*
 * kSem.h
 *
 *  Created on: May 6, 2020
 *      Author: OS1
 */

#ifndef KSEM_H_
#define KSEM_H_

#include "semaphor.h"
#include "pcb.h"

class KernelSem {
public:

	KernelSem(int init/*, Semaphore *semaphore*/);
	~KernelSem();

	int wait(Time maxTimeToWait);
	int signal(int n = 0);

	int val() const;

	int block(Time maxTimeToWait);
	void deblock();

	void update();

	volatile int updateCnt;
	ID id;

private:
//	Semaphore *mySem;	//delete later probably
	int value;
	PCB *blockedHead, *blockedTail;
};

extern volatile int semUpdateCnt;

#endif /* KERNELSEM_H_ */

