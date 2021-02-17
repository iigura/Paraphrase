#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

static bool set(Context& inContext,
				TypedValue& inTarget,TypedValue& inIndex,TypedValue& inValue);

void InitDict_Array() {
	// (n -- array)
	// n new-array
	Install(new Word("new-array",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tvArraySize=Pop(inContext.DS);
		if(tvArraySize.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tvArraySize);
	   	}

		const int n=tvArraySize.intValue;
		if(n<=0) { return inContext.Error(ErrorIdWithInt::E_TOS_POSITIVE_INT,n); }

		TypedValue *dataBody=new TypedValue[n];
		Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
		inContext.DS.emplace_back(arrayPtr);
		NEXT;
	}));

	// sizeOfArray initValue -- array
	Install(new Word("new-array-with",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}

		TypedValue tvInitValue=Pop(inContext.DS);
		TypedValue tvSize=Pop(inContext.DS);
		if(tvSize.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT,tvSize);
		}

		const int n=tvSize.intValue;
		if(n<=0) { return inContext.Error(ErrorIdWithInt::E_SECOND_POSITIVE_INT,n); }

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
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_3);
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
	Install(new Word("get",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tvIndex=Pop(inContext.DS);
		TypedValue& tvTarget=ReadTOS(inContext.DS);

		switch(tvTarget.dataType) {
			case DataType::kTypeArray: {
					if(tvIndex.dataType!=DataType::kTypeInt) {
						return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tvIndex);
					}
					const int index=tvIndex.intValue;
					// is index in [0,tvTarget.arrayPtr->length) ?
					if((unsigned int)(tvTarget.arrayPtr->length)<=(unsigned int)index) {
						return inContext.Error(ErrorIdWith2int::E_ARRAY_INDEX_OUT_OF_RANGE,
											   tvTarget.arrayPtr->length-1,index);
					}
					Lock(tvTarget.arrayPtr->mutex);
						inContext.DS.emplace_back(tvTarget.arrayPtr->data[index]);
					Unlock(tvTarget.arrayPtr->mutex);
				}
				break;
			case DataType::kTypeList: {
					if(tvIndex.dataType!=DataType::kTypeInt) {
						return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tvIndex);
					}
					const int index=tvIndex.intValue;
					// is index in [0,tvTarget.listPtr->size()) ?
					if((unsigned int)(tvTarget.listPtr->size())<=(unsigned int)index) {
						return inContext.Error(ErrorIdWith2int::E_LIST_INDEX_OUT_OF_RANGE,
											   (int)tvTarget.listPtr->size()-1,index);
					}
					inContext.DS.emplace_back(tvTarget.listPtr->at(index));
				} 
				break;
			case DataType::kTypeKV: {
					if(tvTarget.kvPtr->count(tvIndex)<1) {
						return inContext.Error(InvalidValueErrorID::E_TOS_NO_SUCH_A_KEY,tvIndex);
					}
					inContext.DS.emplace_back((*(tvTarget.kvPtr))[tvIndex]);
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_SECOND_ARRAY_OR_LIST_OR_KV,tvTarget);
		}
		NEXT;
	}));

	// (array -- n)
	Install(new Word("size",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeArray:
				inContext.DS.emplace_back(tos.arrayPtr->length);
				break;
			case DataType::kTypeList:
				inContext.DS.emplace_back((int)tos.listPtr->size());
				break;
			case DataType::kTypeString:
			case DataType::kTypeSymbol:
				inContext.DS.emplace_back((int)tos.stringPtr->length());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_ARRAY_OR_LIST_OR_STRING_OR_SYMBOL,tos);
		}
		NEXT;
	}));

	// (array -- array n)
	Install(new Word("@size",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeArray:
				inContext.DS.emplace_back(tos.arrayPtr->length);
				break;
			case DataType::kTypeList:
				inContext.DS.emplace_back((int)tos.listPtr->size());
				break;
			case DataType::kTypeString:
			case DataType::kTypeSymbol:
				inContext.DS.emplace_back((int)tos.stringPtr->length());
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_ARRAY_OR_LIST_OR_STRING_OR_SYMBOL,tos);
		}
		NEXT;
	}));

	// array index --- array index t/f
	// 	or
	// list index --- list index t/f
	Install(new Word("valid-index?",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}

		TypedValue& tvIndex=ReadTOS(inContext.DS);
		if(tvIndex.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tvIndex);
		}

		TypedValue& tvTarget=ReadSecond(inContext.DS);
		bool result;
		if(tvTarget.dataType==DataType::kTypeArray) {
			// is index in [0,tvTarget.arrayPtr->length) ?
			result=(unsigned int)(tvTarget.arrayPtr->length)
					>(unsigned int)(tvIndex.intValue);
		} else if(tvTarget.dataType==DataType::kTypeList) {
			// is index in [0,tvTarget.listPtr->length) ?
			result=(unsigned int)(tvTarget.listPtr->size())
					>(unsigned int)(tvIndex.intValue);
		} else {
			return inContext.Error(InvalidTypeErrorID::E_SECOND_ARRAY_OR_LIST,tvTarget);
		}

		inContext.DS.emplace_back(result);
		NEXT;
	}));
}

static bool set(Context& inContext,
				TypedValue& inTarget,TypedValue& inIndex,TypedValue& inValue) {
	switch(inTarget.dataType) {
		case DataType::kTypeArray: {
				if(inIndex.dataType!=DataType::kTypeInt) {
					return inContext.Error(InvalidTypeErrorID::E_SECOND_INT,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.arrayPtr->length) ?
				if((unsigned int)(inTarget.arrayPtr->length)<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::E_ARRAY_INDEX_OUT_OF_RANGE,
										   inTarget.arrayPtr->length-1,index);
				}
				Lock(inTarget.arrayPtr->mutex);
					inTarget.arrayPtr->data[index]=inValue;
				Unlock(inTarget.arrayPtr->mutex);	
			}
			break;
		case DataType::kTypeList: {
				if(inIndex.dataType!=DataType::kTypeInt) {
					return inContext.Error(InvalidTypeErrorID::E_SECOND_INT,inIndex);
				}
				const int index=inIndex.intValue;
				// is index in [0,tvTarget.listPtr->size()) ?
				if((unsigned int)(inTarget.listPtr->size())<=(unsigned int)index) {
					return inContext.Error(ErrorIdWith2int::E_LIST_INDEX_OUT_OF_RANGE,
										   (int)inTarget.listPtr->size()-1,index);
				}
				inTarget.listPtr->at(index)=inValue;
			}
			break;
		case DataType::kTypeKV: 
			(*(inTarget.kvPtr))[inIndex]=inValue;
			break;
		default:
			return inContext.Error(InvalidTypeErrorID::E_THIRD_ARRAY_OR_LIST_OR_KV,inTarget);
	}
	return true;
}

