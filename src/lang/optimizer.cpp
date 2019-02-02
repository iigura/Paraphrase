#include <vector>
#include <initializer_list>

#include "typedValue.h"
#include "word.h"

#include "externals.h"

struct OptPattern {
	bool isValid=true;
	std::vector<TypedValue> target;
	std::vector<TypedValue> replaceTo;
	int gap;

	OptPattern(const std::initializer_list<TypedValue>& inTargetList,
			   const std::initializer_list<TypedValue>& inReplaceToList)
	  :target(inTargetList),replaceTo(inReplaceToList) {
		gap=(int)target.size()-(int)replaceTo.size();
	}

	OptPattern():isValid(false) {
		// empty
	}
};

static TypedValue getWord(const char *inWordName);
static bool optimize(CodeThread *ioThread,const OptPattern& inPattern);
static bool canGrouping(const CodeThread *inThread,
						const int inStartAddress,const int inBlockLength);

static OptPattern *gOptDB;

PP_API void InitOptPattern() {
	static OptPattern optDB[]={
		OptPattern({getWord("std:>r"),getWord("std:>r")},{getWord("std:>r>r")}),
		// "0 ==" convert to "0?"
		OptPattern({getWord("std:_lit"),TypedValue(0),getWord("std:==")},
				   {getWord("std:0?")}),
		OptPattern({getWord("std:_lit"),TypedValue(2),getWord("std:/")},
				   {getWord("std:2/")}),
		OptPattern({getWord("std:dup"),getWord("std:@r>"),getWord("std:%")},
				   {getWord("std:_dup_@r>%")}),
		OptPattern({getWord("std:dup"),getWord("std:i"),getWord("std:%")},
				   {getWord("std:_dup_@r>%")}),
		OptPattern({getWord("std:0?"),getWord("std:_branch-if-false")},
				   {getWord("std:_branchIfNotZero")}),
		OptPattern({getWord("std:@r>"),getWord("std:%")},{getWord("std:_r>%")}),
		OptPattern({getWord("std:@r>"),getWord("std:+")},{getWord("std:_r>+")}),
		OptPattern({getWord("std:i"),getWord("std:%")},{getWord("std:_r>%")}),
		OptPattern({getWord("std:i"),getWord("std:+")},{getWord("std:_r>+")}),
		OptPattern({getWord("std:dup"),getWord("std:i")},{getWord("std:_dup_i")}),
		OptPattern({getWord("std:_repeat?+"),getWord("std:_branch-if-false")},
				   {getWord("std:_branchWhenLoopEnded")}),
		OptPattern({getWord("std:_lit"),TypedValue(2),getWord("std:_step+")},
				   {getWord("std:_step++")}),
		OptPattern({getWord("std:swap"),getWord("std:_r>+"),getWord("std:swap")},
				   {getWord("std:_r>+_second")}),
		OptPattern({getWord("std:dup"),getWord("std:empty-list?")},
				   {getWord("std:@empty-list?")}),
		OptPattern({getWord("std:dup"),getWord("std:not-empty-list?")},
				   {getWord("std:@not-empty-list?")}),
		OptPattern({getWord("std:_lit"),TypedValue(1)},{getWord("std:_1")}),
		OptPattern({getWord("std:_lit"),TypedValue(2)},{getWord("std:_2")}),
		OptPattern(),	// the sentinel
	};
	gOptDB=optDB;
}

void Optimize(CodeThread *ioThread) {
	bool changed;
	do {
		changed=false;
		for(int i=0; gOptDB[i].isValid; i++) {	
			changed |= optimize(ioThread,gOptDB[i]);
		}
	} while( changed );
}

static TypedValue getWord(const char *inWordName) {
	auto iter=Dict.find(std::string(inWordName));
	if(iter==Dict.end()) {
		fprintf(stderr,"SYSTEM ERROR at getWord."
					   " the  word name is '%s'.\n",inWordName);
		exit(-1);
	}
	return TypedValue(iter->second);
}

static bool optimize(CodeThread *ioThread,const OptPattern& inPattern) {
	bool ret=false;
	const int patternTargetSize=(int)inPattern.target.size();
	for(int i=0; i<=(int)ioThread->size()-patternTargetSize; i++) {
		bool match=true;
		for(size_t j=0; j<patternTargetSize; j++) {
			if(!(ioThread->at(i+j)==inPattern.target[j])) {
				match=false;
				break;
			}
		}
		if(match==false) { continue; }

		if(canGrouping(ioThread,i,patternTargetSize)==false) { continue; }

		auto p=ioThread->begin()+i;
		p=ioThread->erase(p,p+patternTargetSize);
		ioThread->insert(p,inPattern.replaceTo.begin(),
						   inPattern.replaceTo.end());

		for(int k=0; k<ioThread->size(); k++) {
			if(ioThread->at(k).dataType==kTypeAddress
			   && ioThread->at(k).intValue>i) {
				ioThread->at(k).intValue-=inPattern.gap;
			}
		}
		ret=true;
	}
	return ret;
}

static bool canGrouping(const CodeThread *inThread,
						const int inStartAddress,const int inBlockLength) {
	// target block address is [inStartAddress,endAddress)
	const int endAddress=inStartAddress+inBlockLength;
	const int n=(int)inThread->size();
	for(int i=0; i<n; i++) {
		const TypedValue& tv=inThread->at(i);
		if(tv.dataType==kTypeAddress) {
			const int addr=tv.intValue;
			if(inStartAddress<addr && addr<endAddress) {
				// jump into target block
				return false;
			}
		}
	}
	return true;
}

