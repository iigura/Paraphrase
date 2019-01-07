#pragma once

#include <stdlib.h>

#include "thread.h"

template<typename T> struct Array {
	int length;
	T *data;
	bool disposable=false;

	Mutex mutex;

	Array():length(-1),data(NULL) {
		initMutex(mutex);	
	}

	Array(const Array& inSrc)
	  :length(inSrc.length),data(inSrc.data) {
		initMutex(mutex);
	}

	Array(int inLength,T *inDataBody,bool inDisposable=false)
		:length(inLength),data(inDataBody),disposable(inDisposable) {
		initMutex(mutex);
	}

	~Array() {
		if( disposable ) {
			delete[] data;
		}
	}
};

