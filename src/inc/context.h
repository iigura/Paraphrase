#pragma once

#include <vector>
#include <unordered_map>
#include <utility>

#include "thread.h"
#include "typedValue.h"
#include "outer.h"
#include "chan.h"
#include "word.h"
#include "inner.h"
#include "stack.h"

enum class Level {
	Interpret	=0,
	Compile 	=1,
	Symbol		=2,
};

enum class DebugCommand {
	None,
	SystemError,
	Invalid,
	Continue,
	StepOver,
	StepIn,
	Quit,
};

typedef std::vector<std::pair<TypedValue,bool /* isUnprotected */>> LocalVarSlot;
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
	Word *lastDefinedWord=NULL;

	TypedValue initParamForPBlock;
	bool isInitParamBroadcast;

	std::shared_ptr<ChanMan> toPipe;	// >pipe
	std::shared_ptr<ChanMan> fromPipe;	// pipe>

	std::shared_ptr<Thread> thread;
	volatile bool running=true;
	Mutex mutexForJoined;
	volatile bool joined=false;
	int threadIdx;

	bool suppressError=false;
	int LineNo=-1;

	Attr *attr=NULL;
	
	std::string line="";
	std::string hereDocStr="";
	bool appendNewlineForHereDocStr=true;

	bool nowDebugging=false;
	DebugCommand debugCommand=DebugCommand::None;

	PP_API Context(Context *inParent,Level inExecutionThreshold,
				   const std::vector<TypedValue> *inInitParamFromParent=NULL);

	PP_API ~Context();

	PP_API void New_ToPipe();

	PP_API Context *NewContextForCoroutine();

	PP_API bool Exec(const TypedValue& inTvWordPtr);
	PP_API bool Exec(const std::string inWordName);
	PP_API bool Exec(const Word *inWord);

	const Word *GetCurrentWord() { return *ip; }

	void SetInterpretMode() {
		newWord=NULL;
		ExecutionThreshold=Level::Interpret;
	}
	void SetCompileMode() { ExecutionThreshold=Level::Compile; }
	void SetSymbolMode() { ExecutionThreshold=Level::Symbol; }

	bool IsInterpretMode()	const { return ExecutionThreshold==Level::Interpret; }
	bool IsSymbolMode() 	const { return ExecutionThreshold==Level::Symbol;    }
	bool CheckCompileMode() const { return ExecutionThreshold!=Level::Interpret; }

	void PushThreshold() { DS.emplace_back(ExecutionThreshold); }
	void PushNewWord() { DS.emplace_back(DataType::NewWord,newWord); }

	PP_API void FinishWordDef() {
		lastDefinedWord=newWord;
		newWord=NULL;
	}

	PP_API void RemoveDefiningWord();

	PP_API bool BeginControlBlock();
	PP_API bool EndControlBlock();

	PP_API void BeginNoNameWordBlock(ControlBlockType inCBT);
	PP_API bool EndNoNameWordBlock();

	PP_API int ReadThresholdBackup();

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

	PP_API bool IsListConstructing() const;
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

	PP_API const char *GetExecutingWordName() const;

	PP_API TypedValue GetLiteralFromThreadedCode(bool inIsRemoveFromThread=true,
												 bool inPrintErrorMessage=true);

	inline bool SetCurrentLocalVar(int inSlotIndex,TypedValue& inValue) {
		assert(Env.size()>0 && Env.back().size()>inSlotIndex);
		LocalVarSlot& localVarSlot=Env.back();
		// needs full clone for list or array.
		//localVarSlot[inSlotIndex]=FullClone(inValue);
		localVarSlot[inSlotIndex].first=inValue;
		return true;
	}

// TODO:
//	inline bool SetLocalVarValueByDynamic

	inline bool GetLocalVarValue(int inSlotIndex,Stack& ioStack) {
		assert(Env.size()>0 && Env.back().size()>inSlotIndex);
		LocalVarSlot& localVarSlot=Env.back();
		ioStack.emplace_back(localVarSlot[inSlotIndex].first);
		return true;
	}

	inline TypedValue GetLocalVarValue(int inSlotIndex) {
		assert(Env.size()>0 && Env.back().size()>inSlotIndex);
		if(Env.size()<1) { return TypedValue(); }
		LocalVarSlot& localVarSlot=Env.back();
		return localVarSlot[inSlotIndex].first;
	}

	PP_API TypedValue GetLocalVarValueByDynamic(std::string& inVarName,bool *outFound);
	inline bool GetLocalVarValueByDynamic(std::string& inVarName,Stack& ioStack) {
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
				int slotPos=word->GetLocalVarSlotPos(inVarName);
				if(slotPos>=0) {
					ioStack.emplace_back(Env[t][slotPos].first);
					return true;
				}
				if(word->numOfLocalVar==0) { continue; }
				t--;
			}
		}

searchParentContext:
		if(parent!=NULL) {
			return parent->GetLocalVarValueByDynamic(inVarName,ioStack);
		} else {
			printf("ERROR: no such local var (var name:%s).\n",inVarName.c_str());
			ioStack.emplace_back(TypedValue());
			return false;
		}
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

PP_API Context *NewContextForCoroutine(Context& inContext);

