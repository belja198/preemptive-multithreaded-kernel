/*
 * PCBarray.h
 *
 *  Created on: Apr 21, 2020
 *      Author: OS1
 */

#ifndef PCBARRAY_H_
#define PCBARRAY_H_

#include "pcb.h"
#include "thread.h"

class PCBarray {
public:

	PCBarray();
	~PCBarray();

	void put(PCB* pcb);
	void remove(PCB* pcb);

	Thread* getThreadById(ID id);
	PCB* getPCById(ID id);

	void blockSignals(SignalID signal);
	void unblockSignals(SignalID signal);

private:

	PCB** array;
	int size;
	ID firstFree;

};

extern PCBarray *pcbArray;

#endif /* PCBARRAY_H_ */
