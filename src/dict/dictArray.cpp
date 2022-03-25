#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool set(Context& inContext,
				TypedValue& inTarget,TypedValue& inIndex,TypedValue& inValue);
static bool get(Context& inContext,TypedValue& inTarget,TypedValue& inIndex);

void InitDict_Array() {
	// (n -- array)
	// n new-array
	Install(new Word("new-array",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tvArraySize=Pop(inContext.DS);
		if(tvArraySize.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tvArraySize);
	   	}

		const int n=tvArraySize.intValue;
		if(n<=0) { return inContext.Error(ErrorIdWithInt::TosPositiveInt,n); }

		TypedValue *dataBody=new TypedValue[n];
		Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
		inContext.DS.emplace_back(arrayPtr);
		NEXT;
	}));

	// sizeOfArray initValue -- array
	Install(new Word("new-array-with",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tvInitValue=Pop(inContext.DS);
		TypedValue tvSize=Pop(inContext.DS);
		if(tvSize.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::SecondInt,tvSize);
		}

		const int n=tvSize.intValue;
		if(n<=0) { return inContext.Error(ErrorIdWithInt::SecondPositiveInt,n); }

		TypedValue *dataBody=new TypedValue[n];
		for(int i=0; i<n; i++) {
			dataBody[i]=tvInitValue;
		}
		Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
		inContext.DS.emplace_back(arrayPtr);
		NEXT;
	}));

	// (array index value -- array)  <- this operation is thread-safe. 
	// or
	// (list index value -- list)
	// or
	// (kv key value -- kv)
	//
	// effect: array[index]=value
	//			list[index]=value
	Install(new Word("@set",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::DsAtLeast3);
		}
		TypedValue tvValue=Pop(inContext.DS);
		TypedValue tvIndex=Pop(inContext.DS);
		TypedValue& tvTarget=ReadTOS(inContext.DS);
		if(set(inContext,tvTarget,tvIndex,tvValue)==false) { return false; }
		NEXT;
	}));

	// array index value --  <- this operation is thread-safe. 
	//   or
	// list index value -- 
	//   or
	// kv key value -- 
	//
	// effect: array[index]=value
	//			list[index]=value
	//		   assoc[ key ]=value
	Install(new Word("set",WORD_FUNC {
		TypedValue tvValue= Pop(inContext.DS);
		TypedValue tvIndex= Pop(inContext.DS);
		TypedValue tvTarget=Pop(inContext.DS);
		if(set(inContext,tvTarget,tvIndex,tvValue)==false) { return false; }
		NEXT;
	}));

	// array index --- array value  [ <- this operation is thread-safe. ]
	//	or
	// list index --- list value
	//  or
	// kv index --- associated value with the key
	//
	// get array[index]
	// get n-th item at the list
	// get kv[index]
	Install(new Word("@get",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); }
		TypedValue tvIndex=Pop(inContext.DS);
		TypedValue& tvTarget=ReadTOS(inContext.DS);
		if(get(inContext,tvTarget,tvIndex)==false) { return false; }
		NEXT;
	}));

	// A I  --- X
	// L I  --- X
	// Z Xk --- Xv
	Install(new Word("get",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); }
		TypedValue tvIndex=Pop(inContext.DS);
		TypedValue tvTarget=Pop(inContext.DS);
		if(get(inContext,tvTarget,tvIndex)==false) { return false; }
		NEXT;
	}));

	// (array -- n)
	Install(new Word("size",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case DataType::Array:
				inContext.DS.emplace_back(tos.arrayPtr->length);
				break;
			case DataType::List:
				inContext.DS.emplace_back((int)tos.listPtr->size());
				break;
			case DataType::String:
			case DataType::Symbol:
				inContext.DS.emplace_back((int)tos.stringPtr->length());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosArrayOrListOrStringOrSymbol,tos);
		}
		NEXT;
	}));

	// (array -- array n)
	Install(new Word("@size",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Array:
				inContext.DS.emplace_back(tos.arrayPtr->length);
				break;
			case DataType::List:
				inContext.DS.emplace_back((int)tos.listPtr->size());
				break;
			case DataType::String:
			case DataType::Symbol:
				inContext.DS.emplace_back((int)tos.stringPtr->length());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosArrayOrListOrStringOrSymbol,tos);
		}
		NEXT;
	}));

	// array index --- array index t/f
	// 	or
	// list index --- list index t/f
	Install(new Word("@valid-index?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue& tvIndex=ReadTOS(inContext.DS);
		if(tvIndex.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tvIndex);
		}

		TypedValue& tvTarget=ReadSecond(inContext.DS);
		bool result;
		if(tvTarget.dataType==DataType::Array) {
			// is index in [0,tvTarget.arrayPtr->length) ?
			result=(unsigned int)(tvTarget.arrayPtr->length)
					>(unsigned int)(tvIndex.intValue);
		} else if(tvTarget.dataType==DataType::List) {
			// is index in [0,tvTarget.listPtr->length) ?
			result=(unsigned int)(tvTarget.listPtr->size())
					>(unsigned int)(tvIndex.intValue);
		} else {
			return inContext.Error(InvalidTypeErrorID::SecondArrayOrList,tvTarget);
		}

		inContext.DS.emplace_back(result);
		NEXT;
	}));
}

static bool set(Context& inContext,
				TypedValue& inTarget,TypedValue& inIndex,TypedValue& inValue) {
	switch(inTarget.dataType) {
		case DataType::Array: {
				if(inIndex.dataType!=DataType::Int) {
					return inContext.Error(InvalidTypeErrorID::SecondInt,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.arrayPtr->length) ?
				if((unsigned int)(inTarget.arrayPtr->length)<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::ArrayIndexOutOfRange,
										   inTarget.arrayPtr->length-1,index);
				}
				Lock(inTarget.arrayPtr->mutex);
					inTarget.arrayPtr->data[index]=inValue;
				Unlock(inTarget.arrayPtr->mutex);	
			}
			break;
		case DataType::List: {
				if(inIndex.dataType!=DataType::Int) {
					return inContext.Error(InvalidTypeErrorID::SecondInt,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.listPtr->size()) ?
				if((unsigned int)(inTarget.listPtr->size())<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::ListIndexOutOfRange,
										   (int)inTarget.listPtr->size()-1,index);
				}
				inTarget.listPtr->at(index)=inValue;
			}
			break;
		case DataType::KV: 
			(*(inTarget.kvPtr))[inIndex]=inValue;
			break;
		default:
			return inContext.Error(InvalidTypeErrorID::ThirdArrayOrListOrKV,inTarget);
	}
	return true;
}

static bool get(Context& inContext,TypedValue& inTarget,TypedValue& inIndex) {
	switch(inTarget.dataType) {
		case DataType::Array: {
				if(inIndex.dataType!=DataType::Int) {
					return inContext.Error(InvalidTypeErrorID::TosInt,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.arrayPtr->length) ?
				if((unsigned int)(inTarget.arrayPtr->length)<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::ArrayIndexOutOfRange,
										   inTarget.arrayPtr->length-1,index);
				}
				Lock(inTarget.arrayPtr->mutex);
					inContext.DS.emplace_back(inTarget.arrayPtr->data[index]);
				Unlock(inTarget.arrayPtr->mutex);
			}
			break;
		case DataType::List: {
				if(inIndex.dataType!=DataType::Int) {
					return inContext.Error(InvalidTypeErrorID::TosInt,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.listPtr->size()) ?
				if((unsigned int)(inTarget.listPtr->size())<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::ListIndexOutOfRange,
										   (int)inTarget.listPtr->size()-1,index);
				}
				inContext.DS.emplace_back(inTarget.listPtr->at(index));
			} 
			break;
		case DataType::KV: {
				if(inTarget.kvPtr->count(inIndex)<1) {
					return inContext.Error(InvalidValueErrorID::TosNoSuchAKey,inIndex);
				}
				inContext.DS.emplace_back((*(inTarget.kvPtr))[inIndex]);
			}
			break;
		default:
			return inContext.Error(InvalidTypeErrorID::SecondArrayOrListOrKV,inTarget);
	}
	return true;
}

