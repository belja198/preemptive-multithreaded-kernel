/*
 * PCBarray.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include "PCBarray.h"
#include "thread.h"
#include "timer.h"

PCBarray *pcbArray = 0;

PCBarray::PCBarray() {
	array = new PCB*[size = 10];
	firstFree = 0;
	for (int i = 0; i < size; i++)
		array[i] = 0;
}

PCBarray::~PCBarray() {
	delete[] array;
}

void PCBarray::put(PCB* pcb) {
	lockFlag = 0;	//lock
	ID id;
	if (firstFree == size) {		//if the array is full reallocate it and copy(shallow) the content
		unsigned newSize = size + 10;
		PCB** newArray = new PCB*[newSize];

		for (int i = 0; i < size; i++)
			newArray[i] = array[i];
		for (int j = size; j < newSize; j++)
			newArray[j] = 0;

		delete[] array;
		array = newArray;
		size = newSize;
	}
	array[id = firstFree++] = pcb;
	pcb->id = id;
	while (firstFree != size && array[firstFree])
		firstFree++;
	lockFlag = 1;		//unlock
	if (context_switch == 1)	//change the context if it were
		dispatch();				//supposed to happen during the critical section
}

void PCBarray::remove(PCB* pcb) {
	ID freeId = pcb->id;
	array[freeId] = 0;

	lockFlag = 0;	//lock

	if (freeId < firstFree)
		firstFree = freeId;

	lockFlag = 1;		//unlock
	if (context_switch == 1)	//change the context if it were
		dispatch();				//supposed to happen during the critical section
}

Thread* PCBarray::getThreadById(ID id) {
	return array[id]->myThread;
}

PCB* PCBarray::getPCById(ID id) {
	return array[id];
}

void PCBarray::blockSignals(SignalID signal) {
	lockFlag = 0;
	for (int i = 0; i < size; i++)
		if (array[i])
			array[i]->isBlocked[signal] = 1;
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}

void PCBarray::unblockSignals(SignalID signal) {
	lockFlag = 0;
	for (int i = 0; i < size; i++)
		if (array[i])
			array[i]->isBlocked[signal] = 0;
	lockFlag = 1;
	if (context_switch == 1)
		dispatch();
}
