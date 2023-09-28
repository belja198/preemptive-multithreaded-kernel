/*
 * thread
 *
 *  Created on: Apr 19, 2020
 *      Author: OS1
 */

#ifndef THREAD_H_
#define THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice = 2;  // default = 2*55ms
typedef int ID;

typedef void (*SignalHandler)();
typedef unsigned SignalID;

class PCB; // Kernel's implementation of a user's thread

class Thread {
public:

	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread* getThreadById(ID id);

	void signal(SignalID signal);

	void registerHandler(SignalID signal, SignalHandler handler);
	void unregisterAllHandlers(SignalID signal);
	void swap(SignalID signal, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalID signal);
	static void blockSignalGlobally(SignalID signal);
	void unblockSignal(SignalID signal);
	static void unblockSignalGlobally(SignalID signal);

protected:
	friend class PCB;
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice =
			defaultTimeSlice);
	virtual void run() {}

private:
	PCB* myPCB;
};

void dispatch ();

#endif /* THREAD_H_ */
