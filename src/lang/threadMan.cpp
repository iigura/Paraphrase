#include <vector>
#include <chrono>

#include "paraphrase.h"
#include "thread.h"
#include "typedValue.h"
#include "context.h"
#include "util.h"

static std::vector<TypedValue> gThreadPool=std::vector<TypedValue>();
static Mutex gThreadPoolMutex;

static volatile int gIntervalSec=60;	// 60sec

static volatile bool gDoThreadCollector;
static std::shared_ptr<Thread> gThreadCollector;
static Cond gCV;
static Mutex gMutexCV;

static void *threadCollector(void * /* dummy */);

PP_API void StartThreadCollector() {
	gDoThreadCollector=true;
	gThreadCollector=NewSharedThread(threadCollector,nullptr);
}

PP_API void StopThreadCollector() {
	gDoThreadCollector=false;
	NotifyOne(gCV);
	Join(gThreadCollector.get());
}

PP_API void AddToThreadPool(TypedValue& inTvContext) {
	if(inTvContext.dataType!=DataType::Context) {
		ExitOnSystemError("AddToThreadPool");	
	}
	Lock(gThreadPoolMutex);
		gThreadPool.push_back(inTvContext);
	Unlock(gThreadPoolMutex);
}

PP_API int SetThreadCollectorInterval(int inIntervalSec) {
	int preInterval=gIntervalSec;
	gIntervalSec=inIntervalSec;
	return preInterval;
}

PP_API int GetThreadCollectorInterval() {
	return gIntervalSec;
}

static void *threadCollector(void *) {
	LockForWait(gMutexCV);
	while( gDoThreadCollector ) {
		CondTimeWait(gCV,gMutexCV,gIntervalSec);
		Lock(gThreadPoolMutex);
			size_t n=gThreadPool.size();
		Unlock(gThreadPoolMutex);
		for(size_t i=0; i<n; i++) {
			Lock(gThreadPoolMutex);
				TypedValue& tvContext=gThreadPool[i];
			Unlock(gThreadPoolMutex);
			if(tvContext.dataType!=DataType::Context) {
				ExitOnSystemError("threadCollector:type-check");	
			}
			if( tvContext.contextPtr->running ) {
				goto cont;
			} else {	// the case of contextPtr->runnging == false.
				if( TryLock(tvContext.contextPtr->mutexForJoined) ) {
					if(tvContext.contextPtr->joined==false) {
						Join(tvContext.contextPtr->thread.get());
						tvContext.contextPtr->joined=true;
					}
					Unlock(tvContext.contextPtr->mutexForJoined);
				}
				Lock(gThreadPoolMutex);
					gThreadPool.erase(gThreadPool.begin()+i,gThreadPool.begin()+i+1);
				Unlock(gThreadPoolMutex);
			}
cont:
			Lock(gThreadPoolMutex);
				n=gThreadPool.size();
			Unlock(gThreadPoolMutex);
		}
	}

	Lock(gThreadPoolMutex);
		while(gThreadPool.size()>0) {
			TypedValue context=gThreadPool.back();
			gThreadPool.pop_back();
			TerminateThread(context.contextPtr->thread.get());
			Lock(context.contextPtr->mutexForJoined);
				if(context.contextPtr->joined==false) {
					Join(context.contextPtr->thread.get());
					context.contextPtr->joined=true;
				}
			Unlock(context.contextPtr->mutexForJoined);
		}
	Unlock(gThreadPoolMutex);
	
	return NULL;
}

