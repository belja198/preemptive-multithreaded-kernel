/*
 * kernelEv.h
 *
 *  Created on: Jun 25, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "event.h"
#include "pcb.h"

class KernelEv {
public:
	KernelEv(int ivtNo);
	~KernelEv();

	void wait();
	void signal();

private:
	int val;
	PCB *owner, *blocked;
};

#endif /* KERNELEV_H_ */
