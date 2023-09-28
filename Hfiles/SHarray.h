/*
 * SHarray.h
 *
 *  Created on: Jul 1, 2020
 *      Author: OS1
 */

#ifndef SHARRAY_H_
#define SHARRAY_H_

#include "thread.h"

class SHarray {
public:

	SHarray();
	~SHarray();

	void put(SignalHandler handler);
	void execute();
	void swap(SignalHandler hand1, SignalHandler hand2);

private:

	SignalHandler* array;
	int size;
	SignalID firstFree;

};

#endif /* SHARRAY_H_ */
