/*
 * main.cpp
 *
 *  Created on: Apr 27, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include <dos.h>
#include "schedule.h"
#include "timer.h"
#include "thread.h"
#include "pcb.h"
#include "PCBarray.h"
#include "semaphor.h"
#include "KSarray.h"
#include "user.h"

typedef void interrupt (*pInterrupt)(...);

pInterrupt oldTimer;

unsigned oldTimerOFF, oldTimerSEG;

void inic() {
#ifndef BCC_BLOCK_IGNORE
	asm cli
	oldTimer = getvect(0x08);
	setvect(0x08, timer);
	setvect(0x60, oldTimer);
	asm sti
#endif
}

void restore() {
#ifndef BCC_BLOCK_IGNORE
	asm cli
	setvect(0x08, oldTimer);
	asm sti
#endif
}

class IdleThread: public Thread {
public:
	IdleThread(StackSize stackSize, Time timeSlice): Thread(stackSize, timeSlice) {}
	~IdleThread() {}
protected:
	void run() {
		while(numOfActiveThreads > 0);
	}
};

int result = 0;
int g_argc = 0;
char** g_argv = 0;

int userMain(int argc, char* argv[]);

class ThreadMain: public Thread {
public:
	ThreadMain(StackSize stackSize, Time timeSlice): Thread(stackSize, timeSlice) {}
	~ThreadMain() {}
protected:
	void run() {
		result = userMain(g_argc, g_argv);
		cout << "userMain returns " << result << endl;
	}
};

int main(int argc, char* argv[]) {

	g_argc = argc;
	g_argv = new char*[argc + 1];
	for (int i = 0; i <= argc; i++)
		g_argv[i] = argv[i];

	mainPCB = new PCB(1024, 0, 0);
	running = mainPCB;

	Thread *threadMain = new ThreadMain(2048, 10);
	Thread *idleThread = new IdleThread(256, 0);
	idlePCB = pcbArray->getPCById(idleThread->getId());

	inic();

	threadMain->start();
	dispatch();

	restore();

	delete idleThread;
	delete mainPCB;
	delete threadMain;
	delete g_argv;

	cout << "Happy End" << endl;
	cout << "main returns " << result << endl;

	return result;
}

