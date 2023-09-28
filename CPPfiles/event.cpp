/*
 * event.cpp
 *
 *  Created on: Jun 25, 2020
 *      Author: OS1
 */

#include "event.h"
#include "kernelEv.h"

Event::Event(IVTNo ivtNo) {
	myImpl = new KernelEv((int)ivtNo);
}

Event::~Event() {
	delete myImpl;
}

void Event::wait() {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}
