/*
 * ivtEntry.h
 *
 *  Created on: Jun 25, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef void interrupt (*pInterrupt)(...);

#include "event.h"

class KernelEv;

#include "kernelEv.h"

class IVTEntry {
public:
	IVTEntry(int mIvtNo, pInterrupt newRoutine);
	~IVTEntry();

	void signal();
	void callOldRout();

	KernelEv *myKernEvent;

private:
	int ivtNo;
	pInterrupt oldRoutine;
};

extern IVTEntry *ivtArray[256];

#endif /* IVTENTRY_H_ */
