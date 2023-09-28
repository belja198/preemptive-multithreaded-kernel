/*
 * KSarray.h
 *
 *  Created on: May 6, 2020
 *      Author: OS1
 */

#ifndef KSARRAY_H_
#define KSARRAY_H_

#include "kSem.h"
#include "semaphor.h"

class KernelSemArray {
public:

	KernelSemArray();
	~KernelSemArray();

	void put(KernelSem* kSem);
	void remove(KernelSem* kSem);
	KernelSem* getKSemById(ID id);

	int size;
	KernelSem** array;
	ID firstFree;

};

extern KernelSemArray *kSemArray;

#endif /* KSARRAY_H_ */
