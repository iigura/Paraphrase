#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <climits>
#include <cfloat>
#include <cmath>

#include <boost/multiprecision/cpp_int.hpp>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

const BigInt kBigInt_FLT_MAX(FLT_MAX);
const BigInt kBigInt_Minus_FLT_MAX(-FLT_MAX);
const BigInt kBigInt_DBL_MAX(DBL_MAX);
const BigInt kBigInt_Minus_DBL_MAX(-DBL_MAX);

static void listToArray(Context& inContext,TypedValue& inTos);
static void assocToArray(Context& inContext,TypedValue& inTos);
static void stringToArray(Context& inContext,TypedValue& inTos);
static bool queryTosType(Context& inContext,DataType inDataType);
static bool lookAheadQueryTosType(Context& inContext,DataType inDataType);

void InitDict_Type() {
	Install(new Word(">int",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Address:
				// fall through
				break; // do nothing
			case DataType::Float:
				if(tos.floatValue<INT_MIN || INT_MAX<tos.floatValue) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToIntDueToOverflow);
				}
				tos.intValue=(int)tos.floatValue;
				break;
			case DataType::Double:
				if(tos.doubleValue<INT_MIN || INT_MAX<tos.doubleValue) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToIntDueToOverflow);
				}
				tos.intValue=(int)tos.doubleValue;
				break;
			case DataType::Long:
				if(tos.longValue<(long)INT_MIN || (long)INT_MAX<tos.longValue) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToIntDueToOverflow);
				}
				tos.intValue=(int)tos.longValue;
				break;
			case DataType::BigInt:
				if(*tos.bigIntPtr<INT_MIN || INT_MAX<*tos.bigIntPtr) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToIntDueToOverflow);
				}
				tos.intValue=static_cast<int>(*tos.bigIntPtr);
				break;
			case DataType::BigFloat:
				if(*tos.bigFloatPtr<INT_MIN || INT_MAX<*tos.bigFloatPtr) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToIntDueToOverflow);
				}
				tos.intValue=static_cast<int>(*tos.bigFloatPtr);
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID::TosNumber,tv);
					}
				}
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		tos.dataType=DataType::Int;
		NEXT;
	}));

	Install(new Word(">int?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		bool result=false;
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Address:
				result=true;
				break;
			case DataType::Long:
				result = (long)INT_MIN<=tos.longValue && tos.longValue<=(long)INT_MAX;
				break;
			case DataType::Float:
				result = INT_MIN<=tos.floatValue && tos.floatValue<=INT_MAX;
				break;
			case DataType::Double:
				result = INT_MIN<=tos.doubleValue && tos.doubleValue<=INT_MAX;
				break;
			case DataType::BigInt:
				result = INT_MIN<=*tos.bigIntPtr && *tos.bigIntPtr<=INT_MAX;
				break;				
			case DataType::BigFloat:
				result = INT_MIN<=*tos.bigFloatPtr && *tos.bigFloatPtr<=INT_MAX;
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						result=false;
					}
				}
			default:
					result=false;
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word(">long",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Address:
				// fall through
				tos.longValue=(long)tos.intValue;
				break;
			case DataType::Float:
				if(tos.floatValue<LONG_MIN || LONG_MAX<tos.floatValue) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToLongDueToOverflow);
				}
				tos.longValue=(long)tos.floatValue;
				break;
			case DataType::Double:
				if(tos.doubleValue<LONG_MIN || LONG_MAX<tos.doubleValue) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToLongDueToOverflow);
				}
				tos.longValue=(long)tos.doubleValue;
				break;
			case DataType::BigInt:
				if(*tos.bigIntPtr<LONG_MIN || LONG_MAX<*tos.bigIntPtr) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToLongDueToOverflow);
				}
				tos.longValue=static_cast<long>(*tos.bigIntPtr);
				break;
			case DataType::BigFloat:
				if(*tos.bigFloatPtr<LONG_MIN || LONG_MAX<*tos.bigFloatPtr) {
					return inContext.Error(NoParamErrorID
										   ::CanNotConvertToLongDueToOverflow);
				}
				tos.longValue=static_cast<long>(*tos.bigFloatPtr);
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID::TosNumber,tv);
					}
				}
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		tos.dataType=DataType::Long;
		NEXT;
	}));

	Install(new Word(">long?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		bool result=false;
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Long:
			case DataType::Address:
				result=true;
				break;
			case DataType::BigInt:
				result = LONG_MIN<=*tos.bigIntPtr && *tos.bigIntPtr<=LONG_MAX;
				break;
			case DataType::Float:
				result = LONG_MIN<=tos.floatValue && tos.floatValue<=LONG_MAX;
				break;
			case DataType::Double:
				result = LONG_MIN<=tos.doubleValue && tos.doubleValue<=LONG_MAX;
				break;
			case DataType::BigFloat:
				result = LONG_MIN<=*tos.bigFloatPtr && *tos.bigFloatPtr<=LONG_MAX;
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						result=false;
					}
				}
				break;
			default:
				result=false;
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word(">big-int",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		BigInt bigInt;
redo:
		switch(tos.dataType) {
			case DataType::Int:    bigInt=tos.intValue; 	break;
			case DataType::Long:   bigInt=tos.longValue; 	break;
			case DataType::Float:  bigInt=static_cast<BigInt>(tos.floatValue);  break;
			case DataType::Double: bigInt=static_cast<BigInt>(tos.doubleValue); break;
			case DataType::BigInt: /* do nothing */ break;
			case DataType::BigFloat:
				bigInt=static_cast<BigInt>(*tos.bigFloatPtr);
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID
											   ::TosNumberOrString,tv);
					}
				}
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumberOrString,tos);
		}
		if(tos.dataType!=DataType::BigInt) {
			inContext.DS.emplace_back(bigInt);
		} else {
			inContext.DS.emplace_back(*tos.bigIntPtr);
		}
		NEXT;
	}));

	Install(new Word(">float",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Address:
				tos.floatValue=(float)tos.intValue;
				tos.dataType=DataType::Float;
				break;
			case DataType::Long:
				tos.floatValue=(float)tos.longValue;
				tos.dataType=DataType::Float;
				break;
			case DataType::BigInt: {
					if(*tos.bigIntPtr<kBigInt_Minus_FLT_MAX
					  || kBigInt_FLT_MAX<*tos.bigIntPtr) {
						return inContext.Error(
								NoParamErrorID::CanNotConvertToFloatDueToOverflow);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigIntPtr;
					tos.floatValue=f;
					tos.dataType=DataType::Float;
				}
				break;
			case DataType::Float:
				// do nothing
				break;
			case DataType::Double:
				if(tos.doubleValue<-FLT_MAX || FLT_MAX<tos.doubleValue) {
					return inContext.Error(
							NoParamErrorID::CanNotConvertToFloatDueToOverflow);
				}
				tos.floatValue=(float)tos.doubleValue;
				tos.dataType=DataType::Float;
				break;
			case DataType::BigFloat: {
					if(*tos.bigFloatPtr>FLT_MAX || *tos.bigFloatPtr<-FLT_MAX) {
						return inContext.Error(
								NoParamErrorID::CanNotConvertToFloatDueToOverflow);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigFloatPtr;
					tos.floatValue=f;
					tos.dataType=DataType::Float;
				}
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID
											   ::TosNumberOrString,tv);
					}
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	}));

	Install(new Word(">float?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		bool result=false;
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Long:
			case DataType::Float:
			case DataType::Address:
				result=true;
				break;
			case DataType::BigInt:
				result = kBigInt_Minus_FLT_MAX<=*tos.bigIntPtr
						 && *tos.bigIntPtr<=kBigInt_FLT_MAX;
				break;
			case DataType::Double:
				result = -FLT_MAX<=tos.doubleValue && tos.doubleValue<=FLT_MAX;
				break;
			case DataType::BigFloat: 
				result = -FLT_MAX<=*tos.bigFloatPtr && *tos.bigFloatPtr<=FLT_MAX;
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						result=false;
					}
				}
				break;
			default:
				result=false;
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word(">double",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
redo:
		switch(tos.dataType) {
			case DataType::Int:
				tos.doubleValue=(double)tos.intValue;
				tos.dataType=DataType::Double;
				break;
			case DataType::Long:
				tos.doubleValue=(double)tos.longValue;
				tos.dataType=DataType::Double;
				break;
			case DataType::BigInt: {
					if(*tos.bigIntPtr>kBigInt_DBL_MAX
					  || *tos.bigIntPtr<kBigInt_Minus_DBL_MAX) {
						return inContext.Error(
								NoParamErrorID::CanNotConvertToDoubleDueToOverflow);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigIntPtr;
					tos.doubleValue=t;
					tos.dataType=DataType::Double;
				}
				break;
			case DataType::Float:
				tos.doubleValue=(double)tos.floatValue;
				tos.dataType=DataType::Double;
				break;
			case DataType::Double:
				// do nothing
				break;
			case DataType::BigFloat: {
					if(*tos.bigFloatPtr>DBL_MAX || *tos.bigFloatPtr<-DBL_MAX) {
						return inContext.Error(
								NoParamErrorID::CanNotConvertToDoubleDueToOverflow);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigFloatPtr;
					tos.doubleValue=t;
					tos.dataType=DataType::Double;
				}
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID
											   ::TosNumberOrString,tv);
					}
				}
				break;
		default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	}));

	Install(new Word(">double?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		bool result=false;
redo:
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Long:
			case DataType::Address:
			case DataType::Float:
				result=true;
				break;
			case DataType::BigInt:
				result = kBigInt_Minus_DBL_MAX<=*tos.bigIntPtr
						 && *tos.bigIntPtr<=kBigInt_DBL_MAX;
				break;
			case DataType::BigFloat:
				result = -DBL_MAX<=*tos.bigFloatPtr && *tos.bigFloatPtr<=DBL_MAX;
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						result=false;
					}
				}
				break;
			default:
				result=false;
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word(">big-float",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
redo:
		BigFloat bigFloat;
		switch(tos.dataType) {
			case DataType::Int: 	bigFloat=tos.intValue;		break;
			case DataType::Long: 	bigFloat=tos.longValue;		break;
			case DataType::Float:  	bigFloat=tos.floatValue;	break;
			case DataType::Double: 	bigFloat=tos.doubleValue;	break;
			case DataType::BigInt:
				bigFloat=static_cast<BigFloat>(*tos.bigIntPtr);
				break;
			case DataType::BigFloat:
			   	/* do nothing */
				break;
			case DataType::String: {
					TypedValue tv=GetTypedValue(*tos.stringPtr);
					if( tv.IsNumber() ) {
						tos=tv;
						goto redo;
					} else {
						return inContext.Error(InvalidTypeErrorID
											   ::TosNumberOrString,tv);
					}
				}
				break;
			default:
	  			return inContext.Error(InvalidTypeErrorID::TosNumberOrString,tos);
		}
		if(tos.dataType!=DataType::BigFloat) {
			inContext.DS.emplace_back(bigFloat);
		} else {
			inContext.DS.emplace_back(*tos.bigFloatPtr);
		}
		NEXT;
	}));

	Install(new Word(">bool",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:		tos.boolValue = tos.intValue!=0;			break;
			case DataType::Long:	tos.boolValue = tos.longValue!=0;			break;
			case DataType::Float:	tos.boolValue = tos.floatValue!=0.0f;		break;
			case DataType::Double:	tos.boolValue = tos.doubleValue!=0.0;		break;
			case DataType::BigInt: 	tos=TypedValue((*tos.bigIntPtr)!=0); 		break;
			case DataType::BigFloat:tos=TypedValue((*tos.bigFloatPtr)!=0.0); 	break;
			case DataType::Bool:	/* do nothing */							break;
			case DataType::String: 	tos=TypedValue(tos.stringPtr->length()>0);	break;
			case DataType::Address:	tos.boolValue = tos.intValue!=0;			break;
			case DataType::Invalid:	tos.boolValue=false;						break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosCanNotConvertToBool,tos);
		}
		tos.dataType=DataType::Bool;
		NEXT;
	}));

	// X --- s
	Install(new Word(">string",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if( tos.IsCollection() ) {
			return inContext.Error(InvalidTypeErrorID::TosCanNotConvertToString,tos);
		}
		inContext.DS.emplace_back(tos.GetValueString());
		NEXT;
	}));

	Install(new Word(">hex",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.IsInteger()==false) {
			return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
		}

		if(tos.dataType==DataType::BigInt) {
			const std::string hexStr=tos.bigIntPtr->str(0,std::ios_base::hex);
			inContext.DS.emplace_back(hexStr);
		} else {
			// int or long
			std::stringstream ss;
			ss << std::hex << tos.intValue;
			std::string hexStr=ss.str();
			transform(hexStr.begin(),hexStr.end(),hexStr.begin(),toupper);
			inContext.DS.emplace_back(hexStr);
		}
		NEXT;
	}));

	// S --- symbol
	Install(new Word(">symbol",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if( tos.IsCollection() ) {
			return inContext.Error(NoParamErrorID::CanNotConvertToSymbol);
		}
		std::string s=tos.GetValueString();
		char c=s[0];
		if(('A'<=c && c<='Z') || ('a'<=c && c<='z') || c=='_') {
			inContext.DS.emplace_back(s,DataType::Symbol);
		} else {
			return inContext.Error(NoParamErrorID::CanNotConvertToSymbol);
		}
		NEXT;
	}));

	// tuple-list --- R
	Install(new Word(">assoc",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tvList=Pop(inContext.DS);
		if(tvList.dataType!=DataType::List) {
			return inContext.Error(InvalidTypeErrorID::TosList,tvList);
		}
		TypedValue tvAssoc=TypedValue(new KeyValue());
		const size_t n=tvList.listPtr->size();
		if(n>0) {
			for(size_t i=0; i<n; i++) {
				TypedValue tvTuple=tvList.listPtr->at(i);
				if(tvTuple.dataType!=DataType::List || tvTuple.listPtr->size()!=2) {
					return inContext.Error(InvalidTypeErrorID::ElementShouldBeTuple,
										   tvTuple);
				}
				(*(tvAssoc.kvPtr))[tvTuple.listPtr->at(0)]=tvTuple.listPtr->at(1);
			}
		}
		inContext.DS.emplace_back(tvAssoc);
		NEXT;
	}));

	Install(new Word(">array",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);

		switch(tos.dataType) {
			case DataType::Array:	/* do nothing */				break;
			case DataType::List: 	listToArray(inContext,tos);		break;
			case DataType::KV:		assocToArray(inContext,tos);	break;
			case DataType::String:	stringToArray(inContext,tos);	break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosString,tos);
		}
		NEXT;
	}));

	Install(new Word(">list",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		TypedValue tvList=TypedValue(new List());
		if( tos.IsCollection() ) {
			switch(tos.dataType) {
				case DataType::List:
					tvList=tos;
					break;
				case DataType::Array:
					for(int i=0; i<tos.arrayPtr->length; i++) {
						tvList.listPtr->push_back(tos.arrayPtr->data[i]);
					}
					break;
				case DataType::KV:
					for(auto const &pair : *tos.kvPtr) {
						TypedValue tuple=TypedValue(new List());
						tuple.listPtr->push_back(pair.first);
						tuple.listPtr->push_back(pair.second);
						tvList.listPtr->push_back(tuple);
					}
					break;
				default:
					fprintf(stderr,"SYSTEM ERROR (>list).\n");
					exit(-1);
			}
		} else {
			tvList.listPtr->push_back(tos);
		}
		inContext.DS.emplace_back(tvList);
		NEXT;
	}));

	// { string | symbol | wordPtr } -- wordPtr
	Install(new Word(">word",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue& tosLookAhead=ReadTOS(inContext.DS);
		if( tosLookAhead.HasWordPtr(NULL) ) {
			// do nothing
			goto next;
		} else {
			TypedValue tos=Pop(inContext.DS);
			const Word *word;
			if(tos.dataType==DataType::String || tos.dataType==DataType::Symbol) {
				word=GetWordPtr(*tos.stringPtr);
				if(word==NULL) {
					return inContext.Error(ErrorIdWithString::CanNotFindTheWord,
										   *tos.stringPtr);
				}
			} else {
				return inContext.Error(InvalidTypeErrorID::TosString,tos);
			}
			inContext.DS.emplace_back(DataType::Word,word);
		}
next:
		NEXT;
	}));

	Install(new Word(">address",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::Int && tos.dataType!=DataType::Address) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}
		tos.dataType=DataType::Address;
		NEXT;
	}));

	Install(new Word("bool?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Bool)==false) { return false; }
		NEXT;
	}));
	Install(new Word("int?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Int)==false) { return false; }
		NEXT;
	}));
	Install(new Word("long?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Long)==false) { return false; }
		NEXT;
	}));
	Install(new Word("big-int?",WORD_FUNC {
		if(queryTosType(inContext,DataType::BigInt)==false) { return false; }
		NEXT;
	}));
	Install(new Word("float?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Float)==false) { return false; }
		NEXT;
	}));
	Install(new Word("double?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Double)==false) { return false; }
		NEXT;
	}));
	Install(new Word("big-float?",WORD_FUNC {
		if(queryTosType(inContext,DataType::BigFloat)==false) { return false; }
		NEXT;
	}));
	Install(new Word("string?",WORD_FUNC {
		if(queryTosType(inContext,DataType::String)==false) { return false; }
		NEXT;
	}));

	Install(new Word("array?",WORD_FUNC {
		if(queryTosType(inContext,DataType::Array)==false) { return false; }
		NEXT;
	}));
	Install(new Word("list?",WORD_FUNC {
		if(queryTosType(inContext,DataType::List)==false) { return false; }
		NEXT;
	}));
	Install(new Word("assoc?",WORD_FUNC {
		if(queryTosType(inContext,DataType::KV)==false) { return false; }
		NEXT;
	}));

	Install(new Word("@array?",WORD_FUNC {
		if(lookAheadQueryTosType(inContext,DataType::Array)==false) { return false; }
		NEXT;
	}));
	Install(new Word("@list?",WORD_FUNC {
		if(lookAheadQueryTosType(inContext,DataType::List)==false) { return false; }
		NEXT;
	}));
	Install(new Word("@assoc?",WORD_FUNC {
		if(lookAheadQueryTosType(inContext,DataType::KV)==false) { return false; }
		NEXT;
	}));

	// X X --- B
	Install(new Word("is?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		bool result=false;
		switch(tos.dataType) {
			case DataType::Array:
				if(second.dataType!=DataType::Array) { break; }
				result = tos.arrayPtr.get()==second.arrayPtr.get();
				break;
			case DataType::List:
				if(second.dataType!=DataType::List) { break; }
				result = tos.listPtr.get()==second.listPtr.get();
				break;
			case DataType::KV:
				if(second.dataType!=DataType::KV) { break; }
				result = tos.kvPtr.get()==second.kvPtr.get();
				break;
			case DataType::String:
				if(second.dataType!=DataType::String) { break; }
				result = tos.stringPtr.get()==second.stringPtr.get();
				break;
			case DataType::Symbol:
				if(second.dataType!=DataType::Symbol) { break; }
				result = tos.stringPtr.get()==second.stringPtr.get();
				break;
			case DataType::DirectWord:
				if(second.dataType!=DataType::DirectWord) { break; }
				result = tos.wordPtr == second.wordPtr;
				break;
			case DataType::NewWord:
				if(second.dataType!=DataType::NewWord) { break; }
				result = tos.wordPtr == second.wordPtr;
				break;
			case DataType::Word:
				if(second.dataType!=DataType::Word) { break; }
				result = tos.wordPtr == second.wordPtr;
				break;
			case DataType::IP:
				if(second.dataType!=DataType::IP) { break; }
				result = tos.ipValue == second.ipValue;
				break;
			case DataType::ParamDest:	
				if(second.dataType!=DataType::ParamDest) { break; }
				result = tos.ipValue == second.ipValue;
				break;
			case DataType::File:
				if(second.dataType!=DataType::File) { break; }
				result = tos.filePtr.get() == second.filePtr.get();
				break;
			case DataType::BigInt:
				if(second.dataType!=DataType::BigInt) { break; }
				result = tos.bigIntPtr == second.bigIntPtr;
				break;
			case DataType::BigFloat:
				if(second.dataType!=DataType::BigFloat) { break; }
				result = tos.bigFloatPtr == second.bigFloatPtr;
				break;
			case DataType::StdCode:
				if(second.dataType!=DataType::StdCode) { break; }
				result = tos.stdCodePtr == second.stdCodePtr;
				break;
			case DataType::Bool:
				result = tos.boolValue == second.boolValue;
				break;
			case DataType::Int:
				result = tos.intValue == second.intValue;
				break;
			case DataType::Long:
				result = tos.longValue == second.longValue;
				break;
			case DataType::Float:
				result = tos.floatValue == second.floatValue;
				break;
			case DataType::Double:
				result =  tos.doubleValue == second.doubleValue;
				break;
			default:
				fprintf(stderr,"SYSTEM ERROR (at the word 'is')\n");
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word("@type-name",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsIsEmpty);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back(tos.GetTypeStr());
		NEXT;
	}));

	Install(new Word("type-name",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(tos.GetTypeStr());
		NEXT;
	}));

	Install(new Word("@type>",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		inContext.DS.emplace_back((long)tos.dataType);
		NEXT;
	}));

	// X1 J --- X2
	Install(new Word(">type",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		DataType dataType;
		switch(tos.dataType) {
			case DataType::Int: 	dataType=(DataType)tos.intValue; 	break;
			case DataType::Long:	dataType=(DataType)tos.longValue;	break;
			case DataType::BigInt:
				if(*tos.bigIntPtr<0 || *tos.bigIntPtr>LONG_MAX) {
					return inContext.Error(NoParamErrorID::InvalidValue);
				}
				dataType=(DataType)((long)(*tos.bigIntPtr));
				break;
			default:
				return inContext.Error(NoParamErrorID::InvalidValue);
		}
		if(IsValidDataTypeValue(dataType)==false) {
			return inContext.Error(NoParamErrorID::InvalidValue);
		}

		TypedValue& second=ReadTOS(inContext.DS);
		second.dataType=dataType;

		NEXT;
	}));

	Install(new Word("bool-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Bool);
		NEXT;
	}));

	Install(new Word("int-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Int);
		NEXT;
	}));

	Install(new Word("long-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Long);
		NEXT;
	}));

	Install(new Word("float-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Float);
		NEXT;
	}));

	Install(new Word("double-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Double);
		NEXT;
	}));

	Install(new Word("word-type",WORD_FUNC {
		inContext.DS.emplace_back((long)DataType::Word);
		NEXT;
	}));
}

static void listToArray(Context& inContext,TypedValue& inTos) {
	const int n=(int)inTos.listPtr->size();
	TypedValue *dataBody=new TypedValue[n];
	Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
	for(int i=0; i<n; i++) {
		dataBody[i]=inTos.listPtr->at(i);
	}
	inContext.DS.emplace_back(arrayPtr);
}

static void assocToArray(Context& inContext,TypedValue& inTos) {
	const int n=(int)inTos.kvPtr->size();
	TypedValue *dataBody=new TypedValue[n];
	Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
	int index=0;
	for(auto const &pair : *inTos.kvPtr) {
		TypedValue tvList=TypedValue(new List());
		tvList.listPtr->push_back(pair.first);
		tvList.listPtr->push_back(pair.second);
		dataBody[index++]=tvList;
	}
	inContext.DS.emplace_back(arrayPtr);
}

static void stringToArray(Context& inContext,TypedValue& inTos) {
	std::vector<std::string> strVec;
	std::string *str=inTos.stringPtr.get();
	size_t n=str->length();
	const char *p=str->c_str();

	int start,end=-1;
	do {
		start=end+1;
		char c;
		// skip white space
		for(; c=p[start],c==' ' || c=='\t' || c=='\n' || c=='\r'; start++) {
			// empty
		}
		if(c=='\0') { break; }
		// search substring
		for(end=start+1;
			c=p[end],c!=' ' && c!='\t' && c!='\n' && c!='\r' && c!='\0';
			end++) {
			// empty
		}
		strVec.emplace_back(str->substr(start,(size_t)end-(size_t)start));
		if(c=='\0') { break; }
	} while(end<n);		
	
	int numOfElements=(int)strVec.size();
	TypedValue *dataBody=new TypedValue[numOfElements];
	for(size_t i=0; i<numOfElements; i++) { dataBody[i]=strVec[i]; }
	Array<TypedValue> *arrayPtr=new Array<TypedValue>(numOfElements,dataBody,true);
	inContext.DS.emplace_back(arrayPtr);
}

static bool queryTosType(Context& inContext,DataType inDataType) {
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
	TypedValue tos=Pop(inContext.DS);
	bool result=tos.dataType==inDataType;
	inContext.DS.emplace_back(result);
	return true;
}

static bool lookAheadQueryTosType(Context& inContext,DataType inDataType) {
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
	TypedValue& tos=ReadTOS(inContext.DS);
	bool result=tos.dataType==inDataType;
	inContext.DS.emplace_back(result);
	return true;
}

