#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "thread.h"

static inline bool swap(Stack& ioStack) {
	TypedValue tos=Pop(ioStack);
	TypedValue second=Pop(ioStack);
	ioStack.emplace_back(tos);
	ioStack.emplace_back(second);
	return true;
}

void InitDict_Stack() {
	Install(new Word("clear",WORD_FUNC {
		inContext.DS.clear();
		NEXT;
	}));

	Install(new Word("clear-rs",WORD_FUNC {
		inContext.RS.clear();
		NEXT;
	}));

	Install(new Word("empty?",WORD_FUNC {
		inContext.DS.emplace_back(inContext.DS.size()==0);
		NEXT;
	}));

	Install(new Word("depth",WORD_FUNC {
		inContext.DS.emplace_back((int)inContext.DS.size());
		NEXT;
	}));

	Install(new Word("dup",WORD_FUNC {
		if(inContext.DS.size()==0) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.DS.emplace_back(inContext.DS.back());
		NEXT;
	}));

	// コンテナのみを clone する。
	// 全ての要素レベルで clone する場合は full-clone を使うこと。
	Install(new Word("clone",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeString:
			case kTypeSymbol:
				inContext.DS.emplace_back(*tos.stringPtr);
				break;
			case kTypeArray: {
					Array<TypedValue> *srcPtr=tos.arrayPtr.get();
					Lock(srcPtr->mutex);
						const int n=srcPtr->length;
						TypedValue *dataBody=new TypedValue[n];
						for(int i=0; i<n; i++) {
							dataBody[i]=srcPtr->data[i];
						}
					Unlock(srcPtr->mutex);
					Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
   					inContext.DS.emplace_back(arrayPtr);
				}
				break;
			case kTypeList: {
					const std::deque<TypedValue> *srcPtr=tos.listPtr.get();
					std::deque<TypedValue> *destPtr=new std::deque<TypedValue>(*srcPtr);
					inContext.DS.emplace_back(destPtr);
				}
				break;			
			default:
				inContext.DS.emplace_back(tos);
		}
		NEXT;
	}));

	Install(new Word("full-clone",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		TypedValue tvFullClone=FullClone(tos);
		inContext.DS.emplace_back(tvFullClone);
		NEXT;
	}));

	Install(new Word("swap",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		if(swap(inContext.DS)==false) { return false; }
		NEXT;
	}));

	Install(new Word("swap-rs",WORD_FUNC {
		if(inContext.RS.size()<2) { return inContext.Error(E_RS_AT_LEAST_2); }
		if(swap(inContext.RS)==false) { return false; }
		NEXT;
	}));

	// x y --- x y x
	Install(new Word("over",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue& second=inContext.DS[inContext.DS.size()-2];
		inContext.DS.emplace_back(second);
		NEXT;
	}));	

	Install(new Word("pick",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) { return inContext.Error(E_TOS_INT,tos); }
		const int n=tos.intValue;
		if(inContext.DS.size()<n+1) {
			return inContext.Error(E_DEPTH_INDEX_OUT_OF_RANGE,
								   (int)inContext.DS.size(),n+1);
		}
		if(n<0) { return inContext.Error(E_TOS_NON_NEGATIVE,n); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-n-1]);
		NEXT;
	}));

	// a_n ... a2 a1 a0 X k --- a_n ... a_{k+1} X a_{k-1} ... a1 a0
	// nothing change by 'k pick k replace'
	Install(new Word("replace",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) { return inContext.Error(E_TOS_INT,tos); }
		TypedValue newValue=Pop(inContext.DS);
		const int n=tos.intValue;
		if(inContext.DS.size()<n+1) {
			return inContext.Error(E_DEPTH_INDEX_OUT_OF_RANGE,
								   (int)inContext.DS.size(),n+1);
		}
		if(n<0) { return inContext.Error(E_TOS_NON_NEGATIVE,n); }
		const int size=(int)inContext.DS.size();
		inContext.DS[size-n-1]=newValue;
		NEXT;
	}));
	
	// x y --- x y x y
	Install(new Word("2dup",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue& tos=ReadTOS(inContext.DS);
		TypedValue& second=inContext.DS[inContext.DS.size()-2];
		inContext.DS.emplace_back(second);
		inContext.DS.emplace_back(tos);
		NEXT;
	}));	

	Install(new Word("drop",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.DS.pop_back();
		NEXT;
	}));

	Install(new Word("2drop",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.DS.pop_back();
		inContext.DS.pop_back();
		NEXT;
	}));

	Install(new Word("3drop",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.DS.pop_back();
		inContext.DS.pop_back();
		inContext.DS.pop_back();
		NEXT;
	}));

	Install(new Word("drop-rs",WORD_FUNC {
		if(inContext.RS.size()==0) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.RS.pop_back();
		NEXT;
	}));

	// a b c --- b c a
	Install(new Word("rot",WORD_FUNC {
		if(inContext.DS.size()<3) { return inContext.Error(E_DS_AT_LEAST_3); }
		TypedValue third=inContext.DS[inContext.DS.size()-3];
		inContext.DS.erase(inContext.DS.end()-3);
		inContext.DS.emplace_back(third);
		NEXT;
	}));
	
	// (a b c -- c a b) note:inverse of rot.
	Install(new Word("inv-rot",WORD_FUNC {
		if(inContext.DS.size()<3) { return inContext.Error(E_DS_AT_LEAST_3); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.insert(inContext.DS.end()-2,tos);
		NEXT;
	}));

	Install(new Word(">r",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.RS.emplace_back(Pop(inContext.DS));
		NEXT;
	}));

	// equivalent to ">r >r"
	Install(new Word(">r>r",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.RS.emplace_back(Pop(inContext.DS));
		inContext.RS.emplace_back(Pop(inContext.DS));
		NEXT;
	}));

	// equivalent to "dup >r".
	Install(new Word("@>r",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.RS.emplace_back(tos);
		NEXT;
	}));

	Install(new Word("r>",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.DS.emplace_back(Pop(inContext.RS));
		NEXT;
	}));

	Install(new Word("r>r>",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.DS.emplace_back(Pop(inContext.RS));
		inContext.DS.emplace_back(Pop(inContext.RS));
		NEXT;
	}));

	Install(new Word("@r>",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.DS.emplace_back(inContext.RS.back());
		NEXT;
	}));

	Install(new Word("show",WORD_FUNC {
		ShowStack(inContext.DS,"DS");
		NEXT;
	}));

	Install(new Word("show-rs",WORD_FUNC {
		ShowStack(inContext.RS,"RS");
		NEXT;
	}));
}

