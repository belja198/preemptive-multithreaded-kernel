/*
 * thread.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: OS1
 */

#include "thread.h"
#include "PCBarray.h"
#include "pcb.h"
#include "timer.h"

Thread::Thread(StackSize stackSize, Time timeSlice) {
	myPCB = new PCB(stackSize, timeSlice, this);
}

Thread::~Thread() {
	delete myPCB;
}

void Thread::start(){
	myPCB->start();
}

ID Thread::getId() {
	return myPCB->id;
}

ID Thread::getRunningId() {
	return running->id;
}

Thread* Thread::getThreadById(ID id) {
	return pcbArray->getThreadById(id);
}

void Thread::waitToComplete() {
	myPCB->waitToComplete();
}

void Thread::signal(SignalID signal) {
	myPCB->signal(signal);
}

void Thread::registerHandler(SignalID signal, SignalHandler handler) {
	myPCB->registerHandler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalID signal) {
	myPCB->unregisterAllHandlers(signal);
}

void Thread::swap(SignalID signal, SignalHandler hand1, SignalHandler hand2) {
	myPCB->swap(signal, hand1, hand2);
}

void Thread::blockSignal(SignalID signal) {
	myPCB->blockSignal(signal);
}

void Thread::blockSignalGlobally(SignalID signal) {
	pcbArray->blockSignals(signal);
}

void Thread::unblockSignal(SignalID signal) {
	myPCB->unblockSignal(signal);
}

void Thread::unblockSignalGlobally(SignalID signal) {
	pcbArray->unblockSignals(signal);
}

void dispatch(){
#ifndef BCC_BLOCK_IGNORE
asm cli;
//#endif
	context_switch = 1;
	asm int 0x08 //timer();
//#ifndef BCC_BLOCK_IGNORE
asm sti;
#endif
}
