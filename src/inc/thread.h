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
	inline bool TryLock(Mutex& inMutex) {
		return pthread_mutex_trylock(&inMutex)==0;
	}
	inline void NotifyOne(Cond& inCond) { pthread_cond_signal(&inCond);    }
	inline void NotifyAll(Cond& inCond) { pthread_cond_broadcast(&inCond); }
	inline void LockForWait(Mutex& inMutex)   { Lock(inMutex);   }
	inline void UnlockForWait(Mutex& inMutex) { Unlock(inMutex); }
	inline void CondWait(Cond& inCond,Mutex& inMutex) {
		pthread_cond_wait(&inCond,&inMutex);
	}	
	#include <time.h>
	inline void CondTimeWait(Cond& inCond,Mutex& inMutex,int inSec) {
		struct timespec endAbsTime;
		clock_gettime(CLOCK_REALTIME,&endAbsTime);
		endAbsTime.tv_sec+=inSec;
		pthread_cond_timedwait(&inCond,&inMutex,&endAbsTime);
	}
	inline std::shared_ptr<Thread> NewSharedThread(void *(*inFunc)(void *),
												   void *inArg) {
		Thread *th=new Thread();
		pthread_create(th,NULL,inFunc,inArg);	
		return std::shared_ptr<Thread>(th);
	}
	inline void TerminateThread(Thread *inThread) {
		pthread_cancel(*inThread);
	}
	inline void Join(Thread *inThread) { pthread_join(*inThread,NULL); }
	inline void ppYield() { sched_yield(); }
#else
	#include <thread>
	#include <mutex>
	#include <condition_variable>
	typedef std::thread Thread;
	typedef std::mutex Mutex;
	typedef std::condition_variable Cond;

	#ifdef _WIN32
		#pragma warning(disable : 26110)
	#else
		#include <unistd.h>
	#endif
	#define initMutex(mtx) // do nothing
	#define initCond(cnd)  // do nothing
	inline void Lock(Mutex& inMutex)   { inMutex.lock();   }
	inline void Unlock(Mutex& inMutex) { inMutex.unlock(); }
	#define TryLock(mutex) (mutex.try_lock())
	inline void NotifyOne(Cond& inCond) { inCond.notify_one(); }
	inline void NotifyAll(Cond& inCond) { inCond.notify_all(); }
	#define LockForWait(mtx) std::unique_lock<Mutex> _lk(mtx)
	#define UnlockForWait(mtx)
	#define CondWait(cnd,mtx) cnd.wait(_lk)
	#include <chrono>
	#define CondTimeWait(cond,mutex,sec) cond.wait_for(_lk,std::chrono::seconds(sec))
	#define NewSharedThread(func,arg) std::shared_ptr<Thread>(new std::thread(func,arg))
	#ifdef _WIN32
		#define NOMINMAX
		#include <windows.h>
		inline void TerminateThread(Thread *inThread) {
			::TerminateThread(inThread->native_handle(),0);
		}
	#else
		#include <pthread.h>
		inline void TerminateThread(Thread *inThread) {
			if(inThread->native_handle()!=0) {
				pthread_cancel(inThread->native_handle());
			}
		}
	#endif
	inline void ppYield() { std::this_thread::yield(); }
	inline void Join(Thread *inThread) { inThread->join(); }
#endif

