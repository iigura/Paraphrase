#pragma once

#include <vector>

#include "thread.h"
#include "typedValue.h"
#include "outer.h"
#include "chan.h"
#include "word.h"

enum {
	kInterpretLevel	=0,
	kCompileLevel 	=1,
	kSymbolLevel	=2,
};

struct Context {
	const Word **ip=NULL;

	Context *parent;

	Stack DS,RS,SS,ES;	// SS = Syntax Stack
	std::vector<const Word**> IS;	// IP Stack

	int ExecutionThreshold;

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

	PP_API Context(Context *inParent,int inExecutionThreshold,
			std::shared_ptr<ChanMan> inFromParent,
			std::shared_ptr<ChanMan> inToParent,
			const std::vector<TypedValue> *inInitParamFromParent=NULL);

	PP_API bool Exec(const TypedValue& inTypedValue);
	PP_API bool Exec(const std::string inWordName);
	PP_API bool Compile(const std::string& inWordName);
	PP_API void Compile(const TypedValue& inTypedValue);
	PP_API bool Compile(int inAddress,const TypedValue& inTypedValue);

	void SetInterpretMode() {
		newWord=NULL;
		ExecutionThreshold=kInterpretLevel;
	}
	void SetCompileMode() { ExecutionThreshold=kCompileLevel; }
	void SetSymbolMode() { ExecutionThreshold=kSymbolLevel; }

	bool IsInterpretMode() const { return ExecutionThreshold==kInterpretLevel; }
	bool CheckCompileMode() const { return ExecutionThreshold!=kInterpretLevel; }

	void PushThreshold() {
		DS.emplace_back(kTypeThreshold,ExecutionThreshold);
	}

	void PushNewWord() {
		DS.emplace_back(kTypeNewWord,newWord);
	}

	PP_API void FinishNewWord();

	PP_API void RemoveDefiningWord();

	PP_API bool BeginControlBlock(ControlBlockType inControlBlockType);
	PP_API bool EndControlBlock();

	PP_API void BeginNoNameWordBlock();
	PP_API bool EndNoNameWordBlock();

	int ReadThresholdBackup();

	PP_API void BeginListBlock();
	PP_API bool EndListBlock();

	PP_API void MarkHere();
	PP_API int GetMarkPosition();

	PP_API void CompileEmptySlot(int inThresholdBackup);
	PP_API void MarkAndCreateEmptySlot();
	PP_API int GetEmptySlotPosition();

	PP_API int GetNextThreadAddressOnCompile() const;

	PP_API TypedValue GetList() const;

	PP_API void EnterLeavableLoop() const;
	PP_API void ExitLeavableLoop();
	PP_API void Compile_Leave();

	PP_API void EnterSwitchBlock() const;
	PP_API void ExitSwitchBlock();
	PP_API void Compile_Break();

	PP_API bool IsInComment();

	const char *GetExecutingWordName() const;

	PP_API bool Error(const NoParamErrorID inErrorID);
	PP_API bool Error(const ErrorID_withInt inErrorID,const int inIntValue);
	PP_API bool Error(const ErrorID_with2int inErrorID,
			   const int inIntValue1,const int inIntValue2);
	PP_API bool Error(const ErrorID_withString inErrorID,const std::string& inStr);
	PP_API bool Error_InvalidType(const InvalidTypeErrorID inErrorID,
								  const TypedValue& inTV);
	PP_API bool Error_InvalidType(const InvalidTypeTosSecondErrorID inErrorID,
						   const TypedValue& inTos,const TypedValue& inSecond);
	PP_API bool Error_InvalidType(const InvalidTypeStrTvTvErrorID inErrorID,
								  const std::string& inStr,
								  const TypedValue inTV1,const TypedValue& inTV2);
};
