/*
 * SHarray.cpp
 *
 *  Created on: Jul 1, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include "thread.h"
#include "timer.h"
#include "pcb.h"
#include "SHarray.h"

SHarray::SHarray() {
	array = new SignalHandler[size = 5];
	firstFree = 0;
	for (int i = 0; i < size; i++)
		array[i] = 0;
}

SHarray::~SHarray() {
	delete[] array;
}

void SHarray::put(SignalHandler hand) {
	if (firstFree == size) {		//if the array is full reallocate it and copy(shallow) the content
		unsigned newSize = size + 5;
		SignalHandler* newArray = new SignalHandler[newSize];

		for (int i = 0; i < size; i++)
			newArray[i] = array[i];
		for (int j = size; j < newSize; j++)
			newArray[j] = 0;

		delete[] array;
		array = newArray;
		size = newSize;
	}
	array[firstFree++] = hand;
}

void SHarray::swap(SignalHandler hand1, SignalHandler hand2) {
	int i1 = -1, i2 = -1;

	for (int i = 0; i < size && !array[i]; i++) {
		if (array[i] == hand1)
			i1 = i;
		if (array[i] == hand2)
			i2 = i;
	}

	if (i1 < 0 || i2 < 0)
		return;

	SignalHandler temp = array[i1];
	array[i1] = array[i2];
	array[i2] = temp;
}

void SHarray::execute() {
	for (int i = 0; i < size; i++) {
		if (!array[i])
			break;
		(*array[i])();
	}
}
