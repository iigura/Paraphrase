#include <stdio.h>
#include <string.h>

#include <algorithm>

#include <boost/format.hpp>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "thread.h"

static TypedValue fullClone(TypedValue& inTV);
static void showStack(Stack& inStack,const char *inStackName);
static void printLine(int inWidth);
static void printCenteringStr(std::string inString,int inWidth);

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
		TypedValue tvFullClone=fullClone(tos);
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
		if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
		}
		const int n=tos.intValue;
		if(inContext.DS.size()<n) {
			return inContext.Error(E_DEPTH_INDEX_OUT_OF_RANGE,
								   (int)inContext.DS.size(),n);
		}
		if(n<1) { return inContext.Error(E_TOS_POSITIVE_INT,n); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-n]);
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
		TypedValue dsTos=Pop(inContext.DS);
		inContext.RS.emplace_back(dsTos);
		NEXT;
	}));

	// equivalent to ">r >r"
	Install(new Word(">r>r",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv1=Pop(inContext.DS); inContext.RS.emplace_back(tv1);
		TypedValue tv2=Pop(inContext.DS); inContext.RS.emplace_back(tv2);
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
		TypedValue rsTos=Pop(inContext.RS);
		inContext.DS.emplace_back(rsTos);
		NEXT;
	}));

	Install(new Word("@r>",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.DS.emplace_back(inContext.RS.back());
		NEXT;
	}));

	Install(new Word("show",WORD_FUNC {
		showStack(inContext.DS,"DS");
		NEXT;
	}));

	Install(new Word("show-rs",WORD_FUNC {
		showStack(inContext.RS,"RS");
		NEXT;
	}));
}

void ShowStack(Stack& inStack,const char *inStackName) {
	showStack(inStack,inStackName);
}
static void showStack(Stack& inStack,const char *inStackName) {
	int maxLength=-1;
	std::string emptyMessageStr=(boost::format("(%s is empty)")%inStackName).str();
	if(inStack.size()<1) {
		maxLength=(int)emptyMessageStr.size()+2; // +2 for blanks(spaces) for both end.
	} else {
		const int nameLen=(int)strlen(inStackName);
		std::string spc(" ");
		for(int i=0; i<nameLen; i++) {
			spc+=" ";
		}
		std::string spc2("");
		for(int i=0; i<nameLen-3; i++) {
			spc+=" ";
		}
		std::vector<std::string> v;
		const size_t n=inStack.size();
		for(size_t i=0; i<n; i++) {
			std::string s=inStack[i].GetTypeStr()+" "+inStack[i].GetValueString(-1);
			v.push_back(s);
			maxLength=std::max(maxLength,(int)s.size());
		}
		maxLength+=2;
		printf("%s",spc.c_str());
		fputs("   +",stdout); printLine(maxLength); fputs("+   \n",stdout);
		printf("%s",spc2.c_str()); fputs("TOS-->|",stdout);
			printCenteringStr(v[v.size()-1],maxLength);
		fputs("|   \n",stdout);
		for(int i=(int)v.size()-2; i>=0; i--) {
			printf("%s",spc.c_str()); fputs("   |",stdout);
				printCenteringStr(v[i],maxLength);
			fputs("|   \n",stdout);
		}
	}
	printf("%s:",inStackName);
	printLine(maxLength+8);	// +8 for '   |' ... '|   '
	putchar('\n');
	if(inStack.size()<1) {
		printf("    %s    \n",emptyMessageStr.c_str());
	}
}
static void printLine(int inWidth) {
	for(int i=0; i<inWidth; i++) {
		putchar('-');
	}
}
static void printCenteringStr(std::string inString,int inWidth) {
	int leftGap=(inWidth-(int)inString.size())/2;
	for(int i=0; i<leftGap; i++) {
		putchar(' ');
	}
	printf("%s",inString.c_str());
	int rightGap=inWidth-leftGap-(int)inString.size();
	for(int i=0; i<rightGap; i++) {
		putchar(' ');
	}
}

static TypedValue fullClone(TypedValue& inTV) {
	switch(inTV.dataType) {
		case kTypeString:
		case kTypeSymbol:
			return TypedValue(*inTV.stringPtr);
		case kTypeArray: {
				Array<TypedValue> *srcPtr=inTV.arrayPtr.get();
				Lock(srcPtr->mutex);
					const int n=srcPtr->length;
					TypedValue *dataBody=new TypedValue[n];
					for(int i=0; i<n; i++) {
						dataBody[i]=fullClone(srcPtr->data[i]);
					}
				Unlock(srcPtr->mutex);
				Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
				return TypedValue(arrayPtr);
			}
		case kTypeList: {
				std::deque<TypedValue> *srcPtr=inTV.listPtr.get();
				std::deque<TypedValue> *destPtr=new std::deque<TypedValue>();
				const int n=(int)srcPtr->size();
				for(int i=0; i<n; i++) {
					destPtr->emplace_back(fullClone(srcPtr->at(i)));
				}
				return TypedValue(destPtr);
			}
		default:
			return TypedValue(inTV);
	}
}

