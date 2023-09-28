/*
 * ivtEntry.cpp
 *
 *  Created on: Jun 26, 2020
 *      Author: OS1
 */

#include <dos.h>
#include "ivtEntry.h"

IVTEntry *ivtArray[256];

IVTEntry::IVTEntry(int mIvtNo, pInterrupt newRoutine) {
	ivtNo = mIvtNo;
	oldRoutine = 0;
	myKernEvent = 0;

#ifndef BCC_BLOCK_IGNORE
	asm cli
	oldRoutine = getvect(ivtNo);
	setvect(ivtNo, newRoutine);
	asm sti
#endif

	ivtArray[ivtNo] = this;
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	asm cli
	setvect(ivtNo, oldRoutine);
	asm sti
#endif

	ivtArray[ivtNo] = 0;
}

void IVTEntry::signal() {
	if (!myKernEvent)
		return;

	myKernEvent->signal();
}

void IVTEntry::callOldRout() {
	(*oldRoutine)();
}
