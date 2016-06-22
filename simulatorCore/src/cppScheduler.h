/*
 * CppScheduler.h
 *
 *  Created on: 12 janvier 2014
 *      Author: Benoît
 */

#ifndef CPPSCHEDULER_H_
#define CPPSCHEDULER_H_

#include <thread>
#include <functional>

#include "sema.h"
#include "scheduler.h"
#include "network.h"
#include "trace.h"

class CPPScheduler : public BaseSimulator::Scheduler {
protected:
	std::thread *schedulerThread;
	int schedulerMode;

	CPPScheduler();
	virtual ~CPPScheduler();
	void* startPaused(/*void *param */);

public:
	Semaphore *sem_schedulerStart;

	static void createScheduler();
	static void deleteScheduler();
	static CPPScheduler* getScheduler() {
		assert(scheduler != NULL);
		return((CPPScheduler*)scheduler);
	}

	void printInfo() {
		OUTPUT << "I'm a CPPScheduler" << endl;
	}

	void start(int mode);

	void waitForSchedulerEnd() {
		schedulerThread->join();
	}

	inline int getMode() { return schedulerMode; }
};

inline void createScheduler() {
	CPPScheduler::createScheduler();
}

inline void deleteScheduler() {
	CPPScheduler::deleteScheduler();
}

inline CPPScheduler* getScheduler() { return(CPPScheduler::getScheduler()); }

#endif /* CPPSCHEDULER_H_ */
