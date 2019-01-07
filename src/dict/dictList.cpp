#include <climits>

#include <boost/format.hpp>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

void InitDict_List() {
	Install(new Word("(",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }
		inContext.BeginListBlock();
		NEXT;
	}));

	Install(new Word(")",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }

		TypedValue tvList=inContext.GetList();
		if(inContext.EndListBlock()==false) { return false; }

		switch(inContext.ExecutionThreshold) {
			case kInterpretLevel:
				inContext.DS.emplace_back(tvList);
				break;
			case kCompileLevel:		
				if(inContext.newWord==NULL) { goto newWord_is_NULL; }
				inContext.Compile(std::string("_lit"));
				inContext.Compile(tvList);
				break;
			case kSymbolLevel:
				if(inContext.newWord==NULL) { goto newWord_is_NULL; }
				inContext.Compile(tvList);
				break;
			default:
				inContext.Error(E_SYSTEM_ERROR);
				exit(-1);
		}
		NEXT;

newWord_is_NULL:
		fprintf(stderr,"SYSTEM ERROR at ')'. newWord should not be NULL.\n");
		exit(-1);
	}));

	Install(new Word("()",WordLevel::Level2,WORD_FUNC {
		if( inContext.IsInComment() ) { NEXT; }

		if(inContext.ExecutionThreshold!=kInterpretLevel) {
			if(inContext.newWord->type!=WordType::List) {
				inContext.Compile(std::string("_create-empty-list"));
			}
		} else {
			inContext.DS.emplace_back(new List());
		}
		NEXT;
	}));
	Install(new Word("_create-empty-list",WORD_FUNC {
		inContext.DS.emplace_back(new List());
		NEXT;
	}));

	Install(new Word("empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()==0);
		NEXT;
	}));

	Install(new Word("@empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()==0);
		NEXT;
	}));

	Install(new Word("not-empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()!=0);
		NEXT;
	}));

	Install(new Word("@not-empty-list?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->size()!=0);
		NEXT;
	}));

	// list lambda --- list
	Install(new Word("map",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_WP,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,second);
		}

		auto src=second.listPtr.get();

		TypedValue result=TypedValue(new List());
		auto dest=result.listPtr.get();

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=src->size();
		for(size_t i=0; i<n; i++) {
			inContext.DS.emplace_back(src->at(i));
			if(inContext.Exec(tos)==false) {
				inContext.suppressError=true;
			}
			TypedValue t=Pop(inContext.DS);
			dest->emplace_back(t);
		}
		inContext.DS.emplace_back(result);
		inContext.suppressError=suppressErrorBackup;
		NEXT;
	}));

	// list lambda --- list
	Install(new Word("sort",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_WP,lambda);
		}

		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,second);
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
				if(t.dataType!=kTypeBool) {
					return false;
				}
				return t.boolValue;
		});
		inContext.suppressError=suppressErrorBackup;
		NEXT;	
	}));

	// list lambda --- ?
	Install(new Word("apply",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue lambda=Pop(inContext.DS);
		if(lambda.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_WP,lambda);
		}

		TypedValue& tvList=ReadTOS(inContext.DS);
		if(tvList.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,tvList);
		}

		bool suppressErrorBackup=inContext.suppressError;
		const size_t n=tvList.listPtr->size();
		for(size_t i=0; i<n; i++) {
			inContext.DS.emplace_back(tvList.listPtr->front());
			if(inContext.Exec(lambda)==false) {
				inContext.suppressError=true;
			}
			tvList.listPtr->pop_front();
		}
		inContext.suppressError=suppressErrorBackup;
		NEXT;	
	}));

	// list x --- list
	Install(new Word("append",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(inContext.DS);
		TypedValue& tvList=ReadTOS(inContext.DS);
		if(tvList.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,tvList);
		}

		tvList.listPtr->push_back(tos);
		NEXT;	
	}));

	// x list --- list
	// equivalent to 'swap append'.
	Install(new Word("add",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tvList);
		}
		TypedValue tv=Pop(inContext.DS);
		tvList.listPtr->push_back(tv);
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	Install(new Word("car",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		NEXT;
	}));

	Install(new Word("cdr",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		
		const List *src=tos.listPtr.get();
		List *cdr=new List();
		cdr->assign(src->begin()+1,src->end());
		inContext.DS.emplace_back(cdr);
		NEXT;
	}));

	// list --- list x
	Install(new Word("last",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->back());
		NEXT;
	}));

	// list --- list x
	Install(new Word("pop-front",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		inContext.DS.emplace_back(tos.listPtr->front());
		tos.listPtr->pop_front();
		NEXT;
	}));

	// (x -- list)
	Install(new Word("en-list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		TypedValue tvList=TypedValue(new List());
		tvList.listPtr->push_back(tos);
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// string --- list
	Install(new Word("make-literal-code-list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
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
			} else { return inContext.Error(E_CAN_NOT_CONVERT_TO_INT); }
			i+=j-1;
		}
		
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// string --- list
	Install(new Word("str-to-list-byte",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_TOS_STRING,tos);
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

	// (list -- e1 e2 ... eN)
	Install(new Word("de-list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		const size_t n=tos.listPtr->size();
		for(size_t i=0; i<n; i++) {
			inContext.DS.emplace_back(tos.listPtr->front());
			tos.listPtr->pop_front();
		}
		NEXT;
	}));
	
	// ( a b -- (a b) )
	Install(new Word("tuple",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
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
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,second);
		}
		List *list=second.listPtr.get();
		int n=tos.intValue;
		if(n<0 || n>=(int)list->size()) {
			return inContext.Error(E_LIST_INDEX_OUT_OF_RANGE,(int)list->size(),n);
		}
		List *latter=new List();
		latter->assign(list->begin()+n,list->end());
		list->erase(list->begin()+n,list->end());
		inContext.DS.emplace_back(latter);
		NEXT;
	}));

	// list1 list2 --- list
	Install(new Word("concat",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,second);
		}

		second.listPtr->insert(second.listPtr->end(),
							   tos.listPtr->begin(),tos.listPtr->end());
		NEXT;
	}));

	// list start length --- list1 list2
	Install(new Word("slice",WORD_FUNC {
		if(inContext.DS.size()<3) { return inContext.Error(E_DS_AT_LEAST_3); }
		TypedValue tvLength=Pop(inContext.DS);
		if(tvLength.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tvLength);
		}
		const int length=tvLength.intValue;
		if(length<0) { return inContext.Error(E_TOS_NON_NEGATIVE,length); }

		TypedValue tvStart=Pop(inContext.DS);
		if(tvStart.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_SECOND_INT,tvStart);
		}
		const int start=tvStart.intValue;
		if(start<0) { return inContext.Error(E_SECOND_NON_NEGATIVE,start); }

		TypedValue& tvList=ReadTOS(inContext.DS);
		if(tvList.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_THIRD_LIST,tvList);
		}

		List *list=tvList.listPtr.get();
		if((int)list->size()<=start) {
			return inContext.Error(E_LIST_START_INDEX_OUT_OF_RANGE,
								   (int)list->size(),start);
		}
		if((int)list->size()<start+length) {
			return inContext.Error(E_LIST_LENGTH_INDEX_OUT_OF_RANGE,
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
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }	

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_SECOND_LIST,second);
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

	Install(new Word("printf",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeList) {
			return inContext.Error_InvalidType(E_TOS_LIST,tos);
		}
		const size_t n=tos.listPtr->size();
		if(n<1) { return inContext.Error(E_TOS_LIST_NO_ELEMENT); }
		TypedValue first=tos.listPtr->front();
		if(first.dataType!=kTypeString) {
			return inContext.Error_InvalidType(E_FIRST_ELEMENT_STRING,first);
		}
		try {
			boost::format fmt(first.stringPtr->c_str());
			tos.listPtr->pop_front();
			const int kNoIndent=0;
			for(size_t i=0; i<n-1; i++) {
				TypedValue tv=tos.listPtr->front();
				switch(tv.dataType) {
					case kTypeInt:		fmt=fmt%tv.intValue;	break;
					case kTypeLong:		fmt=fmt%tv.longValue;	break;
					case kTypeFloat:	fmt=fmt%tv.floatValue;	break;
					case kTypeDouble:	fmt=fmt%tv.doubleValue;	break;
					case kTypeBigFloat:
					case kTypeBigInt:
					default:
						fmt=fmt%tv.GetValueString(kNoIndent).c_str();
				}
				tos.listPtr->pop_front();
			}
			std::cout << fmt;
		} catch(...) {
			return inContext.Error(E_FORMAT_DATA_MISMATCH);
		}
		NEXT;
	}));
}

