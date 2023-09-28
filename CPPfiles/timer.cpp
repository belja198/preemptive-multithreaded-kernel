/*
 * timer.cpp
 *
 *  Created on: Apr 26, 2020
 *      Author: OS1
 */
#include <iostream.h>
#include <dos.h>

#include "timer.h"
#include "pcb.h"
#include "schedule.h"
#include "kSem.h"
#include "KSarray.h"

unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int counter = 20;
volatile int context_switch = 0;

volatile unsigned int lockFlag = 1;

volatile int i = 0;

void interrupt timer(...) {	// prekidna rutina
											// poziv stare prekidne rutine koja se
#ifndef BCC_BLOCK_IGNORE					// nalazila na 08h, a sad je na 60h
	if(!context_switch) asm int 0x60;		// poziva se samo kada nije zahtevana promena
#endif										// konteksta – tako se da se stara
											// rutina poziva samo kada je stvarno doslo do prekida

	if (!context_switch && semUpdateCnt) {
		i = 0;
		while (semUpdateCnt > 0 && i < kSemArray->size) {
			KernelSem *kSem = kSemArray->array[i];
			if (kSem && kSem->updateCnt > 0)
				kSem->update();
			i++;
		}
	}

	if (!context_switch && counter >= 0)
		counter--;
	if (counter == 0 || context_switch) {
		if (lockFlag) {
			context_switch = 0;
#ifndef BCC_BLOCK_IGNORE
			asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
#endif
			running->sp = tsp;
			running->ss = tss;
			running->bp = tbp;

			lockFlag = 0;
	//		cout<<"Constext switch! Counter = "<<counter<<endl;
#ifndef BCC_BLOCK_IGNORE
			asm cli;
#endif
			lockFlag = 1;

			if (running->status != FINISHED && running->status != BLOCKED) Scheduler::put((PCB *) running);
			running = Scheduler::get();

			if (running == 0)
				if (numOfActiveThreads > 0) running = idlePCB;
				else running = mainPCB;

			tsp = running->sp;
			tss = running->ss;
			tbp = running->bp;

			counter = running->timeSlice;
#ifndef BCC_BLOCK_IGNORE
			asm {
				mov sp, tsp   // restore sp
				mov ss, tss
				mov bp, tbp
			}
#endif

		running->handleSignals();	//handle signals probably :/
		}
		else context_switch = 1;
	}
}

