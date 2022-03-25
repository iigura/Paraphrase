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
	  NumOfWriter(inNumOfWriter),NumOfReader(-1) {
		assert(inNumOfWriter>0);
		initMutex(mutex);
		initCond(cond);
	}

	~ChanMan() {
		assert(NumOfWriter<=0 && NumOfReader<=0);
	}

	inline void SetNumOfReader(const int inNumOfReader) {
		Lock(mutex);	
			assert(NumOfReader<0);
			NumOfReader=inNumOfReader;
		Unlock(mutex);
	}

	inline bool RemoveReader() {
		bool ret=true;
		Lock(mutex);	
			if(NumOfReader<0) {	// rest-pipes on only initialized channel.
				// empty
			} else {
				assert(NumOfReader>0);
				NumOfReader--;
				if(NumOfReader==0 && IsOpen) {
					IsOpen=false;
					if(fifoBuffer.size()==0) {
						ret=false;
					}
					fifoBuffer.clear();			
				}
				NotifyOne(cond);
			}
		Unlock(mutex);	
		return ret;
	}

	inline void CloseOnWrite() {
		Lock(mutex);	
			assert(NumOfWriter>0);
			NumOfWriter--;
			if(NumOfWriter==0) {
				NotifyAll(cond);
			} else {	
				NotifyOne(cond);	
			}
		Unlock(mutex);	
	}

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

