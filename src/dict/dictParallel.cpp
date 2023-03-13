#include <assert.h>

#include <thread>

#include "externals.h"
#include "typedValue.h"
#include "threadMan.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "util.h"

static bool execBody(Context& inContext,int inNumOfParallels);
static bool closeParallelBlock(Context& inContext,int inNumOfParallels);
static bool execParallel(Context& inContext,
						 TypedValue& inTvExec,TypedValue& inTvLambdaInfo,
						 int inNumOfParallels=-1);
static void newChildThreads(std::vector<TypedValue> *outThreads,
							Context *inParent,
							TypedValue inTvExec,int inNumOfParallels);
static TypedValue runThreads(std::vector<TypedValue> &inThreads);
static void *kicker(void *inKickerArg);
static bool threadInfoUpdate(Context& inContext,ControlBlockType inAdditionalThreadInfo);
static bool joinMain(Context& inContext,const TypedValue& inTV);

// [ ... ]
// 	スレッド生成時に、入力用のパイプは作らず、出力用のパイプのみを作る。
//
// >[ ... ]
// 現在のコンテキストの >pipe 用チャネルをを新しく作るスレッドの
// pipe> 用のチャネルとして設定する。
// '[ parent get.>pipe ctx set.pipe> ... ]' と同値
//
// >>[ ... ]
// 	スレッド生成時に、TOS にあるチャネルを入力用パイプとして設定する。
// 	出力用パイプは新たに生成する。
//	注：">>" は TOS にあるチャネルを用いることを表す。
//
// [ ... ]>
// 現在のコンテキストの pipe> 用チャネルに、新しく作るスレッドで
// 作成された出力用チャネルを設定する（上書きする）。
// 使い方
// 	[ ... ]> while-pipe ... repeat
//
// [ ... ]=
// 生成したスレッドのコンテキストをスタックに積む。
// 注："=" は生成したスレッドのコンテキストをスレッドに積むことを意味する。
//
// [ ... ]=>
// 生成したスレッドのコンテキストをスタックに積み、
// 生成したスレッドの出力用チャネルを現在のコンテキストの
// 入力用チャネルとして設定する。
//
// [ ... ]>>
// 生成したスレッドの出力用チャネルをスタックに積む。
// 使い方
//  [ ...  ]>> >>[[ ... ]]
//
// [ ... ]=>>
// 生成したスレッドのコンテキストと出力用チャネルをスタックに積む。
// ワード ']=>>' は --- ctx chan 

void InitDict_Parallel() {
	Install(new Word("[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenSimpleThread);
		NEXT;
	}));

	Install(new Word(">[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenConnectedThread);
		NEXT;
	}));

	Install(new Word(">>[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenTosChanConnectThread);
		NEXT;
	}));
	
	// ctx ---
	// same as '>['
	Install(new Word(">[[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenConnectedThread);
		NEXT;
	}));

	// ctx ---
	// same as '=>['
	Install(new Word(">>[[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenTosChanConnectThread);
		NEXT;
	}));

	Install(new Word("[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenSimpleThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	Install(new Word(">[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenConnectedThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	Install(new Word(">>[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenTosChanConnectThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	// same as '['
	Install(new Word("[[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock(ControlBlockType::OpenSimpleThread);
		NEXT;
	}));

	Install(new Word("[[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenSimpleThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	Install(new Word(">[[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenConnectedThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	Install(new Word(">>[[->",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenTosChanConnectThread
		  | (int)ControlBlockType::kThreadMask_Open_TosBroadcast));
		NEXT;
	}));

	Install(new Word("[[=>",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenSimpleThread
		  | (int)ControlBlockType::kThreadMask_Open_PushTosListElem));
		NEXT;
	}));

	Install(new Word(">[[=>",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenConnectedThread
		  | (int)ControlBlockType::kThreadMask_Open_PushTosListElem));
		NEXT;
	}));

	Install(new Word(">>[[=>",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock((ControlBlockType)(
			(int)ControlBlockType::OpenTosChanConnectThread
		  | (int)ControlBlockType::kThreadMask_Open_PushTosListElem));
		NEXT;
	}));

	Install(new Word("]",WordLevel::Immediate,WORD_FUNC {
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]=",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushContext)
		  ==false) { return false; }
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)
		  ==false) { return false; }
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]=>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)
		   ==false) { return false; }
		threadInfoUpdate(inContext,ControlBlockType::kThreadMask_Close_PushContext);
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]>>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushOutputChan)
		  ==false) { return false; }
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]=>>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushOutputChan)
		  ==false) { return false; }
		threadInfoUpdate(inContext,ControlBlockType::kThreadMask_Close_PushContext);
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]",WordLevel::Immediate,WORD_FUNC {
		const int numOfCores = inContext.isInitParamBroadcast
							 ? G_NumOfCores
							 : (int)inContext.initParamForPBlock.listPtr->size();
		if(closeParallelBlock(inContext,numOfCores)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]=",WordLevel::Immediate,WORD_FUNC {
		const int numOfCores = inContext.isInitParamBroadcast
							 ? G_NumOfCores
							 : (int)inContext.initParamForPBlock.listPtr->size();
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushContext)
		  ==false) { return false; }
		if(closeParallelBlock(inContext,numOfCores)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)
		  ==false) {
			return false;
		}
		const int numOfCores = inContext.isInitParamBroadcast
							 ? G_NumOfCores
							 : (int)inContext.initParamForPBlock.listPtr->size();
		if(closeParallelBlock(inContext,numOfCores)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]=>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)
		  ==false) {
			return false;
		}
		threadInfoUpdate(inContext,ControlBlockType::kThreadMask_Close_PushContext);
		const int numOfCores = inContext.isInitParamBroadcast
							 ? G_NumOfCores
							 : (int)inContext.initParamForPBlock.listPtr->size();
		if(closeParallelBlock(inContext,numOfCores)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]>>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushOutputChan)
		  ==false) { return false; }
		if(closeParallelBlock(inContext,-1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("]]=>>",WordLevel::Immediate,WORD_FUNC {
		if(threadInfoUpdate(inContext,
							ControlBlockType::kThreadMask_Close_PushOutputChan)
		  ==false) { return false; }
		threadInfoUpdate(inContext,ControlBlockType::kThreadMask_Close_PushContext);
		if(closeParallelBlock(inContext,-1)==false) { return false; }
		NEXT;
	}));

	// ctx --- chan
	Install(new Word("get-output-chan",WORD_FUNC {
		// TODO		

		NEXT;
	}));

	// single thread ( for [ ... ]= ).
	// 	ctx ---
	// 		or
	// multi thread ( for [[ ... ]]= ).
	// ( ctx1 ctx2 ... ctxN ) ---
	Install(new Word("join",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(joinMain(inContext,tos)==false) { return false; }
		NEXT;
	}));

	Install(new Word("@join",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(joinMain(inContext,tos)==false) { return false; }
		NEXT;
	}));

	// tvCB(=lambdaInfo) tvExec ---
	Install(new Word("_exec/single",WORD_FUNC {
		if(execBody(inContext,1)==false) { return false; }
		NEXT;
	}));	

	Install(new Word("_exec/parallel",WORD_FUNC {
		if(execBody(inContext,-1)==false) { return false; }
		NEXT;
	}));	

	Install(new Word(">pipe",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		if(inContext.toPipe.get()==nullptr) {
			inContext.toPipe.reset(new ChanMan(1,">pipe"));
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType==DataType::EoC) {
			inContext.toPipe->CloseOnWrite();
		} else {
			if(inContext.toPipe->Send(tos)==false) {
				return inContext.Error(NoParamErrorID::ChanIsAlreadyClosed);
			}
		}
		NEXT;
	}));

	Install(new Word("pipe>",WORD_FUNC {
		if(inContext.fromPipe==NULL) {
			return inContext.Error(NoParamErrorID::NoChanFromPipe);
		}
		inContext.DS.emplace_back(inContext.fromPipe->Recv());
		NEXT;
	}));
	
	Install(new Word("eoc",WORD_FUNC {
		inContext.DS.emplace_back(DataType::EoC);
		NEXT;
	}));

	// dup & eoc?
	Install(new Word("eoc?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==DataType::EoC);
		NEXT;
	}));

	Install(new Word("reset-pipes",WORD_FUNC {
		if(inContext.toPipe!=NULL) {
			inContext.toPipe->CloseOnWrite();
			inContext.toPipe.reset();
		}
		if(inContext.fromPipe!=NULL) {
			inContext.fromPipe->RemoveReader();
			inContext.fromPipe.reset();
		}
		if(inContext.toPipe==NULL) {
			inContext.toPipe.reset(new ChanMan(1,">pipe"));
		}
		NEXT;
	}));

	Install(new Word("_pipe>!eoc?",WORD_FUNC {
		// pipe>
		if(inContext.fromPipe.get()==nullptr) {
			return inContext.Error(NoParamErrorID::NoChanFromPipe);
		}
		TypedValue tv=inContext.fromPipe->Recv();
		if(tv.dataType==DataType::EoC) {
			inContext.DS.emplace_back(false);	// loop exit by _branch-if-false
		} else {
			inContext.DS.emplace_back(tv);
			inContext.DS.emplace_back(true);
		}
		NEXT;
	}));

	// while-pipe
	// SS:
	//    +------------------+
	//    | SyntaxWhile      |
	//    +------------------+
	//    | 7 (==chunk size) |
	//    +------------------+
	//    | alpha1           | <--- the address of loop top with repeat-check (*1)
	//    +------------------+
	//    | alpha2           | <--- the address of repeat-check branch for 'leave'
	//    +------------------+
	//    | alpha3           | <--- the empty slot address for loop-exit.
	//    +------------------+
	//    | alpha4           | <--- the address of loop top without repeat-check (*2)
	//    +------------------+      (top of the loop block)
	//    | alpha5           | <--- the address for 'continue' (same as alpha1)
	//    +------------------+
	//
	//    *1  for the word 'repeat'
	//    *2  for the word 'redo'.
	Install(new Word("while-pipe",WordLevel::Immediate,WORD_FUNC {
		if(inContext.BeginControlBlock()==false) {
			return inContext.Error(NoParamErrorID::SystemError);
		}
		TypedValue tvAlpha1=inContext.MarkHere();
		inContext.newWord->CompileWord("_pipe>!eoc?");
		TypedValue tvAlpha2=inContext.MarkHere();
		inContext.newWord->CompileWord("_branch-if-false");
		TypedValue tvAlpha3=inContext.MarkAndCreateEmptySlot();	// for exit address slot
		TypedValue tvAlpha4=inContext.MarkHere();

		const int chunkSize=7;
		inContext.SS.emplace_back(tvAlpha1);	// alpha5 equal to alpha1.
		inContext.SS.emplace_back(tvAlpha4);
		inContext.SS.emplace_back(tvAlpha3);
		inContext.SS.emplace_back(tvAlpha2);
		inContext.SS.emplace_back(tvAlpha1);
		inContext.SS.emplace_back(chunkSize);
		inContext.SS.emplace_back(ControlBlockType::SyntaxWhile);
		NEXT;
	}));

	Install(new Word("set-num-of-threads",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		if(tos.intValue<1) {
			G_NumOfCores=std::thread::hardware_concurrency();
		} else {
			G_NumOfCores=tos.intValue;
		}
		NEXT;
	}));

	Install(new Word("num-of-threads",WORD_FUNC {
		inContext.DS.emplace_back((int)G_NumOfCores);
		NEXT;	
	}));

	Install(new Word("thread-idx",WORD_FUNC {
		inContext.DS.emplace_back(inContext.threadIdx);
		NEXT;
	}));
}

static bool execBody(Context& inContext,int inNumOfParallels) {
	if(inContext.DS.size()<2) {
		return inContext.Error(NoParamErrorID::DsAtLeast2);
	}

	TypedValue tvExec=Pop(inContext.DS);
	if(tvExec.dataType!=DataType::DirectWord && tvExec.dataType!=DataType::NewWord) {
		return inContext.Error(InvalidTypeErrorID::TosWpOrNw,tvExec);
	}

	TypedValue tvLambdaInfo=Pop(inContext.DS);
	if(tvLambdaInfo.dataType!=DataType::CB) {
		return inContext.Error(InvalidTypeErrorID::SecondCB,tvLambdaInfo);
	}
		
	if(execParallel(inContext,tvExec,tvLambdaInfo,inNumOfParallels)==false) {
		return false;
	}
	return true;
}

static bool closeParallelBlock(Context& inContext,int inNumOfParallels) {
	inContext.newWord->numOfLocalVar=(int)inContext.newWord->localVarDict.size();
	inContext.newWord->CompileWord("_semis");
	Word *newWordBackup=inContext.newWord;
	inContext.newWord->BuildParam();
	inContext.FinishWordDef();
	TypedValue tvExec(DataType::NewWord,newWordBackup);

	TypedValue& tvThreadInfo=ReadTOS(inContext.SS);
	if(tvThreadInfo.dataType!=DataType::CB) {
		return inContext.Error(NoParamErrorID::SsBroken);
	}

	if(inContext.EndNoNameWordBlock()==false) { return false; }
	if(inContext.ExecutionThreshold==Level::Interpret) {
		return execParallel(inContext,tvExec,tvThreadInfo,inNumOfParallels);
	}
	inContext.newWord->CompileWord("_lit");
	inContext.newWord->CompileValue(tvThreadInfo);
	inContext.newWord->CompileWord("_lit");
	inContext.newWord->CompileValue(tvExec);
	if(inNumOfParallels==1) {
		inContext.newWord->CompileWord("_exec/single");
	} else {
		inContext.newWord->CompileWord("_exec/parallel");
	}

	return true;
}

static bool execParallel(Context& inContext,
						 TypedValue& inTvExec,TypedValue& inTvThreadInfo,
						 int inNumOfParallels) {
	assert(inNumOfParallels!=0);
	assert( IsThreadInfo(inTvThreadInfo) );

	std::shared_ptr<ChanMan> toChild;
	if((inTvThreadInfo.intValue
		& (int)ControlBlockType::kThreadMask_Open_ThreadOutputAsInput)!=0) {
		// the case of >[ , >[[
		if(inContext.toPipe.get()==nullptr) {
			inContext.New_ToPipe();
		}
		toChild=inContext.toPipe;
	} else if((inTvThreadInfo.intValue
			   & (int)ControlBlockType::kThreadMask_Open_TosChanAsInput)!=0) {
		// the case of >>[ , >>[[
   		TypedValue tvChan=Pop(inContext.DS);
   		if(tvChan.dataType!=DataType::Channel) {
			return inContext.Error(InvalidTypeErrorID::TosChannel,tvChan);
		}
		toChild=tvChan.channelPtr;
	}

	if((inTvThreadInfo.intValue & (int)ControlBlockType::kThreadMask_Open_TosBroadcast)!=0) {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		inContext.initParamForPBlock=Pop(inContext.DS);
		inContext.isInitParamBroadcast=true;
	}

	int numOfParallels = inNumOfParallels<0 ? G_NumOfCores : inNumOfParallels;

	if((inTvThreadInfo.intValue & (int)ControlBlockType::kThreadMask_Open_PushTosListElem)!=0) {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		numOfParallels=(int)tos.listPtr->size();
		inContext.initParamForPBlock=tos;
		inContext.isInitParamBroadcast=false;
	}

	std::vector<TypedValue> tvThreads;
	newChildThreads(&tvThreads,&inContext,inTvExec,numOfParallels);

	if(toChild.get()!=nullptr) {
		for(int i=0; i<numOfParallels; i++) {
			assert(tvThreads[i].dataType==DataType::Context);
			toChild->IncNumOfReader();
			tvThreads[i].contextPtr->fromPipe=toChild;
		}
	}

	std::shared_ptr<ChanMan> fromChild;
	if( ((inTvThreadInfo.intValue
		  & (int)ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)!=0)
	   || ((inTvThreadInfo.intValue
	   	    & (int)ControlBlockType::kThreadMask_Close_PushOutputChan)!=0) ) {
		// the case of ]> , ]]> , ]>> , ]]>>
		std::shared_ptr<ChanMan> fromChild(new ChanMan(numOfParallels,">pipe"));
		for(int i=0; i<numOfParallels; i++) {
			assert(tvThreads[i].dataType==DataType::Context);
			tvThreads[i].contextPtr->toPipe=fromChild;
		}
		if((inTvThreadInfo.intValue
		  & (int)ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)!=0) {
			// the case of ]> , ]]> , ]=> , ]]=>
			if(inContext.fromPipe.get()!=NULL) {
				inContext.fromPipe->RemoveReader();
			}
			inContext.fromPipe=fromChild;
			fromChild->IncNumOfReader();
		} else if((inTvThreadInfo.intValue
	   	    & (int)ControlBlockType::kThreadMask_Close_PushOutputChan)!=0) {
			// the case of ]>> , ]]>> , ]=>> , ]]=>>
			inContext.DS.emplace_back(fromChild);
		}
	}

	TypedValue tvChildContext=runThreads(tvThreads);
	bool ret=tvChildContext.IsValid();
	if(ret && (inTvThreadInfo.intValue
			   & (int)ControlBlockType::kThreadMask_Close_PushContext)!=0) {
		inContext.DS.emplace_back(tvChildContext); 
	}

	return ret;
}

static void newChildThreads(std::vector<TypedValue> *outThreads,
							Context *inParent,
							TypedValue inTvExec,int inNumOfParallels) {
	for(int i=0; i<inNumOfParallels; i++) {
		Context *childContext=new Context(inParent,Level::Interpret);
		childContext->threadIdx=i;
		if(inParent->isInitParamBroadcast) {
			childContext->DS.emplace_back(inParent->initParamForPBlock);
		} else {
			childContext->DS.emplace_back(inParent->initParamForPBlock.listPtr->at(i));
		}
		childContext->DS.emplace_back(inTvExec);
		outThreads->emplace_back(childContext);
	}
}

static TypedValue runThreads(std::vector<TypedValue> &inThreads) {
	int numOfParallels=(int)inThreads.size();
	TypedValue ret;
	if(numOfParallels>1) { ret=TypedValue(new List()); }
	ChanMan chan(numOfParallels,"runThreads");
	chan.IncNumOfReader();
	for(int i=0; i<numOfParallels; i++) {
		assert(inThreads[i].dataType==DataType::Context);
		std::pair<ChanMan*,TypedValue> kickerArg(&chan,inThreads[i]);
		inThreads[i].contextPtr->thread=NewSharedThread(kicker,&kickerArg);

		TypedValue tvChildContext=chan.Recv();
// printf("runThreads Recv Done (i=%d)\n",i);
		if(numOfParallels>1) {
			ret.listPtr->emplace_back(tvChildContext);
		} else if(numOfParallels==1 && i==0) {
			ret=tvChildContext;
		}
	}
// printf("runThreads RemoveReader\n");
	chan.RemoveReader();
	while(chan.CanDestruct()==false) {
		ppYield();
	}
	return ret;
}

static void *kicker(void *inKickerArg) {
	auto kickerArg=(std::pair<ChanMan*,TypedValue> *)inKickerArg;
	ChanMan *chanPtr=kickerArg->first;
	TypedValue tvChildContext=kickerArg->second;
	assert(tvChildContext.dataType==DataType::Context);
	Context *childContext=tvChildContext.contextPtr.get();
	
	if(childContext->DS.size()<1) {
		childContext->Error(NoParamErrorID::DsIsEmpty);
		return NULL;
	}
	chanPtr->Send(tvChildContext);
	chanPtr->CloseOnWrite();
	TypedValue tvExec=Pop(childContext->DS);
	AddToThreadPool(tvChildContext);
	childContext->Exec(tvExec);

	// epilogue
	if(childContext->fromPipe!=NULL) {
		childContext->fromPipe->RemoveReader();
		childContext->fromPipe.reset();
	}
	if(childContext->toPipe!=NULL) {
		childContext->toPipe->CloseOnWrite();
		childContext->toPipe.reset();
	}
	tvChildContext.contextPtr->running=false;

	return NULL;
}

static bool threadInfoUpdate(Context& inContext,
							 ControlBlockType inAdditionalThreadInfo) {
	TypedValue tvThreadInfo=Pop(inContext.SS);
	if(IsThreadInfo(tvThreadInfo)==false) {
		return inContext.Error(NoParamErrorID::SsBroken);
	}
	tvThreadInfo.intValue|=(int)inAdditionalThreadInfo;
	inContext.SS.emplace_back(tvThreadInfo);
	return true;
}

static bool joinMain(Context& inContext,const TypedValue& inTV) {
	switch(inTV.dataType) {
		case DataType::Context:
			if( inTV.contextPtr->running ) {
				Lock(inTV.contextPtr->mutexForJoined);
					if(inTV.contextPtr->joined==false) {
						Join(inTV.contextPtr->thread.get());
						inTV.contextPtr->joined=true;
					}
				Unlock(inTV.contextPtr->mutexForJoined);
			}
			break;
		case DataType::Array: {
				if(inTV.arrayPtr.get()==nullptr) {
					ExitOnSystemError("joinMain:arrayPtr");
				}
				int n=inTV.arrayPtr->length;
				TypedValue *array=inTV.arrayPtr->data;
				if(array==NULL) { ExitOnSystemError("joinMain:array"); }
				for(int i=0; i<n; i++) {
					if(array[i].dataType!=DataType::Context) {
						return inContext.Error(InvalidTypeErrorID::ArrayElemContext,
											   array[i]);
					}
					joinMain(inContext,array[i]);
				}
			}
			break;
		case DataType::List: {
				if(inTV.listPtr.get()==nullptr) {
					ExitOnSystemError("joinMain:listPtr");
				}
				int n=(int)inTV.listPtr->size();
				for(int i=0; i<n; i++) {
					TypedValue& tvElem=inTV.listPtr->at(i);
					if(tvElem.dataType!=DataType::Context) {
						return inContext.Error(InvalidTypeErrorID::ListElemContext,
											   tvElem);
					}
					joinMain(inContext,tvElem);
				}
			}
			break;
		case DataType::KV: {
				if(inTV.kvPtr.get()==nullptr) { ExitOnSystemError("joinMain:kvPtr"); }
				for(auto const &pair : *inTV.kvPtr) {
					const TypedValue& tvElem=pair.second;
					if(tvElem.dataType!=DataType::Context) {
						return inContext.Error(InvalidTypeErrorID::KvElemContext,
											   tvElem);
					}
					joinMain(inContext,tvElem);
				}
			}
			break;
		default: return inContext.Error(InvalidTypeErrorID::TosContextOrContainer,inTV);
	}
	return true;
}

