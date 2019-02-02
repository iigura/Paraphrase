#include <stdarg.h>

#include "word.h"
#include "stack.h"
#include "context.h"
#include "inner.h"
#include "externals.h"
#include "optimizer.h"

const int DS_InitialStackSize=4096;
const int RS_InitialStackSize=1024;
const int SS_InitialStackSize=1024;
const int IS_InitialStackSize=1024;
const int ES_InitialStackSize=1024;

static std::vector< std::vector<int> > gStackForLeave;
static std::vector< std::vector<int> > gStackForBreak;

bool Error(const Context& inContext,const NoParamErrorID inErrorID);
bool ErrorWithStr(const Context& inContext,
				  const InvalidTypeErrorID,const std::string& inStr);
bool Error(const Context& inContext,const InvalidTypeTosSecondErrorID inErrorID,
		   const TypedValue& inTos,const TypedValue& inSecond);
bool Error(const Context& inContext,const InvalidTypeStrTvTvErrorID inErrorID,
		   const std::string& inStr,const TypedValue inTV1,const TypedValue& inTV2);
bool Error(const Context& inContext,const ErrorID_withInt inErrorID,
		   const int inIntValue);
bool Error(const Context& inContext,const ErrorID_withString inErrorID,
		   const char *inStr);
bool Error(const Context& inContext,
		   const ErrorID_withString inErrorID,const char *inStr);
bool Error(const Context& inContext,const ErrorID_with2int inErrorID,
		   const int inIntValue1,const int inIntValue2);

PP_API Context *GlobalContext=new Context(NULL,kInterpretLevel,NULL,NULL,NULL);

PP_API Context::Context(Context *inParent,int inExecutionThreshold,
				 std::shared_ptr<ChanMan> inFromParent,
				 std::shared_ptr<ChanMan> inToParent,
				 const std::vector<TypedValue> *inInitParamFromParent)
  :parent(inParent),ExecutionThreshold(inExecutionThreshold),
   fromParent(inFromParent),toParent(inToParent),
   toChild(),fromChild(),toPipe(),
   fromPipe(inParent!=NULL ? inParent->lastOutputPipe : NULL),
   lastOutputPipe(),initParamForPBlock(),isInitParamBroadcast(true) {
	DS.reserve(DS_InitialStackSize);
	RS.reserve(RS_InitialStackSize);
	SS.reserve(SS_InitialStackSize);
	ES.reserve(ES_InitialStackSize);
	IS.reserve(IS_InitialStackSize);

	newWord=NULL;

	if(inInitParamFromParent!=NULL) {
		const size_t n=inInitParamFromParent->size();
		for(size_t i=0; i<n; i++) {
			DS.push_back(inInitParamFromParent->at(i));
		}
	}
}


PP_API void Context::RemoveDefiningWord() {
	if(newWord->shortName.length()!=0) {
		Uninstall(newWord);
	}
	delete newWord;
	newWord=NULL;
	lastDefinedWord=NULL;
}

PP_API bool Context::Exec(const TypedValue& inTV) {
	if(inTV.dataType!=kTypeDirectWord
	  && inTV.dataType!=kTypeNewWord
	  && inTV.dataType!=kTypeWord) {
		return Error_InvalidType(E_TOS_WP,inTV);
	}
	const Word **ipBackup=ip;
		const Word *tmpThread[2]={inTV.wordPtr,NULL};
		ip=tmpThread;
		bool ret=InnerInterpreter(*this);
	ip=ipBackup;
	return ret;
}

PP_API bool Context::Exec(const std::string inWordName) {
	auto iter=Dict.find(inWordName);
	if(iter==Dict.end()) { return Error(E_CAN_NOT_FIND_THE_WORD,inWordName); }

	const Word **ipBackup=ip;
		const Word *tmpThread[2]={iter->second,NULL};
		ip=tmpThread;
		bool ret=InnerInterpreter(*this);
	ip=ipBackup;
	return ret;
}

PP_API bool Context::Compile(const std::string& inWordName) {
	const Word *wordPtr=GetWordPtr(inWordName);
	if(wordPtr==NULL) { return false; }
	Compile(TypedValue(wordPtr));
	return true;
}

PP_API void Context::Compile(const TypedValue& inTypedValue) {
	newWord->tmpParam->push_back(inTypedValue);
}

PP_API bool Context::Compile(int inAddress,const TypedValue& inTypedValue) {
	if(newWord->tmpParam->size()<=inAddress) {
		fprintf(stderr,"SYSTEM ERROR at Compile(addr,tv).\n");
		return false;
	}
	newWord->tmpParam->at(inAddress)=inTypedValue;
	return true;
}

PP_API bool Context::BeginControlBlock(ControlBlockType inControlBlockType) {
	SS.emplace_back(kTypeMiscInt,inControlBlockType);
	RS.emplace_back(kTypeThreshold,ExecutionThreshold);
	if(ExecutionThreshold==kInterpretLevel) {
		if(newWord!=NULL) { return false; }
		newWord=new Word(Docol);
		SetCompileMode();
	}
	return true;
}

PP_API bool Context::EndControlBlock() {
	if(SS.size()<1) { return Error(E_SS_BROKEN); }
	Pop(SS);
	if(RS.size()<1) { return Error(E_RS_BROKEN); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=kTypeThreshold) {
		return Error_InvalidType(E_RS_TOS_THRESHOLD,tvThreshold);
	}
	if(tvThreshold.intValue==kInterpretLevel) {
		Compile(std::string("_semis"));
		FinishNewWord();
		TypedValue elementToExec(newWord);
		newWord=NULL;
		ExecutionThreshold=tvThreshold.intValue;
		return Exec(elementToExec);
	} else {
		return true;
	}
}

PP_API void Context::BeginNoNameWordBlock() {
	RS.emplace_back(kTypeNewWord,newWord);
	newWord=new Word(Docol);
	RS.emplace_back(kTypeThreshold,ExecutionThreshold);
	SetCompileMode();
}

PP_API bool Context::EndNoNameWordBlock() {
	if(RS.size()<2) { return Error(E_RS_AT_LEAST_2); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=kTypeThreshold) {
		return Error_InvalidType(E_RS_TOS_THRESHOLD,tvThreshold);
	}
	TypedValue newWordElement=Pop(RS);
	if(newWordElement.dataType!=kTypeNewWord) {
		return Error_InvalidType(E_RS_SECOND_WP,newWordElement);
	}
	
	newWord=(Word *)newWordElement.wordPtr;
	ExecutionThreshold=tvThreshold.intValue;
	if(ExecutionThreshold==kInterpretLevel && newWord!=NULL) {
		Error(E_SYSTEM_ERROR);
		exit(-1);
	}
	return true;
}

PP_API void Context::BeginListBlock() {
	RS.emplace_back(kTypeNewWord,newWord);
	newWord=new Word(WordType::List);
	RS.emplace_back(kTypeThreshold,ExecutionThreshold);
	SetSymbolMode();
}

PP_API bool Context::EndListBlock() {
	if(RS.size()<2) { return Error(E_RS_AT_LEAST_2); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=kTypeThreshold) {
		return Error_InvalidType(E_RS_TOS_THRESHOLD,tvThreshold);
	}

	TypedValue newWordElement=Pop(RS);
	if(newWordElement.dataType!=kTypeNewWord) {
		return Error_InvalidType(E_RS_SECOND_WP,newWordElement);
	}

	newWord=(Word *)newWordElement.wordPtr;
	ExecutionThreshold=tvThreshold.intValue;
	return true;
}

int Context::ReadThresholdBackup() {
	if(RS.size()<1) {
		Error(E_RS_IS_EMPTY);
		return -1;
	}
	TypedValue& tvThreshold=ReadTOS(RS);
	if(tvThreshold.dataType!=kTypeThreshold) {
		Error_InvalidType(E_RS_TOS_THRESHOLD,tvThreshold);
		return -1;
	}
	return tvThreshold.intValue;
}

PP_API void Context::MarkHere() {
	RS.emplace_back(kTypeAddress,GetNextThreadAddressOnCompile());
}

PP_API int Context::GetMarkPosition() {
	if(RS.size()<1) {
		Error(E_RS_IS_EMPTY);
		return -1;
	}
	TypedValue tvMark=Pop(RS);
	if(tvMark.dataType!=kTypeAddress) {
		Error_InvalidType(E_TOS_ADDRESS,tvMark);
		return -1;
	}
	return tvMark.intValue;
}

PP_API void Context::CompileEmptySlot(int inThresholdBackup) {
	TypedValue tv=TypedValue(kTypeEmptySlot,inThresholdBackup);
	Compile(tv);
}

PP_API void Context::MarkAndCreateEmptySlot() {
	MarkHere();
	CompileEmptySlot(ExecutionThreshold);
}

PP_API int Context::GetEmptySlotPosition() {
	int ret=GetMarkPosition();
	if(ret<0) { return ret; }
	TypedValue& emptySlot=newWord->tmpParam->at(ret);
	if(emptySlot.dataType!=kTypeEmptySlot) {
		Error(E_THREAD_MAY_BE_BROKEN);
		return -1;
	}
	return ret;
}

PP_API int Context::GetNextThreadAddressOnCompile() const {
	return (int)(newWord->tmpParam->size());
}


PP_API TypedValue Context::GetList() const {
	assert(newWord->type==WordType::List);
	TypedValue tvList(new List());
	size_t n=newWord->tmpParam->size();
	for(size_t i=0; i<n; i++) {
		tvList.listPtr->push_back(newWord->tmpParam->at(i));
	}
	return tvList;
}

PP_API void Context::EnterLeavableLoop() const {
	std::vector<int> leaveMark;
	gStackForLeave.push_back(leaveMark);
}

PP_API void Context::ExitLeavableLoop() {
	int jumpHere=GetNextThreadAddressOnCompile();
	std::vector<int>& leaveVec=gStackForLeave.back();
	TypedValue tvJumpAddress(kTypeAddress,jumpHere);
	const size_t n=leaveVec.size();
	for(size_t i=0; i<n; i++) {
		int q=leaveVec[i];
		newWord->tmpParam->at(q)=tvJumpAddress;
	}
	gStackForLeave.pop_back();
}

PP_API void Context::Compile_Leave() {
	Compile(std::string("_absolute-jump"));
	int p=GetNextThreadAddressOnCompile();
	gStackForLeave.back().push_back(p);
	CompileEmptySlot(ExecutionThreshold);
}

PP_API void Context::EnterSwitchBlock() const {
	std::vector<int> breakMark;
	gStackForBreak.push_back(breakMark);
}

PP_API void Context::ExitSwitchBlock() {
	int jumpHere=GetNextThreadAddressOnCompile();
	std::vector<int>& breakVec=gStackForBreak.back();
	TypedValue tvJumpAddress(kTypeAddress,jumpHere);
	const size_t n=breakVec.size();
	for(size_t i=0; i<n; i++) {
		int q=breakVec[i];
		newWord->tmpParam->at(q)=tvJumpAddress;
	}
	gStackForBreak.pop_back();
}

PP_API void Context::Compile_Break() {
	Compile(std::string("_absolute-jump"));
	int p=GetNextThreadAddressOnCompile();
	gStackForBreak.back().push_back(p);
	CompileEmptySlot(ExecutionThreshold);
}

const char *Context::GetExecutingWordName() const {
	if(ip==NULL) { return ""; }
	const char *wordName=(*ip)->longName.c_str();
	return wordName;
}

PP_API bool Context::Error(const NoParamErrorID inErrorID) {
	return ::Error(*this,inErrorID);
}

PP_API bool Context::Error(const ErrorID_withInt inErrorID,const int inIntValue) {
	return ::Error(*this,inErrorID,inIntValue);
}

PP_API bool Context::Error(const ErrorID_with2int inErrorID,
					const int inIntValue1,const int inIntValue2) {
	return ::Error(*this,inErrorID,inIntValue1,inIntValue2);
}

PP_API bool Context::Error(const ErrorID_withString inErrorID,const std::string& inStr) {
	return ::Error(*this,inErrorID,inStr.c_str());
}

PP_API bool Context::Error_InvalidType(const InvalidTypeErrorID inErrorID,
								const TypedValue& inTV) {
	return ::ErrorWithStr(*this,inErrorID,inTV.GetTypeStr());
}

PP_API bool Context::Error_InvalidType(const InvalidTypeTosSecondErrorID inErrorID,
								const TypedValue& inTos,const TypedValue& inSecond) {
	return ::Error(*this,inErrorID,inTos,inSecond);
}

PP_API bool Context::Error_InvalidType(const InvalidTypeStrTvTvErrorID inErrorID,
								const std::string& inStr,
								const TypedValue inTV1,const TypedValue& inTV2) {
	return ::Error(*this,inErrorID,inStr,inTV1,inTV2);
}

// tmpParam -> param
static int getParamSize(const std::vector<TypedValue> *inTmpParam);
PP_API void Context::FinishNewWord() {
	Optimize(newWord->tmpParam);

	int paramSize=getParamSize(newWord->tmpParam);

	const size_t n=newWord->tmpParam->size();
	int *addressTransferMap=new int[paramSize];
	size_t newIndex=0;
	for(int i=0; i<n; i++) {
		addressTransferMap[i]=(int)newIndex;
		TypedValue tv=newWord->tmpParam->at(i);
		switch(tv.dataType) {
			case kTypeAddress:
			case kTypeDirectWord: newIndex++; break;

			default:
				newIndex += sizeof(TypedValue)%sizeof(WordFunc*)==0
					 	  ? sizeof(TypedValue)/sizeof(WordFunc*)
					 	  : sizeof(TypedValue)/sizeof(WordFunc*)+1;
		}
	}

	const Word **wordPtrArray=new const Word*[paramSize];
	int dest=0;
	for(int i=0; i<n; i++) {
		TypedValue& tv=newWord->tmpParam->at(i);
		if(tv.dataType==kTypeDirectWord) {
			wordPtrArray[dest]=tv.wordPtr;
			dest++;
		} else if(tv.dataType==kTypeAddress) {
			wordPtrArray[dest]
				=(const Word*)(wordPtrArray+addressTransferMap[tv.intValue]);
			dest++;
		} else {
			new((TypedValue*)(wordPtrArray+dest)) TypedValue(tv);
			dest += TvSize;
 		}
	}
	newWord->param=wordPtrArray;
	newWord->isForgetable=true;
	lastDefinedWord=newWord;
}

static int getParamSize(const std::vector<TypedValue> *inTmpParam) {
	const size_t n=inTmpParam->size();
	size_t ret=0;
	for(size_t i=0; i<n; i++) {
		TypedValue tv=inTmpParam->at(i);
		switch(tv.dataType) {
			case kTypeAddress:
			case kTypeDirectWord:	ret++;	break;
			default: ret += TvSize;
		}
	}
	return (int)ret;
}

PP_API bool Context::IsInComment() {
	if(RS.size()>0) {
		TypedValue& rsTos=ReadTOS(RS);
		return rsTos.dataType==kTypeMiscInt
		  && (rsTos.intValue==kOPEN_C_STYLE_COMMENT
			  || rsTos.intValue==kOPEN_CPP_STYLE_ONE_LINE_COMMENT);
	}
	return false;
}

void Error(const char *inWordName,const char *inFormat,...) {
	if(inWordName!=NULL) {
		fprintf(stderr,"ERROR '%s': ",inWordName);
	}
	va_list vl;
	va_start(vl,inFormat);
	vfprintf(stderr,inFormat,vl);
	va_end(vl);
}

void Error(const Context& inContext,const char *inWordName,const char *inFormat,...) {
	if( inContext.suppressError ) {
		return;
	}
	if(inWordName!=NULL) {
		fprintf(stderr,"ERROR '%s': ",inWordName);
	}
	va_list vl;
	va_start(vl,inFormat);
	vfprintf(stderr,inFormat,vl);
	va_end(vl);
}

