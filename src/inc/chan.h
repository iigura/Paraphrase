#pragma once

#include <assert.h>
#include <deque>

#include "thread.h"
#include "typedValue.h"

const int kDefaultFifoSize=1024;

struct ChanMan {
	const char *name;

	Mutex mutex;
	Cond cond;

	volatile bool IsOpen;

	std::deque<TypedValue> fifoBuffer;
	
	volatile int NumOfWriter;
	volatile int NumOfReader;

	ChanMan(const int inNumOfWriter,const char *inName) : IsOpen(true),name(inName),
	  NumOfWriter(inNumOfWriter),NumOfReader(0) {
		// printf("ChanMan create name='%s' NumOfWriter=%d\n",name,NumOfWriter);
		assert(inNumOfWriter>0);
		initMutex(mutex);
		initCond(cond);
	}

	PP_API ~ChanMan();
	PP_API bool CanDestruct();

	inline void IncNumOfReader() {
		Lock(mutex);	
			NumOfReader++;
		Unlock(mutex);
	}

	PP_API bool RemoveReader();
	PP_API void CloseOnWrite();

	inline bool Send(const TypedValue& inTV) NOEXCEPT {
		bool ret=true;
		Lock(mutex);	
			if(IsOpen==false) {
				ret=false;
			} else {
				fifoBuffer.emplace_back(inTV);
			}
			NotifyOne(cond);
		Unlock(mutex);	
		return ret;
	}

	inline TypedValue Recv() NOEXCEPT {
		TypedValue ret(DataType::EoC);
		LockForWait(mutex);	
			while(fifoBuffer.empty() && NumOfWriter>0) {
				CondWait(cond,mutex);	
			}
			if(fifoBuffer.empty()==false) {
				ret=fifoBuffer.front();
				fifoBuffer.pop_front();
			} else if(NumOfWriter==0) {
				IsOpen=false;
			}
		UnlockForWait(mutex);	
		return ret;
	}
};

