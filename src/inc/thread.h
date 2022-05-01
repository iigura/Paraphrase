#pragma once

#ifdef USE_PTHREAD
	#include <pthread.h>
	typedef pthread_t Thread;
	typedef pthread_mutex_t Mutex;
	typedef pthread_cond_t  Cond;

	#define initMutex(mtx) mtx=PTHREAD_MUTEX_INITIALIZER
	#define initCond(cnd)  cnd=PTHREAD_COND_INITIALIZER
	inline void Lock(Mutex& inMutex)   { pthread_mutex_lock(&inMutex);   }
	inline void Unlock(Mutex& inMutex) { pthread_mutex_unlock(&inMutex); }
	inline void NotifyOne(Cond& inCond) { pthread_cond_signal(&inCond);    }
	inline void NotifyAll(Cond& inCond) { pthread_cond_broadcast(&inCond); }
	inline void LockForWait(Mutex& inMutex)   { Lock(inMutex);   }
	inline void UnlockForWait(Mutex& inMutex) { Unlock(inMutex); }
	inline void CondWait(Cond& inCond,Mutex& inMutex) {
		pthread_cond_wait(&inCond,&inMutex);
	}	
	inline void NewThread(Thread& inThread,void *(*inFunc)(void *),void *inArg) {
		pthread_create(&inThread,NULL,inFunc,inArg);	
	}
	inline void Yield() { sched_yield(); }
#else
	#include <thread>
	#include <mutex>
	#include <condition_variable>
	typedef std::thread Thread;
	typedef std::mutex Mutex;
	typedef std::condition_variable Cond;

	#ifdef _WIN32
		#pragma warning(disable : 26110)
	#endif
	#define initMutex(mtx) // do nothing
	#define initCond(cnd)  // do nothing
	inline void Lock(Mutex& inMutex)   { inMutex.lock();   }
	inline void Unlock(Mutex& inMutex) { inMutex.unlock(); }
	inline void NotifyOne(Cond& inCond) { inCond.notify_one(); }
	inline void NotifyAll(Cond& inCond) { inCond.notify_all(); }
	#define LockForWait(mtx) std::unique_lock<Mutex> _lk(mtx)
	#define UnlockForWait(mtx)
	#define CondWait(cnd,mtx) cnd.wait(_lk)
	#define NewThread(th,func,arg) th=std::thread(func,arg)
	inline void Yield() { std::this_thread::yield(); }
#endif
