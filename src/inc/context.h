#pragma once

#include <vector>
#include <unordered_map>

#include "thread.h"
#include "typedValue.h"
#include "outer.h"
#include "chan.h"
#include "word.h"
#include "inner.h"

enum class Level {
	kInterpret	=0,
	kCompile 	=1,
	kSymbol		=2,
};

typedef std::vector<TypedValue> LocalVarSlot;
typedef std::vector<LocalVarSlot> EnvVector;
typedef WordFunc InnerInterpretFunc;

struct Context {
	const Word **ip=NULL;

	Context *parent;
	InnerInterpretFunc innerInterpreter=InnerInterpreter;

	Stack DS,RS,SS;					// SS = Syntax Stack
	std::vector<const Word**> IS;	// IP Stack
	EnvVector Env;

	Level ExecutionThreshold;

	Word *newWord=NULL;
	Word *lastDefinedWord=NULL;;

	TypedValue initParamForPBlock;
	bool isInitParamBroadcast;

	std::shared_ptr<ChanMan> fromParent;	// parent>
	std::shared_ptr<ChanMan> toParent;		// >parent

	std::shared_ptr<ChanMan> toChild;	// >child
	std::shared_ptr<ChanMan> fromChild;	// child>

	std::shared_ptr<ChanMan> toPipe;	// >pipe
	std::shared_ptr<ChanMan> fromPipe;	// pipe>

	std::shared_ptr<ChanMan> lastOutputPipe;

	Thread thread;

	bool suppressError=false;

	Attr *attr=NULL;
	
	std::string line="";
	std::string hereDocStr="";
	bool appendNewlineForHereDocStr=true;

	PP_API Context(Context *inParent,Level inExecutionThreshold,
			std::shared_ptr<ChanMan> inFromParent,
			std::shared_ptr<ChanMan> inToParent,
			const std::vector<TypedValue> *inInitParamFromParent=NULL);

	PP_API bool Exec(const TypedValue& inTypedValue);
	PP_API bool Exec(const std::string inWordName);
	PP_API bool Exec(const Word *inWord);
	PP_API bool Compile(const std::string& inWordName);
	PP_API void Compile(const TypedValue& inTypedValue);
	PP_API bool Compile(int inAddress,
						const TypedValue& inTypedValue,
						bool inIsForceUpdate=false);

	const Word *GetCurrentWord() {
		return *ip;
	}

	void SetInterpretMode() {
		newWord=NULL;
		ExecutionThreshold=Level::kInterpret;
	}
	void SetCompileMode() { ExecutionThreshold=Level::kCompile; }
	void SetSymbolMode() { ExecutionThreshold=Level::kSymbol; }

	bool IsInterpretMode()	const { return ExecutionThreshold==Level::kInterpret; }
	bool IsSymbolMode() 	const { return ExecutionThreshold==Level::kSymbol;    }
	bool CheckCompileMode() const { return ExecutionThreshold!=Level::kInterpret; }

	void PushThreshold() { DS.emplace_back(ExecutionThreshold); }

	void PushNewWord() {
		DS.emplace_back(DataType::kTypeNewWord,newWord);
	}

	PP_API void FinishNewWord();

	PP_API void RemoveDefiningWord();

	PP_API bool BeginControlBlock();
	PP_API bool EndControlBlock();

	PP_API void BeginNoNameWordBlock();
	PP_API bool EndNoNameWordBlock();

	int ReadThresholdBackup();

	PP_API void BeginListBlock();
	PP_API bool EndListBlock();

	PP_API TypedValue MarkHere(int inOffset=0);
	PP_API int GetChunkIndex(const int inCbtBitMask);
	PP_API bool CheckTopChunk(const int inCbtBitMask);
	PP_API bool RemoveTopChunk();

	PP_API void CompileEmptySlot(const Level inThresholdBackup);
	PP_API TypedValue MarkAndCreateEmptySlot();
	// PP_API int GetEmptySlotPosition(const int inCbtBitMask);

	PP_API int GetNextThreadAddressOnCompile() const;

	PP_API TypedValue GetList() const;

	//PP_API void EnterLeavableLoop() const;
	//PP_API void ExitLeavableLoop();
	//PP_API void Compile_Leave();

	PP_API void EnterSwitchBlock() const;
	PP_API void ExitSwitchBlock();
	PP_API void Compile_Break();

	PP_API bool IsInComment();
	PP_API bool IsInCStyleComment();
	PP_API bool IsInCppStyleComment();

	PP_API void EnterLevel2(const ControlBlockType inCB_ID);
	PP_API bool LeaveLevel2();

	PP_API bool EnterHereDocument(const ControlBlockType inCB_ID);
	PP_API bool LeaveHereDocument();

	const char *GetExecutingWordName() const;

	inline bool SetCurrentLocalVar(int inSlotIndex,TypedValue& inValue) {
		assert(Env.size()>0);
		LocalVarSlot& localVarSlot=Env.back();
		if(localVarSlot.size()<=inSlotIndex) { return false; /* inalid index */ }
		// needs full clone for list or array.
		localVarSlot[inSlotIndex]=FullClone(inValue);
		return true;
	}

	inline bool GetLocalVarValue(int inSlotIndex,Stack& ioStack) {
		assert(Env.back().size()>inSlotIndex);
		if(Env.size()<1) { return false; }
		LocalVarSlot& localVarSlot=Env.back();
		ioStack.emplace_back(localVarSlot[inSlotIndex]);
		return true;
	}

	inline TypedValue GetLocalVarValue(int inSlotIndex) {
		if(Env.size()<1) { return TypedValue(); }
		LocalVarSlot& localVarSlot=Env.back();
		//if(localVarSlot.size()<=inSlotIndex) { return TypedValue(); }
		return localVarSlot[inSlotIndex];
	}

	inline TypedValue GetLocalVarValue(std::string inVarName) {
		if(Env.size()<1) {
			printf("no local vars.\n");
			return TypedValue();
		}
		Word *word=(Word*)*IS.back();
		if(word==NULL) {
			Error(NoParamErrorID::E_SYSTEM_ERROR);
			exit(-1);
		}
		if(word->localVarDict.find(inVarName)==word->localVarDict.end()) {
			Error(ErrorIdWithString::E_NO_SUCH_LOCAL_VAR,inVarName);
			return TypedValue();	// invalid value.
		}
		LocalVarSlot& localVarSlot=Env.back();
		return localVarSlot[word->localVarDict[inVarName]];
	}

	inline bool GetLocalVarValueByDynamic(std::string& inVarName,Stack& ioStack) {
		if(Env.size()<1) { return false; }
		int start;
		for(start=(int)IS.size()-1; (*IS[start])->numOfLocalVar==0; start--) {
			// empty
		}
		if(start<0) { return false; }
		int offset = (*ip)->numOfLocalVar==0 ? 0 : 1;
		for(int w=start,t=(int)Env.size()-1-offset; w>=0; w--) {
			Word *word=(Word *)(*IS[w]);
			int slotPos=word->GetLocalVarSlotPos(inVarName);
			if(slotPos>=0) {
				ioStack.emplace_back(Env[t][slotPos]);
				return true;
			}
			if(word->numOfLocalVar==0) { continue; }
			t--;
		}
		return false;
	}

	PP_API bool Error(const NoParamErrorID inErrorID);
	PP_API bool Error(const ErrorIdWithInt inErrorID,const int inIntValue);
	PP_API bool Error(const ErrorIdWith2int inErrorID,
			   		  const int inIntValue1,const int inIntValue2);
	PP_API bool Error(const ErrorIdWithString inErrorID,
					  const std::string& inStr);
	PP_API bool Error(const InvalidTypeErrorID inErrorID, const TypedValue& inTV);
	PP_API bool Error(const InvalidValueErrorID inErrorID,const TypedValue& inTV);
	PP_API bool Error(const InvalidTypeTosSecondErrorID inErrorID,
					  const TypedValue& inTos,const TypedValue& inSecond);
	PP_API bool Error_InvalidType(const InvalidTypeStrTvTvErrorID inErrorID,
								  const std::string& inStr,
								  const TypedValue inTV1,const TypedValue& inTV2);
	PP_API void ShowIS() const;
};

