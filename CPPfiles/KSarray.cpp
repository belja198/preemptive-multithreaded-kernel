/*
 * KSarray.cpp
 *
 *  Created on: May 6, 2020
 *      Author: OS1
 */
#include "KSarray.h"
#include "thread.h"
#include "timer.h"

KernelSemArray *kSemArray = 0;

KernelSemArray::KernelSemArray() {
	array = new KernelSem*[size = 10];
	firstFree = 0;
	for (int i = 0; i < size; i++)
		array[i] = 0;
}

KernelSemArray::~KernelSemArray() {
	delete[] array;
}

void KernelSemArray::put(KernelSem* kSem) {
	lockFlag = 0;	//lock
	ID id;
	if (firstFree == size) {		//if the array is full reallocate it and copy(shallow) the content
		unsigned newSize = size + 10;
		KernelSem** newArray = new KernelSem*[newSize];

		for (int i = 0; i < size; i++)
			newArray[i] = array[i];
		for (int j = size; j < newSize; j++)
			newArray[j] = 0;

		delete[] array;
		array = newArray;
		size = newSize;
	}
	array[id = firstFree++] = kSem;
	kSem->id = id;
	while (firstFree != size && array[firstFree])
		firstFree++;
	lockFlag = 1;		//unlock
	if (context_switch == 1)	//change the context if it were
		dispatch();				//supposed to happen during the critical section
}

void KernelSemArray::remove(KernelSem* kSem) {
	ID freeId = kSem->id;
	array[freeId] = 0;

	lockFlag = 0;	//lock

	if (freeId < firstFree)
		firstFree = freeId;

	lockFlag = 1;		//unlock
	if (context_switch == 1)	//change the context if it were
		dispatch();				//supposed to happen during the critical section
}



KernelSem* KernelSemArray::getKSemById(ID id) {
	return array[id];
}
