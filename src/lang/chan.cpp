#include "chan.h"

PP_API ChanMan::~ChanMan() {
#ifdef DEBUG
	Lock(mutex);
		// printf("ChanMan DESTRUCT '%s' NumOfWriter=%d NumOfReader=%d\n",name,NumOfWriter,NumOfReader);
		assert(NumOfWriter<=0 && NumOfReader<=0);
	Unlock(mutex);
#endif
}

PP_API bool ChanMan::CanDestruct() {
	bool ret;
	Lock(mutex);
		ret=NumOfWriter<=0 && NumOfReader<=0;
	Unlock(mutex);
	return ret;
}

PP_API void ChanMan::CloseOnWrite() {
	Lock(mutex);	
		// printf("ChanMan CloseOnWrite name='%s' NumOfWriter=%d\n",name,NumOfWriter);
		assert(NumOfWriter>0);
		NumOfWriter--;
		if(NumOfWriter==0) {
			NotifyAll(cond);
		} else {	
			NotifyOne(cond);	
		}
	Unlock(mutex);	
}

PP_API bool ChanMan::RemoveReader() {
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

