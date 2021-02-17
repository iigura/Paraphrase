#include <array>
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

struct MathOpOptDB {
	TypedValue originalOp;
	std::vector<TypedValue> replaceOp;	// int,long,float,dobule,BigInt,BigFloat

	MathOpOptDB(const TypedValue& inOriginalOp,
				const std::initializer_list<TypedValue>& inReplaceOp)
     :originalOp(inOriginalOp),replaceOp(inReplaceOp) {
		assert(inReplaceOp.size()==6);
	}

	MathOpOptDB() {
		// empty
	}
};

static OptPattern *gOptDB=NULL;
static TypedValue gTvLit;	// == getWord("std:_lit");
static MathOpOptDB *gMathOp=NULL;
static TypedValue gWordLit[4];
enum class LitType {
   	kLitInt   =0,
	kLitLong  =1,
	kLitFloat =2,
	kLitDouble=3,
};

static TypedValue getWord(const char *inWordName);
static bool optimize(CodeThread *ioThread,const OptPattern& inPattern);
static bool mathOpOptimize(CodeThread *ioThread);
static bool litOptimize(CodeThread *ioThread);

static bool lvopOptimize(CodeThread *ioThread);
static bool toDsOpCheck(CodeThread *inThread,int inTarget,LVOP inLVOP);
static bool fromDsOpCheck(CodeThread *inThread,int inTarget,LVOP inLVOP);
static bool isLVOpSupport(CodeThread *inThread,int inTarget,LVOP inArgMask);
static bool isPushLVOP(CodeThread *inThread,int inTarget);

static bool lvopPack(CodeThread *ioThread);

static bool canGrouping(const CodeThread *inThread,
						const int inStartAddress,const int inBlockLength);
static void updateCode(CodeThread *ioThread,
					   int inStartPos,
					   std::vector<TypedValue>& inNewCode,
					   int inOldSize);

PP_API void InitOptPattern() {
	static OptPattern optDB[]={
		OptPattern({getWord("std:_0"),getWord("std:_setValue")},{getWord("std:>$0")}),
		OptPattern({getWord("std:_1"),getWord("std:_setValue")},{getWord("std:>$1")}),
		OptPattern({getWord("std:_2"),getWord("std:_setValue")},{getWord("std:>$2")}),
		OptPattern({getWord("std:_3"),getWord("std:_setValue")},{getWord("std:>$3")}),
		OptPattern({getWord("std:_4"),getWord("std:_setValue")},{getWord("std:>$4")}),
		OptPattern({getWord("std:_5"),getWord("std:_setValue")},{getWord("std:>$5")}),
		OptPattern({getWord("std:_6"),getWord("std:_setValue")},{getWord("std:>$6")}),
		OptPattern({getWord("std:_7"),getWord("std:_setValue")},{getWord("std:>$7")}),
		OptPattern({getWord("std:_8"),getWord("std:_setValue")},{getWord("std:>$8")}),
		OptPattern({getWord("std:_9"),getWord("std:_setValue")},{getWord("std:>$9")}),

		OptPattern({getWord("std:_0"),getWord("std:_getValue")},{getWord("std:$0>")}),
		OptPattern({getWord("std:_1"),getWord("std:_getValue")},{getWord("std:$1>")}),
		OptPattern({getWord("std:_2"),getWord("std:_getValue")},{getWord("std:$2>")}),
		OptPattern({getWord("std:_3"),getWord("std:_getValue")},{getWord("std:$3>")}),
		OptPattern({getWord("std:_4"),getWord("std:_getValue")},{getWord("std:$4>")}),
		OptPattern({getWord("std:_5"),getWord("std:_getValue")},{getWord("std:$5>")}),
		OptPattern({getWord("std:_6"),getWord("std:_getValue")},{getWord("std:$6>")}),
		OptPattern({getWord("std:_7"),getWord("std:_getValue")},{getWord("std:$7>")}),
		OptPattern({getWord("std:_8"),getWord("std:_getValue")},{getWord("std:$8>")}),
		OptPattern({getWord("std:_9"),getWord("std:_getValue")},{getWord("std:$9>")}),

		// "dup 2 % 0 ==" -> "@even?"
		OptPattern({getWord("std:dup"),
					getWord("std:_litInt"),TypedValue(2),getWord("std:%"),
					getWord("std:_litInt"),TypedValue(0),getWord("std:==")},
				   {getWord("std:@even?")}),

		OptPattern({getWord("std:>r"),getWord("std:>r")},{getWord("std:>r>r")}),
		OptPattern({getWord("std:r>"),getWord("std:r>")},{getWord("std:r>r>")}),
		// "0 ==" convert to "0?"
		OptPattern({getWord("std:_lit"),TypedValue(0),getWord("std:==")},
				   {getWord("std:0?")}),
		OptPattern({getWord("std:_lit"),TypedValue(2),getWord("std:/")},
				   {getWord("std:2/")}),
		OptPattern({getWord("std:_2"),getWord("std:/")},{getWord("std:2/")}),
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
		//OptPattern({getWord("std:_repeat?+"),getWord("std:_branch-if-false")},
		//		   {getWord("std:_branchWhenLoopEnded")}),
		OptPattern({getWord("std:_lit"),TypedValue(2),getWord("std:_step+")},
				   {getWord("std:_step++")}),
		OptPattern({getWord("std:swap"),getWord("std:_r>+"),getWord("std:swap")},
				   {getWord("std:_r>+_second")}),
		OptPattern({getWord("std:dup"),getWord("std:empty-list?")},
				   {getWord("std:@empty-list?")}),
		OptPattern({getWord("std:dup"),getWord("std:not-empty-list?")},
				   {getWord("std:@not-empty-list?")}),
		OptPattern({getWord("std:dup"),getWord("std:*")},{getWord("std:_square")}),
		OptPattern({getWord("std:dup"),getWord("std:+")},{getWord("std:2*")}),
		OptPattern({getWord("std:+"),getWord(">r")},{getWord("std:+>r")}),

		OptPattern({getWord("std:_litInt"),TypedValue(0)},{getWord("std:_0")}),
		OptPattern({getWord("std:_litInt"),TypedValue(1)},{getWord("std:_1")}),
		OptPattern({getWord("std:_litInt"),TypedValue(2)},{getWord("std:_2")}),
		OptPattern({getWord("std:_litInt"),TypedValue(3)},{getWord("std:_3")}),
		OptPattern({getWord("std:_litInt"),TypedValue(4)},{getWord("std:_4")}),
		OptPattern({getWord("std:_litInt"),TypedValue(5)},{getWord("std:_5")}),
		OptPattern({getWord("std:_litInt"),TypedValue(6)},{getWord("std:_6")}),
		OptPattern({getWord("std:_litInt"),TypedValue(7)},{getWord("std:_7")}),
		OptPattern({getWord("std:_litInt"),TypedValue(8)},{getWord("std:_8")}),
		OptPattern({getWord("std:_litInt"),TypedValue(9)},{getWord("std:_9")}),
		OptPattern({getWord("std:drop"),getWord("std:drop"),getWord("std:drop")},
				   {getWord("std:3drop")}),
		OptPattern({getWord("std:drop"),getWord("std:drop")},{getWord("std:2drop")}),

		OptPattern({getWord("std:_lit"),TypedValue(true)},{getWord("std:true")}),
		OptPattern({getWord("std:_lit"),TypedValue(false)},{getWord("std:false")}),

		OptPattern({getWord("std:_2"),getWord("std:pick")},{getWord("std:2pick")}),
		OptPattern({getWord("std:_3"),getWord("std:pick")},{getWord("std:3pick")}),
		OptPattern({getWord("std:_4"),getWord("std:pick")},{getWord("std:4pick")}),
		OptPattern({getWord("std:_5"),getWord("std:pick")},{getWord("std:5pick")}),
		OptPattern({getWord("std:_6"),getWord("std:pick")},{getWord("std:6pick")}),
		OptPattern({getWord("std:_7"),getWord("std:pick")},{getWord("std:7pick")}),
		OptPattern({getWord("std:_8"),getWord("std:pick")},{getWord("std:8pick")}),
		OptPattern({getWord("std:_9"),getWord("std:pick")},{getWord("std:9pick")}),

		OptPattern({getWord("std:_1"),getWord("std:replace")},
				   {getWord("std:1replace")}),
		OptPattern({getWord("std:_2"),getWord("std:replace")},
				   {getWord("std:2replace")}),
		OptPattern({getWord("std:_3"),getWord("std:replace")},
				   {getWord("std:3replace")}),
		OptPattern({getWord("std:_4"),getWord("std:replace")},
				   {getWord("std:4replace")}),

		OptPattern({getWord("std:_1"),getWord("std:+")},{getWord("std:_inc")}),
		OptPattern({getWord("std:_1"),getWord("std:-")},{getWord("std:_dec")}),

		OptPattern({getWord("std:$0>"),getWord("std:_inc"),getWord("std:>$0")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s0 | LVOP::d0)}),
		OptPattern({getWord("std:$1>"),getWord("std:_inc"),getWord("std:>$1")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s1 | LVOP::d1)}),
		OptPattern({getWord("std:$2>"),getWord("std:_inc"),getWord("std:>$2")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s2 | LVOP::d2)}),
		OptPattern({getWord("std:$3>"),getWord("std:_inc"),getWord("std:>$3")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s3 | LVOP::d3)}),
		OptPattern({getWord("std:$4>"),getWord("std:_inc"),getWord("std:>$4")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s4 | LVOP::d4)}),
		OptPattern({getWord("std:$5>"),getWord("std:_inc"),getWord("std:>$5")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s5 | LVOP::d5)}),
		OptPattern({getWord("std:$6>"),getWord("std:_inc"),getWord("std:>$6")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s6 | LVOP::d6)}),
		OptPattern({getWord("std:$7>"),getWord("std:_inc"),getWord("std:>$7")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s7 | LVOP::d7)}),
		OptPattern({getWord("std:$8>"),getWord("std:_inc"),getWord("std:>$8")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s8 | LVOP::d8)}),
		OptPattern({getWord("std:$9>"),getWord("std:_inc"),getWord("std:>$9")},
				   {getWord("std:_lvop"),TypedValue(LVOP::INC | LVOP::s9 | LVOP::d9)}),

		OptPattern({getWord("std:$0>"),getWord("std:_dec"),getWord("std:>$0")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s0 | LVOP::d0)}),
		OptPattern({getWord("std:$1>"),getWord("std:_dec"),getWord("std:>$1")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s1 | LVOP::d1)}),
		OptPattern({getWord("std:$2>"),getWord("std:_dec"),getWord("std:>$2")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s2 | LVOP::d2)}),
		OptPattern({getWord("std:$3>"),getWord("std:_dec"),getWord("std:>$3")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s3 | LVOP::d3)}),
		OptPattern({getWord("std:$4>"),getWord("std:_dec"),getWord("std:>$4")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s4 | LVOP::d4)}),
		OptPattern({getWord("std:$5>"),getWord("std:_dec"),getWord("std:>$5")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s5 | LVOP::d5)}),
		OptPattern({getWord("std:$6>"),getWord("std:_dec"),getWord("std:>$6")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s6 | LVOP::d6)}),
		OptPattern({getWord("std:$7>"),getWord("std:_dec"),getWord("std:>$7")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s7 | LVOP::d7)}),
		OptPattern({getWord("std:$8>"),getWord("std:_dec"),getWord("std:>$8")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s8 | LVOP::d8)}),
		OptPattern({getWord("std:$9>"),getWord("std:_dec"),getWord("std:>$9")},
				   {getWord("std:_lvop"),TypedValue(LVOP::DEC | LVOP::s9 | LVOP::d9)}),

		OptPattern({getWord("std:_0"),getWord("std:==")},{getWord("std:0?")}),

		OptPattern({getWord("std:dup"),getWord("std:_1"),getWord("std:!=")},
				   {getWord("std:_@1!=")}),

		OptPattern(),	// the sentinel
	};

	gOptDB=optDB;
	gTvLit=getWord("std:_lit");

	static MathOpOptDB mathOpDB[]={
		MathOpOptDB(getWord("std:*"),
					{getWord("std:_int*"),   getWord("std:_long*"),
					 getWord("std:_float*"), getWord("std:_double*"),
					 getWord("std:_bigInt*"),getWord("std:_bigFloat*")}),
		MathOpOptDB(),
	};
	gMathOp=mathOpDB;

	gWordLit[(int)LitType::kLitInt]   =getWord("std:_litInt");
	gWordLit[(int)LitType::kLitLong]  =getWord("std:_litLong");
	gWordLit[(int)LitType::kLitFloat] =getWord("std:_litFloat");
	gWordLit[(int)LitType::kLitDouble]=getWord("std:_litDouble");
}

int gOptimizeLevel=5;

PP_API void SetOptimizeLevel(int inOptimizeLevel) {
	gOptimizeLevel=inOptimizeLevel;
}
PP_API int GetOptimizeLevel() {
	return gOptimizeLevel;
}

void Optimize(CodeThread *ioThread) {
	if(gOptimizeLevel==0) { return; }	// no optimize
	while(mathOpOptimize(ioThread)) { /* empty */ }
	while(litOptimize(ioThread)) { /* empty */ }

	bool changed;
	do {
		changed=false;
		for(int i=0; gOptDB[i].isValid; i++) {	
			changed |= optimize(ioThread,gOptDB[i]);
		}
	} while( changed );

	while(lvopOptimize(ioThread))   { /* empty */ }

	do {
		changed=lvopPack(ioThread);
	} while( changed );
}

void ReplaceTailRecursionToJump(Word *inWord,CodeThread *ioThread) {
	size_t n=ioThread->size();
	if(n<2) { return; }
	TypedValue& semis=ioThread->at(n-1);
	if(semis.dataType!=DataType::kTypeDirectWord || semis.wordPtr!=Dict["std:_semis"]) {
		return;
	}
	TypedValue& lastCall=ioThread->at(n-2);
	if(lastCall.dataType!=DataType::kTypeDirectWord || lastCall.wordPtr!=inWord) {
		return;
	}

	lastCall.wordPtr=Dict["std:_absolute-jump"];
	auto insertPos=ioThread->begin()+n-1;
	ioThread->insert(insertPos,TypedValue(DataType::kTypeAddress,0));

	for(size_t i=0; i<n-2; i++) {
		TypedValue& tv=ioThread->at(i);
		if(tv.dataType!=DataType::kTypeAddress || tv.intValue<=n-2) { continue; }
		tv.intValue++;	// jump to semis.
	}
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
			if(ioThread->at(k).dataType==DataType::kTypeAddress
			   && ioThread->at(k).intValue>i) {
				ioThread->at(k).intValue-=inPattern.gap;
			}
		}
		ret=true;
	}
	return ret;
}

static bool mathOpOptimize(CodeThread *ioThread) {
	bool ret=false;
	for(int i=0; i<(int)ioThread->size()-3; i++) {	// 3 == sizeof{lit,N,OP}
		if(!(ioThread->at(i)==gTvLit)) { continue; }
		TypedValue& tvN=ioThread->at(i+1);
		if(tvN.IsNumber()==false) { continue; }
		TypedValue& op=ioThread->at(i+2);
		int mathOpDB_index=-1;
		for(int j=0; gMathOp[j].originalOp.dataType==DataType::kTypeDirectWord; j++) {
			if(op==gMathOp[j].originalOp) { mathOpDB_index=j; break; }
		}	
		if(mathOpDB_index<0) { continue; }
		int replaceIndex=-1;
		switch(tvN.dataType) {
			case DataType::kTypeInt:		replaceIndex=0;	break;
			case DataType::kTypeLong:		replaceIndex=1;	break;
			case DataType::kTypeFloat:		replaceIndex=2;	break;
			case DataType::kTypeDouble:		replaceIndex=3; break;
			case DataType::kTypeBigInt:		replaceIndex=4;	break;
			case DataType::kTypeBigFloat:	replaceIndex=5; break;
			default:
				fprintf(stderr,"SYSTEM ERROR in mathOpOptimise.\n");
				exit(-1);
		}
		ioThread->at(i+2)=gMathOp[mathOpDB_index].replaceOp[replaceIndex];
		ret=true;
	}
	return ret;
}

static bool litOptimize(CodeThread *ioThread) {
	bool ret=false;
	for(int i=0; i<(int)ioThread->size()-2; i++) {	// 3 == sizeof{lit,N}
		if(!(ioThread->at(i)==gTvLit)) { continue; }
		TypedValue& tvN=ioThread->at(i+1);
		if(tvN.IsNumber()==false) { continue; }
		int index=-1;
		switch(tvN.dataType) {
			case DataType::kTypeInt:	index=(int)LitType::kLitInt;	break;
			case DataType::kTypeLong:	index=(int)LitType::kLitLong;	break;
			case DataType::kTypeFloat:	index=(int)LitType::kLitFloat;	break;
			case DataType::kTypeDouble:	index=(int)LitType::kLitDouble;	break;
			default:
				;	// empty
		}
		if(index<0) { continue; }
		ioThread->at(i)=gWordLit[index];
		ret=true;
	}
	return ret;
}

static bool lvopOptimize(CodeThread *ioThread) {
	bool ret=false;
	for(int i=0; i<ioThread->size(); i++) {
		LVOP newOP;
		std::vector<TypedValue> newCode;
		int originalSize=-1;
		int newSize=-1;
		if(toDsOpCheck(ioThread,i,LVOP::TST) && toDsOpCheck(ioThread,i+1,LVOP::TST)
		  && isLVOpSupport(ioThread,i+2,LVOP::LVOpSupported2args)
		  && fromDsOpCheck(ioThread,i+3,LVOP::TLV)
		  && canGrouping(ioThread,i,4)) {
			LVOP src1=ioThread->at(i).wordPtr->LVOpHint & LVOP::src1Mask;
			LVOP src2=((ioThread->at(i+1).wordPtr->LVOpHint & LVOP::src1Mask)) >> 4;
			LVOP op=ioThread->at(i+2).wordPtr->LVOpHint & LVOP::opMask;
			LVOP dest=ioThread->at(i+3).wordPtr->LVOpHint & LVOP::destMask;

			newOP=op | src1 | src2 | dest;
			newCode=std::vector<TypedValue>({getWord("std:_lvop"),TypedValue(newOP)});
			originalSize=4;
			newSize=2;
		} else if(toDsOpCheck(ioThread,i,LVOP::TST)
				  && isLVOpSupport(ioThread,i+1,LVOP::LVOpSupported1args)
				  && fromDsOpCheck(ioThread,i+2,LVOP::TLV)
				  && canGrouping(ioThread,i,3)) {
			LVOP src1=ioThread->at(i).wordPtr->LVOpHint & LVOP::src1Mask;
			LVOP op=ioThread->at(i+1).wordPtr->LVOpHint & LVOP::opMask;
			LVOP dest=ioThread->at(i+2).wordPtr->LVOpHint & LVOP::destMask;

			newOP=op | src1 | dest;
			newCode=std::vector<TypedValue>({getWord("std:_lvop"),TypedValue(newOP)});
			originalSize=3;
			newSize=2;
		} else if(toDsOpCheck(ioThread,i,LVOP::TST)
				  && toDsOpCheck(ioThread,i+1,LVOP::TST)
				  && isLVOpSupport(ioThread,i+2,LVOP::LVOpSupported2args)
				  && canGrouping(ioThread,i,3)) {
			LVOP src1=ioThread->at(i).wordPtr->LVOpHint & LVOP::src1Mask;
			LVOP src2=((ioThread->at(i+1).wordPtr->LVOpHint & LVOP::src1Mask)) >> 4;
			LVOP op=ioThread->at(i+2).wordPtr->LVOpHint & LVOP::opMask;

			newOP=LVOP::XPushBase | op | src1 | src2 | LVOP::dDS;
			newCode=std::vector<TypedValue>({getWord("std:_lvop"),TypedValue(newOP)});
			originalSize=3;
			newSize=2;
		} else if(i>1 && isPushLVOP(ioThread,i-1)
				  && toDsOpCheck(ioThread,i,LVOP::TST)
				  && isLVOpSupport(ioThread,i+1,LVOP::LVOpSupported2args)
				  && fromDsOpCheck(ioThread,i+2,LVOP::TLV)
				  && canGrouping(ioThread,i,3)) {
			LVOP src1=(LVOP)(ioThread->at(i-1).intValue) & LVOP::destMask;
			LVOP src2=((ioThread->at(i).wordPtr->LVOpHint & LVOP::src1Mask)) >> 4;
			LVOP op=ioThread->at(i+1).wordPtr->LVOpHint & LVOP::opMask;
			LVOP dest=ioThread->at(i+2).wordPtr->LVOpHint & LVOP::destMask;
			newOP=LVOP::XPopBase | op | src1 | src2 | dest;
			newCode=std::vector<TypedValue>({getWord("std:_lvop"),TypedValue(newOP)});
			originalSize=3;
			newSize=2;
		} else if(toDsOpCheck(ioThread,i,LVOP::TST)
				  && isLVOpSupport(ioThread,i+1,LVOP::LVOpSupported1args)
				  && canGrouping(ioThread,i,2)) {
			LVOP src1=ioThread->at(i).wordPtr->LVOpHint & LVOP::src1Mask;
			LVOP op=ioThread->at(i+1).wordPtr->LVOpHint & LVOP::opMask;
			newOP=LVOP::XPushBase | op | src1 | LVOP::dDS;
			newCode=std::vector<TypedValue>({getWord("std:_lvop"),TypedValue(newOP)});
			originalSize=2;
			newSize=2;
		}

		if(originalSize>=0) {
			updateCode(ioThread,i,newCode,originalSize);
			ret=true;
		}
	}
	return ret;
}

static bool toDsOpCheck(CodeThread *inThread,int inTarget,LVOP inLVOP) {
	if(inThread->size()<=inTarget) { return false; }
	TypedValue& tv=inThread->at(inTarget);
	if(tv.dataType!=DataType::kTypeDirectWord) { return false; }
	LVOP lvop=tv.wordPtr->LVOpHint;
	if((lvop&LVOP::opMask)!=inLVOP) { return false; }
	return (lvop & LVOP::destMask)==LVOP::dDS;
}

static bool fromDsOpCheck(CodeThread *inThread,int inTarget,LVOP inLVOP) {
	if(inThread->size()<=inTarget) { return false; }
	TypedValue& tv=inThread->at(inTarget);
	if(tv.dataType!=DataType::kTypeDirectWord) { return false; }
	LVOP lvop=tv.wordPtr->LVOpHint;
	if((lvop&LVOP::opMask)!=inLVOP) { return false; }
	return (lvop & LVOP::src1Mask)==LVOP::sDS;
}

static bool isLVOpSupport(CodeThread *inThread,int inTarget,LVOP inArgMask) {
	if(inThread->size()<=inTarget) { return false; }
	TypedValue& tv=inThread->at(inTarget);
	if(tv.dataType!=DataType::kTypeDirectWord) { return false; }
	LVOP lvop=tv.wordPtr->LVOpHint;
	return (lvop & inArgMask) !=0;
}

static bool isPushLVOP(CodeThread *inThread,int inTarget) {
	if(inThread->size()<=inTarget) { return false; }
	TypedValue& tv=inThread->at(inTarget);
	if(tv.dataType!=DataType::kTypeLVOP) { return false; }
	LVOP lvop=static_cast<LVOP>(tv.intValue);
	return (lvop & LVOP::XPushBase) !=0;
}

static bool lvopPack(CodeThread *ioThread) {
	bool ret=false;
	TypedValue tvLvopWord=getWord("std:_lvop");
	TypedValue tv_N_LvopWord=getWord("std:_nLVOP");
	for(int i=0; i<(int)ioThread->size()-4; i++) {
		if(ioThread->at(i)==tvLvopWord
		   && ioThread->at(i+1).dataType==DataType::kTypeLVOP
		   && ioThread->at(i+2)==tvLvopWord
		   && ioThread->at(i+3).dataType==DataType::kTypeLVOP) {
			int lvopCount=0;
			for(int t=0;
				t<ioThread->size() && ioThread->at(i+t)==tvLvopWord
				&& ioThread->at(i+t+1).dataType==DataType::kTypeLVOP
				&& canGrouping(ioThread,i,t+2);
				t+=2, lvopCount++) {
				// empty
			}	
			assert(lvopCount>=2);		
			// build _nLVOP	numOfLVOP LVOP1 LVOP2 ... LVOPn
			std::vector<TypedValue> newCode;
			newCode.emplace_back(tv_N_LvopWord);
			newCode.emplace_back(lvopCount);
			for(int j=0; j<lvopCount; j++) {
				newCode.emplace_back(ioThread->at(i+j*2+1));
			}

			int originalSize=lvopCount*2;
			updateCode(ioThread,i,newCode,originalSize);
			ret=true;
		}
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
		if(tv.dataType==DataType::kTypeAddress) {
			const int addr=tv.intValue;
			if(inStartAddress<addr && addr<endAddress) {
				// jump into target block
				return false;
			}
		}
	}
	return true;
}

static void updateCode(CodeThread *ioThread,
					   int inStartPos,
					   std::vector<TypedValue>& inNewCode,
					   int inOldSize) {
	auto p=ioThread->begin()+inStartPos;
	p=ioThread->erase(p,p+inOldSize);
	ioThread->insert(p,inNewCode.begin(),inNewCode.end());

	int offset=inOldSize-(int)inNewCode.size();
	for(int k=0; k<ioThread->size(); k++) {
		if(ioThread->at(k).dataType==DataType::kTypeAddress
		   && ioThread->at(k).intValue>inStartPos) {
				ioThread->at(k).intValue-=offset;
		}
	}
}

