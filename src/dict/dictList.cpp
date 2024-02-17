#include <assert.h>
#include <climits>

#include <boost/format.hpp>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

extern void LockForConsoleOutput();
extern void UnlockForConsoleOutput();

static bool expandBody(Context& inContext,TypedValue& inTypedValue);
static bool hasElement(TypedValue& inTvList,TypedValue& inElement);

void InitDict_List() {
	Install(new Word("(",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.BeginListBlock();
		NEXT;
	}));

	Install(new Word(")",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }

		if(inContext.IsListConstructing()==false) {
			return inContext.Error(NoParamErrorID::ListMismatch);
		}
		TypedValue tvList=inContext.GetList();
		if(inContext.EndListBlock()==false) { return false; }

		switch(inContext.ExecutionThreshold) {
			case Level::Interpret:
				inContext.DS.emplace_back(tvList);
				break;
			case Level::Compile:	
				if(inContext.newWord==NULL) { goto newWord_is_NULL; }
				inContext.newWord->CompileWord("_lit");
				inContext.newWord->CompileValue(tvList);
				inContext.newWord->CompileWord("create-working-value");
				break;
			case Level::Symbol:
				if(inContext.newWord==NULL) { goto newWord_is_NULL; }
				inContext.newWord->CompileValue(tvList);
				break;
			default:
				inContext.Error(NoParamErrorID::SystemError);
				exit(-1);
		}
		NEXT;

newWord_is_NULL:
		fprintf(stderr,"SYSTEM ERROR at ')'. newWord should not be NULL.\n");
		exit(-1);
	}));

	Install(new Word("()",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }

		if(inContext.ExecutionThreshold==Level::Interpret) {
			inContext.DS.emplace_back(new List());
		} else if(inContext.newWord->type==WordType::List) {
			inContext.newWord->CompileValue(new List());
		} else {
			inContext.newWord->CompileWord("_create-empty-list");
			inContext.newWord->CompileWord("create-working-value");
		}
		NEXT;
	}));
	Install(new Word("_create-empty-list",WORD_FUNC {
		inContext.DS.emplace_back(new List());
		NEXT;
	}));

	Install(new Word("empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()==0);
		NEXT;
	}));

	Install(new Word("@empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()==0);
		NEXT;
	}));

	Install(new Word("not-empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()!=0);
		NEXT;
	}));

	Install(new Word("@not-empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()!=0);
		NEXT;
	}));

	// list x --- list
	Install(new Word("@append",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tos=Pop(inContext.DS);
		TypedValue& tvList=ReadTOS(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		tvList.listPtr->push_back(tos);
		NEXT;	
	}));

	// list x --- list
	Install(new Word("append",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tos=Pop(inContext.DS);
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		tvList.listPtr->push_back(tos);
		NEXT;	
	}));


	// x list --- list
	// equivalent to 'swap @append'.
	Install(new Word("@push-back",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tvList);
		}
		TypedValue tv=Pop(inContext.DS);
		tvList.listPtr->push_back(tv);
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// x list --- list
	// equivalent to 'swap append'.
	Install(new Word("push-back",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tvList);
		}
		TypedValue tv=Pop(inContext.DS);
		tvList.listPtr->push_back(tv);
		NEXT;
	}));

	// list --- X
	Install(new Word("car",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		NEXT;
	}));

	// list --- list X
	Install(new Word("@car",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		NEXT;
	}));

	Install(new Word("@cdr",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		
		const List *src=tos.listPtr.get();
		List *cdr=new List();
		cdr->assign(src->begin()+1,src->end());
		inContext.DS.emplace_back(cdr);
		NEXT;
	}));

	Install(new Word("cdr",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		
		const List *src=tos.listPtr.get();
		List *cdr=new List();
		cdr->assign(src->begin()+1,src->end());
		inContext.DS.emplace_back(cdr);
		NEXT;
	}));

	// list --- list x
	Install(new Word("@last",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Array:
				if(tos.arrayPtr->length<=0) {
					return inContext.Error(NoParamErrorID::TosArrayNoElement);
				}
				inContext.DS.emplace_back(tos.arrayPtr->data[tos.arrayPtr->length-1]);
				break;
			case DataType::List:
				if(tos.listPtr->size()<=0) {
					return inContext.Error(NoParamErrorID::TosListNoElement);
				}
				inContext.DS.emplace_back(tos.listPtr->back());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosArrayOrList,tos);
		}
		NEXT;
	}));

	// list --- x
	Install(new Word("last",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case DataType::Array:
				if(tos.arrayPtr->length<=0) {
					return inContext.Error(NoParamErrorID::TosArrayNoElement);
				}
				inContext.DS.emplace_back(tos.arrayPtr->data[tos.arrayPtr->length-1]);
				break;
			case DataType::List:
				if(tos.listPtr->size()<=0) {
					return inContext.Error(NoParamErrorID::TosListNoElement);
				}
				inContext.DS.emplace_back(tos.listPtr->back());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosArrayOrList,tos);
		}
		NEXT;
	}));

	// list --- list x
	Install(new Word("@pop-front",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		tos.listPtr->pop_front();
		NEXT;
	}));

	// list --- x
	Install(new Word("pop-front",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		if(tos.listPtr->size()<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		tos.listPtr->pop_front();
		NEXT;
	}));

	// (x -- list)
	Install(new Word("en-list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		TypedValue tvList=TypedValue(new List());
		tvList.listPtr->push_back(tos);
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// { list | array } -- e1 e2 ... eN
	// old name 'de-list'
	Install(new Word("expand",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(expandBody(inContext,tos)==false) {
			return inContext.Error(InvalidTypeErrorID::TosArrayOrList,tos);
		}
		NEXT;
	}));
	
	// list -- list e1 e2 ... eN
	Install(new Word("@expand",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(expandBody(inContext,tos)==false) {
			return inContext.Error(InvalidTypeErrorID::TosArrayOrList,tos);
		}
		NEXT;
	}));
		
	// ( a b -- (a b) )
	Install(new Word("tuple",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvList=TypedValue(new List());
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		tvList.listPtr->push_back(second);
		tvList.listPtr->push_back(tos);
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// list n --- list1 list2
	Install(new Word("split",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}
		List *list=second.listPtr.get();
		int n=tos.intValue;
		if(n<0 || n>=(int)list->size()) {
			return inContext.Error(ErrorIdWith2int::ListIndexOutOfRange,
								   (int)list->size(),n);
		}
		List *latter=new List();
		latter->assign(list->begin()+n,list->end());
		list->erase(list->begin()+n,list->end());
		inContext.DS.emplace_back(latter);
		NEXT;
	}));

#if 0
	// list n --- L1 L2 ... Ln
	Install(new Word("divide",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tvN=Pop(inContext.DS);
		if(tvN.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tvN);
		}
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}
		int n=tvN.intValue;
		int numOfElement=(int)tvList.listPtr->size();
		for(int i=0; i<n-1; i++) {
			if(i<numOfElement) {
				inContext.DS.emplace_back(tvList.listPtr->at(i));
			} else {
				inContext.DS.emplace_back(new List());
			}
		}
		List *rest=new List();
		for(int i=n-1; i<numOfElement; i++) {
			rest->push_back(tvList.listPtr->at(i));
		}
		inContext.DS.emplace_back(rest);
		NEXT;
	}));
#endif

	// list1 list2 --- list
	Install(new Word("concat",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}

		second.listPtr->insert(second.listPtr->end(),
							   tos.listPtr->begin(),tos.listPtr->end());
		NEXT;
	}));

	// list start length --- list1 list2
	Install(new Word("slice",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvLength=Pop(inContext.DS);
		if(tvLength.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tvLength);
		}
		const int length=tvLength.intValue;
		if(length<0) {
			return inContext.Error(ErrorIdWithInt::TosNonNegative,length);
		}

		TypedValue tvStart=Pop(inContext.DS);
		if(tvStart.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::SecondInt,tvStart);
		}
		const int start=tvStart.intValue;
		if(start<0) {
			return inContext.Error(ErrorIdWithInt::SecondNonNegative,start);
		}

		TypedValue& tvList=ReadTOS(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::ThirdList,tvList);
		}

		List *list=tvList.listPtr.get();
		if((int)list->size()<=start) {
			return inContext.Error(ErrorIdWith2int::ListStartIndexOutOfRange,
								   (int)list->size(),start);
		}
		if((int)list->size()<start+length) {
			return inContext.Error(ErrorIdWith2int::ListLengthIndexOutOfRange,
								   (int)list->size(),start+length);
		}
		
		List *slice=new List();
		slice->assign(list->begin()+start,list->begin()+start+length);
		list->erase(list->begin()+start,list->begin()+start+length);
		inContext.DS.emplace_back(slice);
		NEXT;
	}));

	// ex: ( A B ) ( 1 2 3 ) --> ( (A 1) (A 2) (A 3) (B 1) (B 2) (B 3) )
	Install(new Word("product",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}	

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}

		List *result=new List();

		List *list1=second.listPtr.get();
		const size_t n1=list1->size();

		List *list2=tos.listPtr.get();
		const size_t n2=list2->size();	

		for(size_t i=0; i<n1; i++) {
			for(size_t j=0; j<n2; j++) {
				List *e=new List();
				e->emplace_back(list1->at(i));
				e->emplace_back(list2->at(j));
				result->emplace_back(e);
			}
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// ex: ( a b ) ( 1 2 ) -> ( ( a 1 ) ( b 2 ) )
	Install(new Word("zip",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tos);
		}
		if(tos.listPtr->size()!=second.listPtr->size()) {
			return inContext.Error(NoParamErrorID::ListMismatch);
		}
		List *result=new List();
		const size_t n=tos.listPtr->size();
		for(size_t i=0; i<n; i++) {
			List *tuple=new List();
			tuple->emplace_back(second.listPtr->at(i));
			tuple->emplace_back(tos.listPtr->at(i));
			result->emplace_back(tuple);
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));
	// ex: ( a b c ) ( 1 2 ) -> ( ( a 1 ) ( b 2 ) )
	Install(new Word("min-zip",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tos);
		}
		List *result=new List();
		const size_t n=std::min(tos.listPtr->size(),second.listPtr->size());
		for(size_t i=0; i<n; i++) {
			List *tuple=new List();
			tuple->emplace_back(second.listPtr->at(i));
			tuple->emplace_back(tos.listPtr->at(i));
			result->emplace_back(tuple);
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));
	// ex: ( a b c ) ( 1 2 ) -> ( ( a 1 ) ( b 2 ) ( c invalid ) )
	Install(new Word("max-zip",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tos);
		}
		List *result=new List();
		const size_t numOfTos=tos.listPtr->size();
		const size_t numOfSecond=second.listPtr->size();
		const size_t n=std::max(numOfTos,numOfSecond);
		TypedValue tvInvalid;
		for(size_t i=0; i<n; i++) {
			List *tuple=new List();
			tuple->emplace_back(i<numOfSecond ? second.listPtr->at(i) : tvInvalid);
			tuple->emplace_back(i<numOfTos    ? tos.listPtr->at(i)    : tvInvalid);
			result->emplace_back(tuple);
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));
	// ex: ( a b c ) ( 1 2 ) "" -> ( ( a 1 ) ( b 2 ) ( c "" ) )
	Install(new Word("max-zip-with",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvDefaultValue=Pop(inContext.DS);
		TypedValue tvL1=Pop(inContext.DS);
		if(tvL1.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tvL1);
		}
		TypedValue tvL2=Pop(inContext.DS);
		if(tvL2.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvL2);
		}
		List *result=new List();
		const size_t numOfL1=tvL1.listPtr->size();
		const size_t numOfL2=tvL2.listPtr->size();
		const size_t n=std::max(numOfL1,numOfL2);
		for(size_t i=0; i<n; i++) {
			List *tuple=new List();
			tuple->emplace_back(i<numOfL2 ? tvL2.listPtr->at(i) : tvDefaultValue);
			tuple->emplace_back(i<numOfL1 ? tvL1.listPtr->at(i) : tvDefaultValue);
			result->emplace_back(tuple);
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// ( { cond1 } { cond2 } ... { condN } ) and
	// ex: check in [0,10)
	// 	( { dup 0 >= } { 10 < } ) and
	Install(new Word("and",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		auto listPtr=tos.listPtr.get();
		const size_t n=listPtr->size();
		for(size_t i=0; i<n; i++) {
			TypedValue& e=listPtr->at(i);
			if(e.HasWordPtr(NULL)==false) {
				return inContext.Error(NoParamErrorID::TosLambdaList);
			}
			if(inContext.Exec(e)==false) {
				return false;
			}
			TypedValue result=Pop(inContext.DS);
			if(result.dataType!=DataType::Bool || result.boolValue!=true) {
				inContext.DS.emplace_back(false);
				goto onExit;
			}
		}
		inContext.DS.emplace_back(true);
onExit:
		NEXT;
	}));

	// ( { cond1 } { cond2 } ... { condN } ) or
	// ex: to check tos>10 || tos<0 
	//	( { dup 10 > } { 0 < } ) or
	Install(new Word("or",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		auto listPtr=tos.listPtr.get();
		const size_t n=listPtr->size();
		for(size_t i=0; i<n; i++) {
			TypedValue& e=listPtr->at(i);
			if(e.dataType!=DataType::Word && e.dataType!=DataType::Lambda) {
				return inContext.Error(NoParamErrorID::TosLambdaList);
			}
			if(inContext.Exec(e)==false) {
				return false;
			}
			TypedValue result=Pop(inContext.DS);
			if(result.dataType==DataType::Bool && result.boolValue==true) {
				inContext.DS.emplace_back(true);
				goto onExit;
			}
		}
		inContext.DS.emplace_back(false);
onExit:
		NEXT;
	}));

	// list lambda --- list
	// note:
	// 	In the applicable process, you can get the index value of the element
	// 	currently being processed by the Word "i". 
	Install(new Word("map",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tos=Pop(inContext.DS);
		// if(tos.dataType!=DataType::Word) {
		if(tos.HasWordPtr(NULL)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}

		auto src=second.listPtr.get();

		TypedValue result=TypedValue(new List());
		auto dest=result.listPtr.get();

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=src->size();
		inContext.RS.emplace_back(-1);
		for(size_t i=0; i<n; i++) {
			TypedValue& rsTos=ReadTOS(inContext.RS);
			if(rsTos.dataType!=DataType::Int) {
				return inContext.Error(NoParamErrorID::RsBroken);
			}
			rsTos.intValue=(int)i;

			inContext.DS.emplace_back(src->at(i));
			if(inContext.Exec(tos)==false) {
				inContext.suppressError=true;
			}
			TypedValue t=Pop(inContext.DS);
			dest->emplace_back(t);
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBroken);
		}
		Pop(inContext.RS);

		inContext.DS.emplace_back(result);
		inContext.suppressError=suppressErrorBackup;
		NEXT;
	}));

	// list list --- list
	// ( 1 2 ) ( { 2 * } { 3 * } ) --- ( 2 6 )
	Install(new Word("vmap",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}
		if(second.listPtr->size()!=tos.listPtr->size()) {
			return inContext.Error(NoParamErrorID::ListMismatch);
		}
		auto lambdaList=tos.listPtr.get();
		auto srcList=second.listPtr.get();
		const size_t n=srcList->size();
		for(size_t i=0; i<n; i++) {
			if(lambdaList->at(i).dataType!=DataType::Word) {
				return inContext.Error(NoParamErrorID::TosLambdaList);
			}
		}

		TypedValue result=TypedValue(new List());
		auto destList=result.listPtr.get();

		bool suppressErrorBackup=inContext.suppressError;
		for(size_t i=0; i<n; i++) {
			inContext.DS.emplace_back(srcList->at(i));
			if(inContext.Exec(lambdaList->at(i))==false) {
				inContext.suppressError=true;
			}
			TypedValue t=Pop(inContext.DS);
			destList->emplace_back(t);
		}
		inContext.DS.emplace_back(result);
		inContext.suppressError=suppressErrorBackup;
		NEXT;
	}));
	
	// list lambda --- list
	Install(new Word("sort",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.HasWordPtr(NULL)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}

		auto src=second.listPtr.get();
		bool suppressErrorBackup=inContext.suppressError;
		std::sort(src->begin(),src->end(),
			  [&inContext,&lambda](const TypedValue& inA,const TypedValue& inB)->bool {
				inContext.DS.emplace_back(inA);
				inContext.DS.emplace_back(inB);
				if(inContext.Exec(lambda)==false) {
					inContext.suppressError=true;
				}
				TypedValue t=Pop(inContext.DS);
				if(t.dataType!=DataType::Bool) {
					return false;
				}
				return t.boolValue;
		});
		inContext.suppressError=suppressErrorBackup;
		NEXT;
	}));

	// list --- list
	Install(new Word("sort+",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		auto src=tos.listPtr.get();
		std::sort(src->begin(),src->end(),
			  [&inContext](const TypedValue& inA,const TypedValue& inB)->bool {
				bool ret;
				GetCmpOpResult(ret,inA,<,inB);
				return ret;
		});
		NEXT;
	}));

	// list --- list
	Install(new Word("sort-",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		auto src=tos.listPtr.get();
		std::sort(src->begin(),src->end(),
			  [&inContext](const TypedValue& inA,const TypedValue& inB)->bool {
				bool ret;
				GetCmpOpResult(ret,inA,>,inB);
				return ret;
		});
		NEXT;
	}));

	// list lambda --- list
	// note:
	// 	In the applicable process, you can get the index value of the element
	// 	currently being processed by the Word "i". 
	Install(new Word("filter",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=DataType::Word && lambda.dataType!=DataType::Lambda) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		TypedValue result(new List());

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=tvList.listPtr->size();
		inContext.RS.emplace_back(-1);
		for(size_t i=0; i<n; i++) {
			TypedValue& rsTos=ReadTOS(inContext.RS);
			if(rsTos.dataType!=DataType::Int) {
				return inContext.Error(NoParamErrorID::RsBroken);
			}
			rsTos.intValue=(int)i;

			TypedValue& target=tvList.listPtr->at(i);
			inContext.DS.emplace_back(target);
			if(inContext.Exec(lambda)==false) {
				inContext.suppressError=true;
			}
			TypedValue t=Pop(inContext.DS);
			if(t.dataType==DataType::Bool && t.boolValue) {
				result.listPtr->push_back(target);
			}
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBroken);
		}
		Pop(inContext.RS);

		inContext.DS.emplace_back(result);
		NEXT;
	}));

	// list lambda --- list
	// note:
	// 	In the applicable process, you can get the index value of the element
	// 	currently being processed by the Word "i". 
	Install(new Word("find",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=DataType::Word && lambda.dataType!=DataType::Lambda) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=tvList.listPtr->size();
		inContext.RS.emplace_back(-1);
		bool find=false;
		for(size_t i=0; i<n; i++) {
			TypedValue& rsTos=ReadTOS(inContext.RS);
			if(rsTos.dataType!=DataType::Int) {
				return inContext.Error(NoParamErrorID::RsBroken);
			}
			rsTos.intValue=(int)i;

			TypedValue& target=tvList.listPtr->at(i);
			inContext.DS.emplace_back(target);
			if(inContext.Exec(lambda)==false) {
				inContext.suppressError=true;
			}
			TypedValue t=Pop(inContext.DS);
			if(t.dataType==DataType::Bool && t.boolValue) {
				inContext.DS.emplace_back(target);
				find=true;
				break;
			}
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBroken);
		}
		Pop(inContext.RS);

		if(find==false) { inContext.DS.emplace_back(TypedValue()); }
		NEXT;
	}));

#if 0	// reduce is same as foreach
	// list lambda --- list
	// note:
	// 	In the applicable process, you can get the index value of the element
	// 	currently being processed by the Word "i". 
	Install(new Word("reduce",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=tvList.listPtr->size();
		inContext.RS.emplace_back(-1);
		for(size_t i=0; i<n; i++) {
			TypedValue& rsTos=ReadTOS(inContext.RS);
			if(rsTos.dataType!=DataType::Int) {
				return inContext.Error(NoParamErrorID::RsBroken);
			}
			rsTos.intValue=(int)i;

			TypedValue& target=tvList.listPtr->at(i);
			inContext.DS.emplace_back(target);
			if(inContext.Exec(lambda)==false) {
				inContext.suppressError=true;
			}
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::RsBroken);
		}
		Pop(inContext.RS);

		NEXT;
	}));
#endif

	// list lambda --- ?
	Install(new Word("foreach",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.HasWordPtr(NULL)==false) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		const size_t n=tvList.listPtr->size();
		if(n>0) {
			bool suppressErrorBackup=inContext.suppressError;
			inContext.RS.emplace_back(0);
			for(size_t i=0; i<n; i++) {
				TypedValue& tvCounter=ReadTOS(inContext.RS);
				if(tvCounter.dataType!=DataType::Int) {
					return inContext.Error(NoParamErrorID::RsBroken);
				}
				tvCounter.intValue=(int)i;

				inContext.DS.emplace_back(tvList.listPtr->front());
				if(inContext.Exec(lambda)==false) {
					inContext.suppressError=true;
				}
				tvList.listPtr->pop_front();
			}
			Pop(inContext.RS);
			inContext.suppressError=suppressErrorBackup;
		}
		NEXT;	
	}));

	// list lambda --- ?
	Install(new Word("reverse-foreach",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=DataType::Word) {
			return inContext.Error(InvalidTypeErrorID::TosWp,lambda);
		}

		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,tvList);
		}

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=tvList.listPtr->size();
		if(n>0) {
			inContext.RS.emplace_back(0);
			for(int i=(int)(n-1); i>=0; i--) {
				TypedValue& tvCounter=ReadTOS(inContext.RS);
				if(tvCounter.dataType!=DataType::Int) {
					return inContext.Error(NoParamErrorID::RsBroken);
				}
				tvCounter.intValue=i;

				inContext.DS.emplace_back(tvList.listPtr->back());
				if(inContext.Exec(lambda)==false) {
					inContext.suppressError=true;
				}
				tvList.listPtr->pop_back();
			}
			Pop(inContext.RS); 
			inContext.suppressError=suppressErrorBackup;
		}
		NEXT;	
	}));

	// string --- list
	Install(new Word("make-literal-code-list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}

		const unsigned char *src=(const unsigned char *)tos.stringPtr.get()->c_str();
		TypedValue tvList=TypedValue(new List());
		auto listPtr=tvList.listPtr.get();
		for(int i=0; src[i]!='\0'; i++) {
			unsigned long long t=0;
			int j=0;
			do {
				t<<=8;
				t |= src[i+j];
				j++;
			} while((src[i+j] & 0xC0)==0x80);
			if(t<(unsigned long long)UINT_MAX) {
				listPtr->emplace_back((int)t);
			} else if(t<(unsigned long long)ULONG_MAX) {
				listPtr->emplace_back((long)t);
			} else {
				return inContext.Error(NoParamErrorID::CanNotConvertToInt);
			}
			i+=j-1;
		}
		
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// string --- list
	Install(new Word("str-to-list-byte",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}

		const char *src=tos.stringPtr.get()->c_str();
		TypedValue tvList=TypedValue(new List());
		auto listPtr=tvList.listPtr.get();
		for(int i=0; src[i]!='\0'; i++) {
			listPtr->emplace_back((int)src[i]);
		}
		
		inContext.DS.emplace_back(tvList);
		NEXT;	
	}));

	Install(new Word("printf",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		const size_t n=tos.listPtr->size();
		if(n<1) {
			return inContext.Error(NoParamErrorID::TosListNoElement);
		}
		TypedValue first=tos.listPtr->front();
		if(first.dataType!=DataType::String) {
			return inContext.Error(InvalidTypeErrorID::FirstElementString,first);
		}
		try {
			boost::format fmt(first.stringPtr->c_str());
			const int kNoIndent=0;
			for(size_t i=1; i<n; i++) {
				TypedValue tv=tos.listPtr->at(i);
				switch(tv.dataType) {
					case DataType::Int:		fmt=fmt%tv.intValue;	break;
					case DataType::Long:	fmt=fmt%tv.longValue;	break;
					case DataType::Float:	fmt=fmt%tv.floatValue;	break;
					case DataType::Double:	fmt=fmt%tv.doubleValue;	break;
					case DataType::Symbol: {
							bool found=false;
							TypedValue varValue
							  =inContext.GetLocalVarValueByDynamic(*tv.stringPtr.get(),
							  									   &found);
							if(found==false) { return false; }
							switch(varValue.dataType) {
								case DataType::Int:
								  	fmt=fmt%varValue.intValue;
								 	break;
								case DataType::Long:
									fmt=fmt%varValue.longValue;
								   	break;
								case DataType::Float:
								   	fmt=fmt%varValue.floatValue; 
								   	break;
								case DataType::Double:
									fmt=fmt%varValue.doubleValue;
									break;

								case DataType::Symbol:	// don't double-reference.
								case DataType::BigInt:
								default:
									fmt=fmt%varValue.GetValueString(kNoIndent).c_str();
							}
						}
						break;

					default:
						fmt=fmt%tv.GetValueString(kNoIndent).c_str();
				}
			}
			LockForConsoleOutput();
				std::cout << fmt;
			UnlockForConsoleOutput();
		} catch(...) {
			return inContext.Error(NoParamErrorID::FormatDataMismatch);
		}
		NEXT;
	}));

	// X L --- B
	Install(new Word("has?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue second=Pop(inContext.DS);
		inContext.DS.emplace_back(hasElement(tos,second));
		NEXT;
	}));

	// X L --- X L B
	Install(new Word("@has?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tos);
		}
		TypedValue& second=ReadSecond(inContext.DS);
		inContext.DS.emplace_back(hasElement(tos,second));
		NEXT;
	}));
	
	// L X --- B
	Install(new Word("in?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}
		inContext.DS.emplace_back(hasElement(second,tos));
		NEXT;
	}));

	// L X --- L X B
	Install(new Word("@in?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		TypedValue& second=ReadSecond(inContext.DS);
		if(second.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::SecondList,second);
		}
		inContext.DS.emplace_back(hasElement(second,tos));
		NEXT;
	}));

	// SS: --- I
	// equivalent to: depth >r
	Install(new Word("(<",WORD_FUNC {
		inContext.SS.emplace_back((int)inContext.DS.size());
		NEXT;
	}));

	// SS: I ---
	Install(new Word(">)",WORD_FUNC {
		if(inContext.SS.size()<1) { return inContext.Error(NoParamErrorID::SsBroken); }
		TypedValue tvDepth=Pop(inContext.SS);
		if(tvDepth.dataType!=DataType::Int) {
			return inContext.Error(NoParamErrorID::SsBroken);
		}
		List *result=new List();
		int n=(int)inContext.DS.size();
		for(int i=tvDepth.intValue; i<n; i++) {
			result->emplace_front(Pop(inContext.DS));
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));
}

static bool expandBody(Context& inContext,TypedValue& inTos) {
	switch(inTos.dataType) {
		case DataType::List: {
				const size_t n=inTos.listPtr->size();
				for(size_t i=0; i<n; i++) {
					inContext.DS.emplace_back(inTos.listPtr->at(i));
				}
			}
			break;
		case DataType::Array: {
				const size_t n=inTos.arrayPtr->length;
				for(size_t i=0; i<n; i++) {
					inContext.DS.emplace_back(inTos.arrayPtr->data[i]);
				}
			}
			break;
		default:
			return false;
	}
	return true;
}

static bool hasElement(TypedValue& inTvList,TypedValue& inElement) {
	assert(inTvList.dataType==DataType::List);
	const size_t n=inTvList.listPtr->size();
	bool found=false;
	for(size_t i=0; i<n; i++) {
		if(inTvList.listPtr->at(i)==inElement) {
			found=true;
			break;
		}
	}
	return found;
}

