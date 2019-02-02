#include <assert.h>

#include <thread>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool closeParallelBlock(Context& inContext,int inNumOfParallels);
static void execParallel(Context& inContext,TypedValue& inTVExec,
						 int inNumOfParallels=-1);
static void *kicker(void *inContext);

void InitDict_Parallel() {
	Install(new Word("[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("]",WordLevel::Immediate,WORD_FUNC {
		if(closeParallelBlock(inContext,1)==false) { return false; }
		NEXT;
	}));

	Install(new Word("[[",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("]]",WordLevel::Immediate,WORD_FUNC {
		const int numOfCores = inContext.isInitParamBroadcast
							 ? G_NumOfCores
							 : (int)inContext.initParamForPBlock.listPtr->size();
		if(closeParallelBlock(inContext,numOfCores)==false) { return false; }
		NEXT;
	}));
	
	Install(new Word("_exec/single",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeDirectWord && tos.dataType!=kTypeNewWord) {
			return inContext.Error_InvalidType(E_TOS_WP_OR_NW,tos);
		}
		execParallel(inContext,tos,1);
		NEXT;
	}));	

	Install(new Word("_exec/parallel",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeDirectWord && tos.dataType!=kTypeNewWord) {
			return inContext.Error_InvalidType(E_TOS_WP_OR_NW,tos);
		}
		execParallel(inContext,tos);
		NEXT;
	}));	

	Install(new Word(">child",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		if(inContext.toChild==NULL) {
			inContext.toChild.reset(new ChanMan(1,">child"));
		}
		if(inContext.toChild->IsOpen==false) {
			return inContext.Error(E_CHAN_IS_ALREADY_CLOSED);
		}
		if(tv.dataType==kTypeEOC) {
			inContext.toChild->CloseOnWrite();
		} else {
			if(inContext.toChild->Send(tv)==false) {
				return inContext.Error(E_CHAN_IS_ALREADY_CLOSED);
			}
		}
		NEXT;
	}));

	Install(new Word("parent>",WORD_FUNC {
		if(inContext.fromParent==NULL) {
			return inContext.Error(E_NO_CHAN_FROM_PARENT);
		}
		inContext.DS.emplace_back(inContext.fromParent->Recv());
		NEXT;
	}));

	Install(new Word(">parent",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		if(inContext.toParent==NULL) { return inContext.Error(E_NO_CHAN_TO_PARENT); }
		if(tv.dataType==kTypeEOC) {
			inContext.toParent->CloseOnWrite();
		} else {
			if(inContext.toParent->Send(tv)==false) {
				return inContext.Error(E_CHAN_IS_ALREADY_CLOSED);
			}
		}
		NEXT;
	}));

	Install(new Word("child>",WORD_FUNC {
		if(inContext.fromChild==NULL) { return inContext.Error(E_NO_CHAN_FROM_CHILD); }
		if(inContext.fromChild->IsOpen==false) {
			return inContext.Error(E_CHAN_IS_ALREADY_CLOSED);
		}
		inContext.DS.emplace_back(inContext.fromChild->Recv());
		NEXT;
	}));

	Install(new Word(">pipe",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		if(inContext.toPipe==NULL
		  || (&inContext==GlobalContext
			  && inContext.fromPipe==inContext.lastOutputPipe)) {
			inContext.toPipe.reset(new ChanMan(1,">pipe"));
			inContext.lastOutputPipe=inContext.toPipe;
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType==kTypeEOC) {
			inContext.toPipe->CloseOnWrite();
		} else {
			if(inContext.toPipe->Send(tos)==false) {
				return inContext.Error(E_CHAN_IS_ALREADY_CLOSED);
			}
		}
		NEXT;
	}));

	Install(new Word("pipe>",WORD_FUNC {
		if(&inContext==GlobalContext && inContext.fromPipe==NULL) {
			inContext.fromPipe=inContext.lastOutputPipe;
		} else if(inContext.fromPipe==NULL) {
			return inContext.Error(E_NO_CHAN_FROM_PIPE);
		}
		inContext.DS.emplace_back(inContext.fromPipe->Recv());
		NEXT;
	}));
	
	Install(new Word("eoc",WORD_FUNC {
		inContext.DS.emplace_back(kTypeEOC);
		NEXT;
	}));

	// dup & eoc?
	Install(new Word("eoc?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }	
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.dataType==kTypeEOC);
		NEXT;
	}));

	Install(new Word("reset-pipes",WORD_FUNC {
		if(inContext.toChild!=NULL) {
			inContext.toChild->CloseOnWrite();
			inContext.toChild.reset();
		}
		if(inContext.fromChild!=NULL) {
			inContext.fromChild->RemoveReader();
			inContext.fromChild.reset();
		}
		if(inContext.toPipe!=NULL) {
			inContext.toPipe->CloseOnWrite();
			inContext.toPipe.reset();
		}
		if(inContext.fromPipe!=NULL) {
			inContext.fromPipe->RemoveReader();
			inContext.fromPipe.reset();
		}

		if(inContext.toChild==NULL) {
			inContext.toChild.reset(new ChanMan(1,">child"));
		}
		if(inContext.toPipe==NULL) {
			inContext.toPipe.reset(new ChanMan(1,">pipe"));
			inContext.lastOutputPipe=inContext.toPipe;
		}

		NEXT;
	}));

	// equivalent to "pipe> eoc? _branch-if-true".
	Install(new Word("_while/pipe>_body",WORD_FUNC {
		// pipe>
		if(inContext.fromPipe==NULL) {
			if(&inContext==GlobalContext) {
				inContext.toPipe->CloseOnWrite();
				inContext.fromPipe=inContext.lastOutputPipe;
			} else { return inContext.Error(E_NO_CHAN_FROM_PIPE); }	
		}
		TypedValue tv=inContext.fromPipe->Recv();
		
		// eoc? and _branch-if-true
		if(tv.dataType==kTypeEOC) {
			inContext.ip=(const Word**)(*(inContext.ip+1));
		} else {
			inContext.DS.emplace_back(tv);
			inContext.ip+=2;
		}
		return true;
	}));

	// while-pipe
	Install(new Word("while-pipe",WordLevel::Immediate,WORD_FUNC {
		inContext.BeginControlBlock(kSyntax_WHILE);
		inContext.MarkHere();
		inContext.Compile(std::string("_while/pipe>_body"));
		inContext.MarkAndCreateEmptySlot();	// for exit address slot
		inContext.EnterLeavableLoop();
		NEXT;
	}));

	Install(new Word("_broadcastToChild",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.initParamForPBlock=Pop(inContext.DS);
		inContext.isInitParamBroadcast=true;
		NEXT;
	}));

	Install(new Word("_initChildDSByList",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		if(tos.listPtr->size()<1) { return inContext.Error(E_TOS_LIST_NO_ELEMENT); }
		if(tos.listPtr->size()<(size_t)G_NumOfCores) {
			return inContext.Error(E_TOS_LIST_AT_LEAST_CORES);
		}
		inContext.initParamForPBlock=tos;
		inContext.isInitParamBroadcast=false;
		NEXT;
	}));

	Install(new Word("[->",WordLevel::Immediate,WORD_FUNC {
		const std::string _broadcastToChild("_broadcastToChild");
		if(inContext.ExecutionThreshold==kInterpretLevel) {
			inContext.Exec(_broadcastToChild);
		} else {
			inContext.Compile(_broadcastToChild);
		}
		// same as '['
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("[[->",WordLevel::Immediate,WORD_FUNC {
		const std::string _broadcastToChild("_broadcastToChild");
		if(inContext.ExecutionThreshold==kInterpretLevel) {
			inContext.Exec(_broadcastToChild);
		} else {
			inContext.Compile(_broadcastToChild);
		}
		// same as '[['
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

#if 0
	// pointless?
	Dict["[=>"]=new Word("[=>",WordLevel::Immediate,WORD_FUNC {
		std::string _initChildDSByList("_initChildDSByList");
		if(inContext.ExecutionThreshold==kInterpretLevel) {
			inContext.Exec(_initChildDSByList);
		} else {
			inContext.Compile(_initChildDSByList);
		}
		// same as '['
		inContext.BeginNoNameWordBlock();
		NEXT;
	});
#endif

	Install(new Word("[[=>",WordLevel::Immediate,WORD_FUNC {
		std::string _initChildDSByList("_initChildDSByList");
		if(inContext.ExecutionThreshold==kInterpretLevel) {
			inContext.Exec(_initChildDSByList);
		} else {
			inContext.Compile(_initChildDSByList);
		}
		// same as '[['
		inContext.BeginNoNameWordBlock();
		NEXT;
	}));

	Install(new Word("set-num-of-threads",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
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
}

static bool closeParallelBlock(Context& inContext,int inNumOfParallels) {
	inContext.Compile(std::string("_semis"));
	inContext.FinishNewWord();
	TypedValue tvExec(kTypeNewWord,inContext.newWord);
	if(inContext.EndNoNameWordBlock()==false) { return false; }
	if(inContext.ExecutionThreshold!=kInterpretLevel) {
		inContext.Compile(std::string("_lit"));
		inContext.Compile(tvExec);
		if(inNumOfParallels==1) {
			inContext.Compile(std::string("_exec/single"));
		} else {
			inContext.Compile(std::string("_exec/parallel"));
		}
	} else {
		execParallel(inContext,tvExec,inNumOfParallels);
	}
	return true;
}

static void execParallel(Context& inContext,TypedValue& inTVExec,
						 int inNumOfParallels) {
	assert(inTVExec.dataType==kTypeWord || inTVExec.dataType==kTypeNewWord);
	assert(inNumOfParallels!=0);

	const int numOfParallels = inNumOfParallels<0 ? G_NumOfCores
												  : inNumOfParallels;
	if( inContext.fromChild ) {
		inContext.fromChild->RemoveReader();
		inContext.fromChild.reset();
	}	
	inContext.fromChild.reset(new ChanMan(numOfParallels,"child>"));
	inContext.fromChild->SetNumOfReader(1);	// only parent

	if( inContext.toChild ) {
		inContext.toChild->CloseOnWrite();
		inContext.toChild.reset(new ChanMan(1,">child"));
		inContext.toChild->SetNumOfReader(numOfParallels);
	}
	if( inContext.lastOutputPipe ) {
		inContext.lastOutputPipe->SetNumOfReader(numOfParallels);
	}

	std::shared_ptr<ChanMan> toPipe(new ChanMan(numOfParallels,">pipe"));
		for(int i=0; i<numOfParallels; i++) {
			Context *childContext=new Context(&inContext,kInterpretLevel,
											  inContext.toChild,
											  inContext.fromChild);	
			childContext->fromPipe=inContext.lastOutputPipe;
			childContext->toPipe=toPipe;
			if(inContext.initParamForPBlock.dataType!=kTypeInvalid) {
				if(inContext.isInitParamBroadcast) {
					childContext->DS.emplace_back(
											inContext.initParamForPBlock);
				} else {
					childContext->DS.emplace_back(
							inContext.initParamForPBlock.listPtr->at(i));
				}
			}
			childContext->DS.emplace_back(inTVExec);
			NewThread(childContext->thread,kicker,childContext);
		}
		inContext.lastOutputPipe=toPipe;
	toPipe.reset();

	inContext.initParamForPBlock=TypedValue();
	inContext.isInitParamBroadcast=true;
	
	if(&inContext==GlobalContext) {
		inContext.fromPipe=inContext.lastOutputPipe;
	}
}
static void *kicker(void *inContext) {
	Context *childContext=(Context *)inContext;
	
	if(childContext->DS.size()<1) {
		childContext->Error(E_DS_IS_EMPTY);
		return NULL;
	}
	TypedValue tvExec=Pop(childContext->DS);
	childContext->Exec(tvExec);

	// epilogue
	if(childContext->fromParent!=NULL) {
		childContext->fromParent->RemoveReader();
		childContext->fromParent.reset();
	}
	if(childContext->toParent!=NULL) {
		childContext->toParent->CloseOnWrite();
		childContext->toParent.reset();
	}

	if(childContext->fromChild!=NULL) {
		// Wait for all child thread to finish.
		while(childContext->fromChild->NumOfWriter>0) {
			Yield();
		}
		if(childContext->fromChild->RemoveReader()==false) {
			fprintf(stderr,"WARNING 'child>': Data still exists in the channel.\n");
		}
		childContext->fromChild.reset();
	}
	if(childContext->toChild!=NULL) {
		childContext->toChild->CloseOnWrite();
		childContext->toChild.reset();
	}

	if(childContext->fromPipe!=NULL) {
		childContext->fromPipe->RemoveReader();
		childContext->fromPipe.reset();
	}
	if(childContext->toPipe!=NULL) {
		childContext->toPipe->CloseOnWrite();
		childContext->toPipe.reset();
	}

	return NULL;
}

