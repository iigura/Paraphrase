#include <stdarg.h>

#include "word.h"
#include "stack.h"
#include "context.h"
#include "externals.h"
#include "optimizer.h"

const int DS_InitialStackSize=4096;
const int RS_InitialStackSize=1024;
const int SS_InitialStackSize=1024;
const int IS_InitialStackSize=1024;
const int ES_InitialStackSize=1024;

static std::vector< std::vector<int> > gStackForLeave;
static std::vector< std::vector<int> > gStackForBreak;

bool Error(Context& inContext,const NoParamErrorID inErrorID);
bool Error(Context& inContext,
		   const InvalidTypeErrorID,const std::string& inStr);
bool Error(Context& inContext,
		   const InvalidValueErrorID,const std::string& inStr);
bool Error(Context& inContext,const InvalidTypeTosSecondErrorID inErrorID,
		   const TypedValue& inTos,const TypedValue& inSecond);
bool Error(Context& inContext,const InvalidTypeStrTvTvErrorID inErrorID,
		   const std::string& inStr,const TypedValue inTV1,const TypedValue& inTV2);
bool Error(Context& inContext,const ErrorIdWithInt inErrorID,
		   const int inIntValue);
bool Error(Context& inContext,const ErrorIdWithString inErrorID,
		   const char *inStr);
bool Error(Context& inContext,
		   const ErrorIdWithString inErrorID,const char *inStr);
bool Error(Context& inContext,const ErrorIdWith2int inErrorID,
		   const int inIntValue1,const int inIntValue2);

PP_API Context *GlobalContext=new Context(NULL,Level::Interpret);

PP_API Context::Context(Context *inParent,Level inExecutionThreshold,
				 const std::vector<TypedValue> *inInitParamFromParent)
  :parent(inParent),ExecutionThreshold(inExecutionThreshold),threadIdx(-1),
   toPipe(NULL),fromPipe(NULL),
   initParamForPBlock(),isInitParamBroadcast(true),newWord(NULL) {
	DS.reserve(DS_InitialStackSize);
	RS.reserve(RS_InitialStackSize);
	SS.reserve(SS_InitialStackSize);
	IS.reserve(IS_InitialStackSize);

	if(inInitParamFromParent!=NULL) { BackInsert(&DS,inInitParamFromParent); }
}

PP_API Context::~Context() {
	// empty
	// printf("~CONTEXT: %p\n",this);
	// fflush(stdout);
}

PP_API void Context::New_ToPipe() {
	if(toPipe.get()!=nullptr) {
		toPipe->CloseOnWrite();
		toPipe.reset();
	}
	toPipe.reset(new ChanMan(1,">pipe"));
}

PP_API Context *Context::NewContextForCoroutine() {
	//Context *newContext=new Context(this,
	return NULL;


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
	const Word *wordPtr;
	if(inTV.HasWordPtr(&wordPtr)==false) {
		return Error(InvalidTypeErrorID::TosWp,inTV);
	}
	return Exec(wordPtr);
}

PP_API bool Context::Exec(const std::string inWordName) {
	auto iter=Dict.find(inWordName);
	if(iter==Dict.end()) {
		return Error(ErrorIdWithString::CanNotFindTheWord,inWordName);
	}
	return Exec(iter->second);
}

PP_API bool Context::Exec(const Word *inWord) {
	const Word **ipBackup=ip;
		const Word *tmpThread[2]={inWord,NULL};
		ip=tmpThread;
		bool ret=innerInterpreter(*this);
		if(ret==false) { IS.clear(); }
	ip=ipBackup;
	return ret;
}

PP_API bool Context::BeginControlBlock() {
	RS.emplace_back(ExecutionThreshold);
	if(ExecutionThreshold==Level::Interpret) {
		if(newWord!=NULL) { return false; }
		newWord=new Word(G_DocolAdvice);
		SetCompileMode();
	}
	return true;
}

PP_API bool Context::EndControlBlock() {
	if(RS.size()<1) { return Error(NoParamErrorID::RsBroken); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=DataType::Threshold) {
		return Error(InvalidTypeErrorID::RsTosThreshold,tvThreshold);
	}
	if(tvThreshold.intValue==(int)Level::Interpret) {
		newWord->CompileWord("_semis");
		TypedValue elementToExec(newWord);
		newWord->BuildParam();
		FinishWordDef();
		ExecutionThreshold=(Level)tvThreshold.intValue;
		return Exec(elementToExec);
	} else {
		return true;
	}
}

PP_API void Context::BeginNoNameWordBlock(ControlBlockType inCBT) {
	assert(((int)inCBT & (int)ControlBlockType::kOPEN_LAMBDA_GROUP)!=0);

	SS.emplace_back(TypedValue(inCBT));
	RS.emplace_back(DataType::NewWord,newWord);
	newWord=new Word(G_DocolAdvice);
	RS.emplace_back(ExecutionThreshold);
	SetCompileMode();
}

PP_API bool Context::EndNoNameWordBlock() {
	if(SS.size()<1) { return Error(NoParamErrorID::SsBroken); }
	TypedValue tvLambdaInfo=Pop(SS);
	if(tvLambdaInfo.dataType!=DataType::CB) {
		return Error(NoParamErrorID::SsBroken);
	}
	if((tvLambdaInfo.intValue & (int)ControlBlockType::kOPEN_LAMBDA_GROUP)==0) {
		return Error(NoParamErrorID::SsBroken);
	}

	if(RS.size()<2) { return Error(NoParamErrorID::RsAtLeast2); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=DataType::Threshold) {
		return Error(InvalidTypeErrorID::RsTosThreshold,tvThreshold);
	}
	TypedValue newWordElement=Pop(RS);
	if(newWordElement.dataType!=DataType::NewWord) {
		return Error(InvalidTypeErrorID::RsSecondWp,newWordElement);
	}
	
	newWord=(Word *)newWordElement.wordPtr;
	ExecutionThreshold=(Level)tvThreshold.intValue;
	if(ExecutionThreshold==Level::Interpret && newWord!=NULL) {
		Error(NoParamErrorID::SystemError);
		exit(-1);
	}
	return true;
}

PP_API void Context::BeginListBlock() {
	RS.emplace_back(DataType::NewWord,newWord);
	newWord=new Word(WordType::List);
	RS.emplace_back(ExecutionThreshold);
	SetSymbolMode();
}

PP_API bool Context::EndListBlock() {
	if(RS.size()<2) { return Error(NoParamErrorID::RsAtLeast2); }
	TypedValue tvThreshold=Pop(RS);
	if(tvThreshold.dataType!=DataType::Threshold) {
		return Error(InvalidTypeErrorID::RsTosThreshold,tvThreshold);
	}

	TypedValue newWordElement=Pop(RS);
	if(newWordElement.dataType!=DataType::NewWord) {
		return Error(InvalidTypeErrorID::RsSecondWp,newWordElement);
	}

	newWord=(Word *)newWordElement.wordPtr;
	ExecutionThreshold=(Level)tvThreshold.intValue;
	return true;
}

TypedValue Context::GetLocalVarValueByDynamic(std::string& inVarName,bool *outFound) {
	if(Env.size()<1) { goto searchParentContext; }

	int start;
	for(start=(int)IS.size()-1; (*IS[start])->numOfLocalVar==0; start--) {
		// empty
	}
	if(start<0) { goto searchParentContext; }

	{
		int offset = (*ip)->numOfLocalVar==0 ? 0 : 1;
		for(int w=start,t=(int)Env.size()-1-offset; w>=0; w--) {
			Word *word=(Word *)(*IS[w]);
			if(word->numOfLocalVar==0) { continue; }
			int slotPos=word->GetLocalVarSlotPos(inVarName);
			if(slotPos>=0) {
				*outFound=true;
				return Env[t][slotPos].first;
			}
			t--;
		}
	}

searchParentContext:
	if(parent!=NULL) {
		return parent->GetLocalVarValueByDynamic(inVarName,outFound);
	} else {
		printf("ERROR: no such local vars (var name:%s).\n",inVarName.c_str());
		*outFound=false;
		return TypedValue();
	}
}

int Context::ReadThresholdBackup() {
	if(RS.size()<1) {
		Error(NoParamErrorID::RsIsEmpty);
		return -1;
	}
	TypedValue& tvThreshold=ReadTOS(RS);
	if(tvThreshold.dataType!=DataType::Threshold) {
		Error(InvalidTypeErrorID::RsTosThreshold,tvThreshold);
		return -1;
	}
	return tvThreshold.intValue;
}

PP_API TypedValue Context::MarkHere(int inOffset) {
	return TypedValue(DataType::Address,GetNextThreadAddressOnCompile()+inOffset);
}

PP_API int Context::GetChunkIndex(const int inCbtMask) {
	if(SS.size()<2) {
		Error(NoParamErrorID::SsBroken);
		return -1;
	}
	// chunk size contains CB info.
	// ex:
	//     +------------------+
	//     | kSyntax_IF       |
	//     +------------------+
	//     | 3 (==chunk size) |
	//     +------------------+
	//     | alpha            |
	//     +------------------+
	int chunkSize=0;
	for(int i=(int)SS.size()-1; i>0; i-=chunkSize) {
		const TypedValue& tvCB=SS.at(i);
		if(tvCB.dataType!=DataType::CB) {
			Error(NoParamErrorID::SsBroken);
			return -1;
		}
		if((tvCB.intValue & inCbtMask)!=0) {
			return i;
		}
		if(i-1<0) {
			Error(NoParamErrorID::SsBroken);
			return -1;
		}
		const TypedValue& tvChunkSize=SS.at(i-1);
		if(tvChunkSize.dataType!=DataType::Int) {
			Error(NoParamErrorID::SsBroken);
			return -1;
		}
		if(tvChunkSize.intValue<=1) {
			Error(NoParamErrorID::SsBroken);
			return -1;
		}
		chunkSize=tvChunkSize.intValue;
	}
	return -1;
}

PP_API bool Context::RemoveTopChunk() {
	TypedValue& tvCB=ReadTOS(SS);
	if(tvCB.dataType!=DataType::CB) { return Error(NoParamErrorID::SsBroken); }

	TypedValue& tvChunkSize=ReadSecond(SS);
	if(tvChunkSize.dataType!=DataType::Int) { return Error(NoParamErrorID::SsBroken); }

	int chunkSize=tvChunkSize.intValue;
	if(chunkSize<0) { return Error(NoParamErrorID::SsBroken); }
	if(SS.size()<chunkSize) { return Error(NoParamErrorID::SsBroken); }
	for(int i=0; i<chunkSize; i++) {
		SS.pop_back();
	}
	return true;
}

PP_API void Context::CompileEmptySlot(const Level inThresholdBackup) {
	TypedValue tv=TypedValue(DataType::EmptySlot,inThresholdBackup);
	newWord->CompileValue(tv);
}

PP_API TypedValue Context::MarkAndCreateEmptySlot() {
	TypedValue ret=MarkHere();
	CompileEmptySlot(ExecutionThreshold);
	return ret;
}

PP_API int Context::GetNextThreadAddressOnCompile() const {
	return (int)(newWord->tmpParam->size());
}

PP_API bool Context::IsListConstructing() const {
	return newWord->type==WordType::List;
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

PP_API const char *Context::GetExecutingWordName() const {
	if(ip==NULL) { return ""; }
	const char *wordName=(*ip)->longName.c_str();
	return wordName;
}

PP_API TypedValue Context::GetLiteralFromThreadedCode(bool inIsRemoveFromThread,
													  bool inPrintErrorMessage) {
	if(newWord==NULL) {
		Error(NoParamErrorID::ShouldBeExecutedInDefinition);
		return TypedValue();
	}
	if(newWord->tmpParam==NULL) {
		Error(NoParamErrorID::SystemError);
		exit(-1);
	}
	CodeThread *thread=newWord->tmpParam;
	int n=(int)thread->size();
	if(n<1) { goto onError; }

	int litIndex;
	bool multiple;
	if(n>2 && (thread->at(n-1).dataType==DataType::Word
			   || thread->at(n-1).dataType==DataType::DirectWord)
	  && thread->at(n-1).wordPtr->longName=="std:create-working-value") {
		litIndex=n-3;
		multiple=true;
		if(litIndex<0) { goto onError; }
	} else {
		litIndex=n-2;
		multiple=false;
		if(litIndex<0) { goto onError; }
	}

	if((thread->at(litIndex).dataType!=DataType::Word
		&& thread->at(litIndex).dataType!=DataType::DirectWord)
	  || thread->at(litIndex).wordPtr->longName!="std:_lit") {
		goto onError;
	}
	{
		TypedValue tv=thread->at(litIndex+1);
		if( inIsRemoveFromThread ) {
			thread->pop_back();
			thread->pop_back();
			if( multiple ) { thread->pop_back(); }
		}
		return tv;
	}

onError:
	if( inPrintErrorMessage ) { Error(NoParamErrorID::InvalidUse); }
	return TypedValue();
}

PP_API bool Context::Error(const NoParamErrorID inErrorID) {
	return ::Error(*this,inErrorID);
}

PP_API bool Context::Error(const ErrorIdWithInt inErrorID,
						   const int inIntValue) {
	return ::Error(*this,inErrorID,inIntValue);
}

PP_API bool Context::Error(const ErrorIdWith2int inErrorID,
						   const int inIntValue1,const int inIntValue2) {
	return ::Error(*this,inErrorID,inIntValue1,inIntValue2);
}

PP_API bool Context::Error(const ErrorIdWithString inErrorID,
						   const std::string& inStr) {
	return ::Error(*this,inErrorID,inStr.c_str());
}

PP_API bool Context::Error(const InvalidTypeErrorID inErrorID,
						   const TypedValue& inTV) {
	return ::Error(*this,inErrorID,inTV.GetTypeStr());
}

PP_API bool Context::Error(const InvalidValueErrorID inErrorID,
						   const TypedValue& inTV) {
	return ::Error(*this,inErrorID,inTV.GetValueString());
}

PP_API bool Context::Error(const InvalidTypeTosSecondErrorID inErrorID,
						   const TypedValue& inTos,const TypedValue& inSecond) {
	return ::Error(*this,inErrorID,inTos,inSecond);
}

PP_API bool Context::Error_InvalidType(const InvalidTypeStrTvTvErrorID inErrorID,
								const std::string& inStr,
								const TypedValue inTV1,const TypedValue& inTV2) {
	return ::Error(*this,inErrorID,inStr,inTV1,inTV2);
}

PP_API void Context::ShowIS() const {
	int maxLength=-1;
	for(int i=0; i<(int)IS.size(); i++) {
		const Word *word=*(IS[i]);
		maxLength=std::max(maxLength,(int)(word->longName.size()));
	}

	printf(" +");
	for(int i=0; i<maxLength; i++) { putchar('-'); }
	printf("+ \n");

	for(int i=(int)IS.size()-1; i>=0; i--) {
		printf(" |");
		const Word *word=*IS[i];
		const int len=(int)(word->longName.size());
		int gap=(maxLength-len)/2;
		for(int j=0; j<gap; j++) { putchar(' '); }
		printf("%s",word->longName.c_str());
		for(int j=0; j<maxLength-gap-len; j++) { putchar(' '); }
		printf("|\n");
	}

	for(int i=0; i<maxLength+2*2; i++) { putchar('-'); }
	printf("\n");
}

PP_API bool Context::IsInComment() {
	if(RS.size()>0) {
		TypedValue& rsTos=ReadTOS(RS);
		return rsTos.dataType==DataType::MiscInt
		  && (rsTos.intValue==(int)ControlBlockType::OpenCStyleComment
			  || rsTos.intValue==(int)ControlBlockType::OpenCppStyleOneLineComment);
	}
	return false;
}

PP_API bool Context::IsInCStyleComment() {
	if(RS.size()>0) {
		TypedValue& rsTos=ReadTOS(RS);
		if(rsTos.dataType==DataType::MiscInt
		   && rsTos.intValue==(int)ControlBlockType::OpenCStyleComment) {
			return true;
		}
	}
	return false;
}

PP_API bool Context::IsInCppStyleComment() {
	if(RS.size()>0) {
		TypedValue& rsTos=ReadTOS(RS);
		if(rsTos.dataType==DataType::MiscInt
		  && rsTos.intValue==(int)ControlBlockType::OpenCppStyleOneLineComment) {
			return true;
		}
	}
	return false;
}

PP_API void Context::EnterLevel2(const ControlBlockType inCB_ID) {
	PushThreshold();
	PushNewWord();
	RS.emplace_back(DataType::MiscInt,inCB_ID);
	newWord=new Word(WordType::Normal);
	ExecutionThreshold=Level::Symbol;
}

PP_API bool Context::LeaveLevel2() {
	delete newWord->tmpParam;
	delete newWord;

	if(DS.size()<2) { return Error(NoParamErrorID::DsAtLeast2); }
	TypedValue tvNW=Pop(DS);
	if(tvNW.dataType!=DataType::NewWord) {
		return Error(InvalidTypeErrorID::TosNewWord,tvNW);
	}
 	newWord=(Word *)tvNW.wordPtr;

	TypedValue tvThreshold=Pop(DS);
	if(tvThreshold.dataType!=DataType::Threshold) {
		return Error(InvalidTypeErrorID::SecondThreshold,tvThreshold);
	}
	ExecutionThreshold=(Level)tvThreshold.intValue;
	return true;
}

PP_API bool Context::EnterHereDocument(ControlBlockType inCB_ID) {
	if( IsInCStyleComment() )   { return false; }
	if( IsInCppStyleComment() ) { return false; }
	if(ExecutionThreshold==Level::Symbol) { return false; }
	EnterLevel2(inCB_ID);
	return true;
}

PP_API bool Context::LeaveHereDocument() {
	if(LeaveLevel2()==false) { return false; }
	switch(ExecutionThreshold) {
		case Level::Interpret:
			DS.emplace_back(hereDocStr);
			break;
		case Level::Compile:
			newWord->CompileWord("_lit");
			newWord->CompileValue(hereDocStr);
			break;
		default:
			fprintf(stderr,"SYSTEM ERROR, invalid execution threshold (in >>>raw)\n");
			exit(-1);
	}
	hereDocStr="";
	return true;
}

PP_API Context *NewContextForCoroutine(Context& inContext) {
	Context *newContext=new Context(inContext.parent,inContext.ExecutionThreshold);


return NULL;
}


void Error(const char *inWordName,const char *inFormat,...) {
	if(inWordName!=NULL) { fprintf(stderr,"ERROR '%s': ",inWordName); }
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

