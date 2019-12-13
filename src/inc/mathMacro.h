#pragma once

extern BigInt G_BI_DBL_MAX, G_BI_MINUS_DBL_MAX;

#define SKIP(x)

// c=a OP b
// MathOp(c,a,OP,b);
#define MathOp(outDest,inSrc1,OP,inSrc2) do { \
	int combinationType=GetMathOpType(inSrc1.dataType,inSrc2.dataType); \
	SKIP( if(combinationType>35) { \
		fprintf(stderr,"SYSTEM ERROR\n"); \
		exit(-1); \
	} ) \
	switch(combinationType) { \
		case kIntInt: \
			outDest.Set(inSrc1.intValue OP inSrc2.intValue); \
			break; \
		case kIntLong: \
			outDest=TypedValue((long)inSrc1.intValue OP inSrc2.longValue); \
			break; \
		case kIntBigInt: \
			outDest=TypedValue(inSrc1.intValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case kIntFloat: \
			outDest.Set((float)inSrc1.intValue OP inSrc2.floatValue); \
			break; \
		case kIntDouble: \
			outDest.Set((double)inSrc1.intValue OP inSrc2.doubleValue); \
			break; \
		case kIntBigFloat: \
			outDest=TypedValue(inSrc1.intValue OP *(inSrc2.bigFloatPtr)); \
			break; \
		case kLongInt: \
			outDest=TypedValue(inSrc1.longValue OP (long)inSrc2.intValue); \
			break; \
		case kLongLong: \
			outDest=TypedValue(inSrc1.longValue OP inSrc2.longValue); \
			break; \
		case kLongBigInt: \
			outDest=TypedValue(inSrc1.longValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case kLongFloat: \
			outDest.Set((float)inSrc1.longValue OP inSrc2.floatValue); \
			break; \
		case kLongDouble: \
			outDest.Set((double)inSrc1.longValue OP inSrc2.doubleValue); \
			break; \
		case kLongBigFloat: \
			outDest=TypedValue(inSrc1.longValue OP *(inSrc2.bigFloatPtr)); \
			break; \
		case kBigIntInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.intValue); \
			break; \
		case kBigIntLong: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.longValue); \
			break; \
		case kBigIntBigInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP *(inSrc2.bigIntPtr)); \
			break; \
		case kBigIntFloat: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP BigFloat(inSrc2.floatValue)); \
			break; \
		case kBigIntDouble: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP BigFloat(inSrc2.doubleValue)); \
			break; \
		case kBigIntBigFloat: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case kFloatInt: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.intValue); \
			break; \
		case kFloatLong: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.longValue); \
			break; \
		case kFloatBigInt: \
			outDest=TypedValue(BigFloat(inSrc1.floatValue) \
								OP BigFloat(*(inSrc2.bigIntPtr))); \
			break; \
		case kFloatFloat: \
			outDest.Set(inSrc1.floatValue OP inSrc2.floatValue); \
			break; \
		case kFloatDouble: \
			outDest.Set((double)inSrc1.floatValue OP inSrc2.doubleValue); \
			break; \
		case kFloatBigFloat: \
			outDest=TypedValue(BigFloat(inSrc1.floatValue) \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case kDoubleInt: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.intValue); \
			break; \
		case kDoubleLong: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.longValue); \
			break; \
		case kDoubleBigInt: \
			outDest=TypedValue(inSrc1.doubleValue \
								OP BigFloat(*(inSrc2.bigFloatPtr))); \
			break; \
		case kDoubleFloat: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.floatValue); \
			break; \
		case kDoubleDouble: \
			outDest.Set(inSrc1.doubleValue OP inSrc2.doubleValue); \
			break; \
		case kDoubleBigFloat: \
			outDest=TypedValue(inSrc1.doubleValue \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case kBigFloatInt: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.intValue); \
			break; \
		case kBigFloatLong: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.longValue); \
			break; \
		case kBigFloatBigInt: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) \
								OP BigFloat(*(inSrc2.bigIntPtr))); \
			break; \
		case kBigFloatFloat: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.floatValue); \
			break; \
		case kBigFloatDouble: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.doubleValue); \
			break; \
		case kBigFloatBigFloat: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP *(inSrc2.bigFloatPtr)); \
			break; \
		default: \
			fprintf(stderr,"SYSTEM ERROR\n"); \
			exit(-1); \
	} \
} while(0)

// a += b
// AssignOp(a,+,b)
#define AssignOp(inContext,inTarget,OP,inRef) do { \
	if(#OP=="+" && inTarget.dataType==kTypeString && inRef.dataType==kTypeString) { \
		inTarget=TypedValue(*inTarget.stringPtr+*inRef.stringPtr); \
	} else { \
		const int combinationType=GetMathOpType(inTarget.dataType,inRef.dataType); \
		if(combinationType>=kInvalidMathOpTypeThreshold) { \
			return inContext.Error(E_INVALID_DATA_TYPE_TOS_SECOND,inRef,inTarget); \
		} \
		switch(combinationType) { \
			case kIntInt:  \
				inTarget.intValue=inTarget.intValue OP inRef.intValue; \
				break; \
			case kIntLong: \
				inTarget.longValue=(long)inTarget.intValue OP inRef.longValue; \
				inTarget.dataType=kTypeLong; \
				break; \
			case kIntBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.intValue OP *inRef.bigIntPtr); \
				inTarget.dataType=kTypeBigInt; \
				break; \
			case kIntFloat: \
				inTarget.floatValue=(float)inTarget.intValue OP inRef.floatValue; \
				inTarget.dataType=kTypeFloat; \
				break; \
			case kIntDouble: \
				inTarget.doubleValue=(double)inTarget.intValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kIntBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.intValue OP *inRef.bigFloatPtr); \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kLongInt: \
				inTarget.longValue=inTarget.longValue OP (long)inRef.intValue; \
				break; \
			case kLongLong: \
				inTarget.longValue=inTarget.longValue OP inRef.longValue; \
				break; \
			case kLongBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.longValue OP *inRef.bigIntPtr); \
				inTarget.dataType=kTypeBigInt; \
			case kLongFloat: \
				inTarget.floatValue=(float)inTarget.longValue OP inRef.floatValue; \
				inTarget.dataType=kTypeFloat; \
				break; \
			case kLongDouble: \
				inTarget.doubleValue=(double)inTarget.longValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kLongBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.longValue OP *inRef.bigFloatPtr); \
				inTarget.dataType=kTypeBigFloat; \
			case kBigIntInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inRef.intValue; \
				break; \
			case kBigIntLong: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inRef.longValue; \
				break; \
			case kBigIntBigInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP *inRef.bigIntPtr; \
				break; \
			case kBigIntFloat: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inRef.floatValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kBigIntDouble: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inRef.doubleValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kBigIntBigFloat: { \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=BigFloat(*inTarget.bigIntPtr) OP *inRef.bigFloatPtr; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kFloatInt: \
				inTarget.floatValue=inTarget.floatValue OP (float)inRef.intValue; \
				break; \
			case kFloatLong: \
				inTarget.floatValue=inTarget.floatValue OP (float)inRef.longValue; \
				break; \
			case kFloatBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inRef.bigIntPtr); \
					*bigFloat=inTarget.floatValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kFloatFloat: \
				inTarget.floatValue=inTarget.floatValue OP inRef.floatValue; \
				break; \
			case kFloatDouble: \
				inTarget.doubleValue=(double)inTarget.floatValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kFloatBigFloat: { \
					BigFloat *bigFloat=new BigFloat(*inRef.bigIntPtr); \
					*bigFloat=inTarget.floatValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kDoubleInt: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.intValue; \
				break; \
			case kDoubleLong: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.longValue; \
				break; \
			case kDoubleBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inRef.bigIntPtr); \
					*bigFloat=inTarget.doubleValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kDoubleFloat: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.floatValue; \
				break; \
			case kDoubleDouble: \
				inTarget.doubleValue=inTarget.doubleValue OP inRef.doubleValue; \
				break; \
			case kDoubleBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.doubleValue OP *inRef.bigFloatPtr); \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			case kBigFloatInt: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inRef.intValue); \
				break; \
			case kBigFloatLong: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inRef.longValue); \
				break; \
			case kBigFloatBigInt: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(*inRef.bigIntPtr); \
				break; \
			case kBigFloatFloat: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inRef.floatValue); \
				break; \
			case kBigFloatDouble: \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inRef.doubleValue); \
				break; \
			case kBigFloatBigFloat: \
				*inTarget.bigFloatPtr = *inTarget.bigFloatPtr OP *inRef.bigFloatPtr; \
				break; \
			default: \
				return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_FOR_OP, \
												   std::string(#OP),inTarget,inRef); \
		} \
	} \
} while(0)

// Push(a + b)
// PushOp(inContext.DS,a,+,b)
#define PushOp(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==kTypeInt && inTos.dataType==kTypeInt) { \
		/* int x int -> int */ \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==kTypeLong && inTos.dataType==kTypeLong) { \
		/* long x long -> long */ \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==kTypeFloat && inTos.dataType==kTypeFloat) { \
		/* float x float -> float */ \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==kTypeDouble && inTos.dataType==kTypeDouble) { \
		/* double x double -> double */ \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==kTypeBigInt && inTos.dataType==kTypeBigInt) { \
		/* bigInt x bigInt -> bigInt */ \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else if(inSecond.dataType==kTypeBigFloat && inTos.dataType==kTypeBigFloat) { \
		/* bigFloat x bigFloat -> bigFloat */ \
		inStack.emplace_back(*(inSecond.bigFloatPtr) OP *(inTos.bigFloatPtr)); \
	} else if(inSecond.dataType==kTypeInt) { \
		switch(inTos.dataType) { \
			case kTypeLong:		/* int x long -> long */ \
				inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
				break; \
			case kTypeFloat:	/* int x float -> float */ \
				inStack.emplace_back((float)inSecond.intValue OP inTos.floatValue); \
				break; \
			case kTypeDouble:	/* int x double -> double */ \
				inStack.emplace_back((double)inSecond.intValue OP inTos.doubleValue); \
				break; \
			case kTypeBigInt: 	/* int x BigInt -> BigInt */ \
				inStack.emplace_back(BigInt(inSecond.intValue OP *inTos.bigIntPtr)); \
				break; \
			case kTypeBigFloat: /* int x BigFloat -> BigFloat */ \
				inStack.emplace_back( \
							BigFloat(inSecond.intValue OP *inTos.bigFloatPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==kTypeLong) { \
		switch(inTos.dataType) { \
			case kTypeInt:		/* long x int -> long */ \
				inStack.emplace_back(inSecond.longValue OP (long)inTos.intValue); \
				break; \
			case kTypeFloat:	/* long x float -> float */ \
				inStack.emplace_back((float)inSecond.longValue OP inTos.floatValue); \
				break; \
			case kTypeDouble:	/* long x double -> double */ \
				inStack.emplace_back((double)inSecond.longValue OP inTos.doubleValue); \
				break; \
			case kTypeBigInt: 	/* long x bigInt -> bigInt */ \
				inStack.emplace_back(BigInt(inSecond.longValue OP *inTos.bigIntPtr)); \
				break; \
			case kTypeBigFloat:	/* long x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.longValue) OP *inTos.bigFloatPtr); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==kTypeFloat) { \
		switch(inTos.dataType) { \
			case kTypeInt:		/* float x int -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.intValue); \
				break; \
			case kTypeLong:		/* float x long -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.longValue); \
				break; \
			case kTypeDouble:	/* float x double -> double */ \
				inStack.emplace_back( \
					(double)inSecond.floatValue OP inTos.doubleValue); \
				break; \
			case kTypeBigInt: 	/* float x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					inSecond.floatValue OP BigFloat(*inTos.bigIntPtr)); \
				break; \
			case kTypeBigFloat:	/* float x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.floatValue OP *(inTos.bigFloatPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==kTypeDouble) { \
		switch(inTos.dataType) { \
			case kTypeInt:		/* double x int -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.intValue); \
				break; \
			case kTypeLong:		/* double x long -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.longValue); \
				break; \
			case kTypeFloat:	/* double x float -> double */ \
				inStack.emplace_back( \
					inSecond.doubleValue OP (double)inTos.floatValue); \
				break; \
			case kTypeBigInt: 	/* double x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.doubleValue) OP BigFloat(*(inTos.bigIntPtr))); \
				break; \
			case kTypeBigFloat:	/* double x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.doubleValue OP *(inTos.bigFloatPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==kTypeBigInt) { \
		switch(inTos.dataType) { \
			case kTypeInt:		/* bigInt x int -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
				break; \
			case kTypeLong: 	/* bigInt x long -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
				break; \
			case kTypeFloat:	/* bigInt x float -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP BigFloat(inTos.floatValue)); \
				break; \
			case kTypeDouble:	/* bigInt x double -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP inTos.doubleValue); \
				break; \
			case kTypeBigFloat:	/* bigInt x bigFloat -> bigFlaot */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP *(inTos.bigFloatPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==kTypeBigFloat) { \
		switch(inTos.dataType) { \
			case kTypeInt: 		/* bigFloat x int -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.intValue); \
				break; \
			case kTypeLong: 	/* bigFloat x long -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.longValue); \
				break; \
			case kTypeFloat:	/* bigFloat x float -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.floatValue); \
				break; \
			case kTypeDouble:	/* bigFloat x double -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.doubleValue); \
				break; \
			case kTypeBigInt:	/* bigFloat x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP BigFloat(*(inTos.bigIntPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if(#OP=="+" \
			 && inSecond.dataType==kTypeString && inTos.dataType==kTypeString) { \
		inStack.emplace_back(TypedValue(*(inSecond.stringPtr)+*(inTos.stringPtr))); \
	} else { \
onError: \
		return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_FOR_OP, \
										   std::string(#OP),inSecond,inTos); \
	} \
} while(0)

/* TwoOp use inContext */
#define TwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	AssignOp(inContext,second,OP,tos); \
	NEXT; \
} while(0)

#define RefTwoOp(inStack,OP) do { \
	if(inStack.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue& tos   =ReadTOS(inStack); \
	TypedValue& second=ReadSecond(inStack); \
	PushOp(inStack,second,OP,tos); \
	NEXT; \
} while(0)

#define CmpOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(tos.dataType==kTypeInt && second.dataType==kTypeInt) { \
		second.dataType=kTypeBool; \
		second.boolValue=second.intValue OP tos.intValue; \
	} else if(tos.dataType==kTypeLong && second.dataType==kTypeLong) { \
		second.dataType=kTypeBool; \
		second.boolValue=second.longValue OP tos.longValue; \
	} else if(tos.dataType==kTypeDouble && second.dataType==kTypeDouble) { \
		second.dataType=kTypeBool; \
		second.boolValue=second.doubleValue OP tos.doubleValue; \
	} else if(tos.dataType==kTypeFloat && second.dataType==kTypeFloat) { \
		second.dataType=kTypeBool; \
		second.boolValue=second.floatValue OP tos.floatValue; \
	} else if(tos.dataType==kTypeString && second.dataType==kTypeString) { \
		second.dataType=kTypeBool; \
		second.boolValue=*second.stringPtr OP *tos.stringPtr; \
	} else if(tos.dataType==kTypeBigInt && second.dataType==kTypeBigInt) { \
		second.dataType=kTypeBool; \
		second.boolValue=*(second.bigIntPtr) OP *(tos.bigIntPtr); \
	} else if(#OP=="==" && second.dataType==kTypeList && tos.dataType==kTypeList) { \
		bool result = second==tos; \
		second.listPtr.reset();	\
		second.dataType=kTypeBool; \
		second.boolValue=result; \
	} else { \
		switch(second.dataType) { \
			case kTypeInt: \
				switch(tos.dataType) { \
					case kTypeLong: \
						second.dataType=kTypeBool; \
						second.boolValue=(long)second.intValue  OP tos.longValue; \
						break; \
					case kTypeBigInt: { \
							BigInt bi(second.intValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case kTypeFloat: \
						second.dataType=kTypeBool; \
						second.boolValue=(float)second.intValue OP tos.floatValue; \
						break; \
					case kTypeDouble: \
						second.dataType=kTypeBool; \
						second.boolValue=((double)second.intValue) \
											OP (tos.doubleValue); \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(second.intValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeLong: \
				switch(tos.dataType) { \
					case kTypeInt: \
						second.dataType=kTypeBool; \
						second.boolValue=second.longValue OP (long)tos.intValue; \
						break; \
					case kTypeBigInt: { \
							BigInt bi(second.longValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case kTypeFloat: \
						second.dataType=kTypeBool; \
						second.boolValue=(float)second.longValue OP tos.floatValue; \
						break; \
					case kTypeDouble: \
						second.dataType=kTypeBool; \
						second.boolValue=(double)second.longValue OP tos.doubleValue; \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(second.longValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeBigInt: \
				switch(tos.dataType) { \
					case kTypeInt: { \
							BigInt bi(tos.intValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case kTypeLong: { \
							BigInt bi(tos.longValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case kTypeFloat: { \
							BigFloat s(*second.bigIntPtr); \
							BigFloat t(tos.floatValue); \
							second.dataType=kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case kTypeDouble: { \
							BigFloat s(*second.bigIntPtr); \
							BigFloat t(tos.doubleValue); \
							second.dataType=kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(*second.bigIntPtr); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeFloat: \
				switch(tos.dataType) { \
					case kTypeInt: \
						second.dataType=kTypeBool; \
						second.boolValue=second.floatValue OP (float)tos.intValue; \
						break; \
					case kTypeLong: \
						second.dataType=kTypeBool; \
						second.boolValue=(double)second.floatValue \
											OP (double)tos.longValue; \
						break; \
					case kTypeBigInt: { \
							BigFloat s(second.floatValue); \
							BigFloat t(*tos.bigIntPtr); \
							second.dataType=kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case kTypeDouble: \
						second.dataType=kTypeBool; \
						second.boolValue=(double)second.floatValue \
											OP tos.doubleValue; \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(second.floatValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeDouble: \
				switch(tos.dataType) { \
					case kTypeInt: \
						second.dataType=kTypeBool; \
						second.boolValue=second.doubleValue OP (double)tos.intValue; \
						break; \
					case kTypeLong: \
						second.dataType=kTypeBool; \
						second.boolValue=second.doubleValue OP (double)tos.longValue; \
						break; \
					case kTypeBigInt: { \
							BigFloat s(second.doubleValue); \
							BigFloat t(*tos.bigIntPtr); \
							second.dataType=kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case kTypeFloat: \
						second.dataType=kTypeBool; \
						second.boolValue=second.doubleValue \
											OP (double)tos.floatValue; \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(second.doubleValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeBigFloat: \
				switch(tos.dataType) { \
					case kTypeInt: { \
							BigFloat bf(tos.intValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case kTypeLong: { \
							BigFloat bf(tos.longValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case kTypeBigInt: { \
							BigFloat bf(*tos.bigIntPtr); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case kTypeFloat: { \
							BigFloat bf(tos.floatValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case kTypeDouble: { \
							BigFloat bf(tos.doubleValue); \
							second.dataType=kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
				} \
				break; \
			default: goto onError; \
		} \
	} \
	NEXT; \
onError: \
	return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_FOR_OP, \
									   std::string(#OP),second,tos); \
} while(0)

#define RefCmpOpMain(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==kTypeInt && inTos.dataType==kTypeInt) { \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==kTypeLong && inTos.dataType==kTypeLong) { \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==kTypeDouble || inTos.dataType==kTypeDouble) { \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==kTypeFloat || inTos.dataType==kTypeFloat) { \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==kTypeString && inTos.dataType==kTypeString) { \
		inStack.emplace_back(*(inSecond.stringPtr) OP *(inTos.stringPtr)); \
	} else if(inSecond.dataType==kTypeBigInt && inTos.dataType==kTypeBigInt) { \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else { \
		switch(inSecond.dataType) { \
			case kTypeInt: \
				switch(inTos.dataType) { \
					case kTypeLong: /* int x long -> bool */ \
						inStack.emplace_back( \
							(long)inSecond.intValue  OP inTos.longValue); \
						break; \
					case kTypeBigInt: /* int x bigInt -> bool */ \
						inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
						break; \
					case kTypeFloat: /* int x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.intValue OP inTos.floatValue); \
						break; \
					case kTypeDouble: /* int x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.intValue OP inTos.doubleValue); \
						break; \
					case kTypeBigFloat: /* int x bigInt -> bool */ \
						inStack.emplace_back( \
							inSecond.intValue OP *(inTos.bigIntPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeLong: \
				switch(inTos.dataType) { \
					case kTypeInt: /* long x int -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP (long)inTos.intValue); \
						break; \
					case kTypeBigInt: /* long x bigIng -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP *(inTos.bigIntPtr)); \
						break; \
					case kTypeFloat: /* long x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.longValue OP inTos.floatValue); \
						break; \
					case kTypeDouble: /* long x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.longValue OP inTos.doubleValue); \
						break; \
					case kTypeBigFloat: /* long x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeBigInt: \
				switch(inTos.dataType) { \
					case kTypeInt: /* bigInt x int -> bool */ \
						inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
						break; \
					case kTypeLong: /* bigInt x long -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigIntPtr) OP inTos.longValue); \
						break; \
					case kTypeFloat: /* bigInt x float -> bool */ \
						if(#OP==">" && *(inSecond.bigIntPtr) > G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP==">") { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(#OP==">=" \
								  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">=" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP==">=") { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(#OP=="<" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<" \
								  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<") { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(#OP=="<=" \
								  && *(inSecond.bigIntPtr) <= G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<=" \
								  && G_BI_DBL_MAX < *(inSecond.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<=") { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(#OP=="==" \
								  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
									|| *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(false); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP=="!=" \
								  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
									|| *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="!=") { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else { \
							inContext.Error(E_SYSTEM_ERROR); \
						} \
						break; \
					case kTypeDouble: /* bigInt x double -> bool */ \
						if(#OP==">" && *(inSecond.bigIntPtr) > G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP==">") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP==">=" \
								  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">=" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP==">=") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP=="<" \
								  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<" \
								  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP=="<=" \
								  && *(inSecond.bigIntPtr) <= G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<=" \
								  && G_BI_DBL_MAX < *(inSecond.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<=") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP=="==" \
								  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
									|| *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(false); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(#OP=="!=" \
								  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
									|| *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="!=") { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else { \
							inContext.Error(E_SYSTEM_ERROR); \
						} \
						break; \
					case kTypeBigFloat: /* bigInt x bigFloat -> bool */ \
						/* bigInt が bigFloat の範囲を超えた場合の挙動は未定とする */ \
						inStack.emplace_back( \
							static_cast<BigFloat>(*(inSecond.bigIntPtr)) \
							OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeFloat: \
				switch(inTos.dataType) { \
					case kTypeInt: /* float x int -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP (float)inTos.intValue); \
						break; \
					case kTypeLong: /* float x long -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.floatValue OP (double)inTos.longValue); \
						break; \
					case kTypeBigInt: /* float x bigInt -> bool */ \
						if(#OP==">" && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP==">" && *(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">") { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(#OP==">=" && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP==">=" \
								  && *(inTos.bigIntPtr)<=G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">=") { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(#OP=="<" && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<" \
								  && *(inTos.bigIntPtr) <= G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<") { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(#OP=="<=" && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<=" \
								  && *(inTos.bigIntPtr) < G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<=") { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(#OP=="==" \
								  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
									  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(false); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								inSecond.floatValue OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(#OP=="!=" \
								  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
									  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(true); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								inSecond.floatValue OP \
								static_cast<float>(*(inTos.bigIntPtr))); \
						} else { \
							inContext.Error(E_SYSTEM_ERROR); \
						} \
						break; \
					case kTypeDouble: /* float x double -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP inTos.doubleValue); \
						break; \
					case kTypeBigFloat: /* float x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeDouble: \
				switch(inTos.dataType) { \
					case kTypeInt: /* double x int -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP tos.intValue); \
						break; \
					case kTypeLong: /* double x long -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP inTos.longValue); \
						break; \
					case kTypeBigInt: /* double x bigInt -> bool */ \
						if(#OP==">" && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP==">" && *(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">") { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(#OP==">=" && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(#OP==">=" \
								  && *(inTos.bigIntPtr)<=G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(#OP==">=") { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(#OP=="<" && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<" \
								  && *(inTos.bigIntPtr) <= G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<") { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(#OP=="<=" && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(#OP=="<=" \
								  && *(inTos.bigIntPtr) < G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(#OP=="<=") { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(#OP=="==" \
								  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
									  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(false); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								inSecond.doubleValue OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(#OP=="!=" \
								  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
									  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(true); \
						} else if(#OP=="==") { \
							inStack.emplace_back( \
								inSecond.doubleValue OP \
								static_cast<double>(*(inTos.bigIntPtr))); \
						} else { \
							inContext.Error(E_SYSTEM_ERROR); \
						} \
						break; \
					case kTypeFloat: /* double x float -> bool */ \
						inStack.emplace_back( \
							inSecond.doubleValue OP inTos.floatValue); \
						break; \
					case kTypeBigFloat: /* double x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.doubleValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case kTypeBigFloat: \
				switch(inTos.dataType) { \
					case kTypeInt: /* bigFloat x int -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.intValue); \
						break; \
					case kTypeLong: /* bigFloat x long -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.longValue); \
						break; \
					case kTypeBigInt: /* bigFloat x bigInt -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP \
							static_cast<BigFloat>(*(inTos.bigIntPtr))); \
						break; \
					case kTypeFloat: /* bigFloat x float -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.floatValue); \
						break; \
					case kTypeDouble: /* bigFloat x double -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.doubleValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			default: goto onError; \
		} \
	} \
	goto onSuccess; \
onError: \
	return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_FOR_OP, \
									   std::string(#OP),inSecond,inTos); \
onSuccess: ; \
} while(0)

#define RefCmpOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefCmpOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

// result=tv1 OP tv2
// ex: GetIntegerCmpOpResult_AndConvert(a,second,<,tos)
// 	   equivalent to: a=second<tos;
#define GetIntegerCmpOpResult_AndConvert(result,tv1,OP,tv2) do { \
	if(tv1.dataType==kTypeInt && tv2.dataType==kTypeInt) { \
		result=tv1.intValue OP tv2.intValue; \
	} else if(tv1.dataType==kTypeLong && tv2.dataType==kTypeLong) { \
		result=tv1.longValue OP tv2.longValue; \
	} else if(tv1.dataType==kTypeBigInt && tv2.dataType==kTypeBigInt) { \
		result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
	} else { \
		switch(tv1.dataType) { \
			case kTypeInt: \
				if(tv2.dataType==kTypeLong) { \
					tv1.dataType=kTypeLong; \
					tv1.longValue=(long)tv1.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(tv2.dataType==kTypeBigInt); \
					tv1.dataType=kTypeBigInt; \
					tv1.bigIntPtr=new BigInt(tv1.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case kTypeLong: \
				if(tv2.dataType==kTypeInt) { \
					tv2.dataType=kTypeLong; \
					tv2.longValue=(long)tv2.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(rsSecond.dataType==kTypeBigInt); \
					tv1.dataType=kTypeBigInt; \
					tv1.bigIntPtr=new BigInt(tv1.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case kTypeBigInt: \
				if(tv2.dataType==kTypeInt) { \
					tv2.dataType=kTypeBigInt; \
					tv2.bigIntPtr=new BigInt(tv2.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} else { \
					assert(tv2.dataType==kTypeLong); \
					tv2.dataType=kTypeBigInt; \
					tv2.bigIntPtr=new BigInt(tv2.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			default: \
				return inContext.Error(E_SYSTEM_ERROR); \
		} \
	} \
} while(0)

// result=tv1 OP tv2
// ex: GetCmpOpResult(t,second,<,tos);
//     equivalent to t=second<tos;
#define GetCmpOpResult(result,tv1,OP,tv2) do { \
	if(tv1.dataType==tv2.dataType) { \
		switch(tv1.dataType) { \
			case kTypeInt: 		result=tv1.intValue OP tv2.intValue; 		 break; \
			case kTypeLong: 	result=tv1.longValue OP tv2.longValue; 		 break; \
			case kTypeBigInt:	result=*(tv1.bigIntPtr) OP *(tv2.bigIntPtr); break; \
			case kTypeFloat:	result=tv1.floatValue OP tv2.floatValue;	 break; \
			case kTypeDouble:	result=tv1.doubleValue OP tv2.doubleValue;	 break; \
			case kTypeBigFloat: \
				result=*(tv1.bigFloatPtr) OP *(tv2.bigFloatPtr); break; \
			default: \
				result=false; \
		} \
	} else { \
		switch(tv1.dataType) { \
			case kTypeInt: \
				switch(tv2.dataType) { \
					case kTypeLong: \
						result=((long)tv1.intValue) OP tv2.longValue; break; \
					case kTypeBigInt: \
						result=tv1.intValue OP *(tv2.bigIntPtr); break; \
					case kTypeFloat: \
						result=((float)tv1.intValue) OP tv2.floatValue; break; \
					case kTypeDouble: \
						result=((double)tv1.intValue) OP tv2.doubleValue; break; \
					case kTypeBigFloat: \
						result=tv1.intValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case kTypeLong: \
				switch(tv2.dataType) { \
					case kTypeInt: \
						result=tv1.longValue OP ((long)tv2.intValue); break; \
					case kTypeBigInt: \
						result=tv1.longValue OP *(tv2.bigFloatPtr); break; \
					case kTypeFloat: \
						result=((float)(tv1.longValue)) OP tv2.floatValue; break; \
					case kTypeDouble: \
						result=((double)tv1.longValue) OP tv2.doubleValue; break; \
					case kTypeBigFloat: \
						result=tv1.longValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case kTypeBigInt: \
				switch(tv2.dataType) { \
					case kTypeInt: \
						result=*(tv1.bigIntPtr) OP tv2.intValue; break; \
					case kTypeLong: \
						result=*(tv1.bigIntPtr) OP tv2.longValue; break; \
					case kTypeFloat: \
						result=BigFloat(*tv1.bigIntPtr) OP tv2.floatValue; break; \
					case kTypeDouble: \
						result=BigFloat(*tv1.bigIntPtr) OP tv2.doubleValue; break; \
					case kTypeBigFloat: \
						result=BigFloat(*tv1.bigIntPtr) OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case kTypeFloat: \
				switch(tv2.dataType) { \
					case kTypeInt: \
						result=tv1.floatValue OP (float)tv2.intValue; break; \
					case kTypeLong: \
						result=tv1.floatValue OP (float)tv2.longValue; break; \
					case kTypeBigInt: \
						result=tv1.floatValue OP BigFloat(*(tv2.bigIntPtr)); break; \
					case kTypeDouble: \
						result=(double)tv1.floatValue OP tv2.doubleValue; break; \
					case kTypeBigFloat: \
						result=tv1.floatValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case kTypeDouble: \
				switch(tv2.dataType) { \
					case kTypeInt: \
						result=tv1.doubleValue OP (double)tv2.intValue; break; \
					case kTypeLong: \
						result=tv1.doubleValue OP (double)tv2.longValue; break; \
					case kTypeBigInt: \
						result=tv1.doubleValue OP BigFloat(*(tv2.bigIntPtr)); break; \
					case kTypeFloat: \
						result=tv1.doubleValue OP (double)tv2.floatValue; break; \
					case kTypeBigFloat: \
						result=tv1.doubleValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case kTypeBigFloat: \
				switch(tv2.dataType) { \
					case kTypeInt: \
						result=*(tv1.bigFloatPtr) OP tv2.intValue; break; \
					case kTypeLong: \
						result=*(tv1.bigFloatPtr) OP tv2.longValue; break; \
					case kTypeFloat: \
						result=*(tv1.bigFloatPtr) OP tv2.floatValue; break; \
					case kTypeDouble: \
						result=*(tv1.bigFloatPtr) OP tv2.doubleValue; break; \
					default: result=false; \
				} \
				break; \
			default: /* never comes here */ \
				result=false; \
		} \
	} \
} while(0);

#define BitOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case kTypeInt: \
			switch(tos.dataType) { \
				case kTypeInt: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case kTypeLong: \
					second.dataType=kTypeLong; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=kTypeBigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeLong: \
			switch(tos.dataType) { \
				case kTypeInt: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case kTypeLong: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=kTypeBigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeBigInt: \
			switch(tos.dataType) { \
				case kTypeInt: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case kTypeLong: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case kTypeBigInt: \
					*second.bigIntPtr=(*second.bigIntPtr) OP (*tos.bigIntPtr); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case kTypeInt: \
			switch(inTos.dataType) { \
				case kTypeInt: /* int x int -> int  */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case kTypeLong: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* int x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeLong: \
			switch(inTos.dataType) { \
				case kTypeInt: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case kTypeLong: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* long x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.longValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeBigInt: \
			switch(inTos.dataType) { \
				case kTypeInt: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case kTypeLong: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* bigInt x bigInt -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case kTypeInt: \
			switch(tos.dataType) { \
				case kTypeInt: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case kTypeLong: \
					second.dataType=kTypeLong; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=kTypeBigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeLong: \
			switch(tos.dataType) { \
				case kTypeInt: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case kTypeLong: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=kTypeBigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case kTypeBigInt: \
			switch(tos.dataType) { \
				case kTypeInt: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case kTypeLong: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case kTypeBigInt: \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						*second.bigIntPtr=(*second.bigIntPtr) \
											OP static_cast<long>(*tos.bigIntPtr); \
					} else { \
						inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); tos.Dump(); \
						printf("SECOND: "); second.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case kTypeInt: \
			switch(inTos.dataType) { \
				case kTypeInt: /* int x int -> int */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case kTypeLong: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* int x bigInt -> bigInt */ \
					if(*inTos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.intValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		case kTypeLong: \
			switch(inTos.dataType) { \
				case kTypeInt: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case kTypeLong: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* long x bigInt -> bigInt */ \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.longValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		case kTypeBigInt: \
			switch(inTos.dataType) { \
				case kTypeInt: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case kTypeLong: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case kTypeBigInt: /* bigInt x bigInt -> bigInt */ \
					if(*(inTos.bigIntPtr)<=LONG_MAX) { \
						inStack.emplace_back(*(inSecond.bigIntPtr) \
											OP static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		default: \
			return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,inSecond); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitShiftOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BoolOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	if(tos.dataType!=kTypeBool) { return inContext.Error(E_TOS_BOOL,tos); } \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(second.dataType!=kTypeBool) { return inContext.Error(E_SECOND_BOOL,second); } \
	second.boolValue=tos.boolValue OP second.boolValue; \
	NEXT; \
} while(0)

#define OneArgFloatingFunc(FUNC) do { \
	if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	if(tos.dataType==kTypeBigInt) { \
		BigFloat *bigFloat=new BigFloat(); \
		*bigFloat=FUNC(BigFloat(*tos.bigIntPtr)); \
		delete(tos.bigIntPtr); \
		tos.bigFloatPtr=bigFloat; \
		tos.dataType=kTypeBigFloat; \
		goto next; \
	} else if(tos.dataType==kTypeBigFloat) { \
		*tos.bigFloatPtr=FUNC(*tos.bigFloatPtr); \
		goto next; \
	} \
	double result; \
	switch(tos.dataType) { \
		case kTypeInt: 		result=FUNC((double)tos.intValue);		break; \
		case kTypeLong:		result=FUNC((double)tos.longValue);		break; \
		case kTypeFloat:	result=FUNC((double)tos.floatValue);	break; \
		case kTypeDouble:	result=FUNC(tos.doubleValue);			break; \
		default: 			return inContext.Error(E_TOS_DOUBLABLE,tos); \
	} \
	tos.dataType=kTypeDouble; \
	tos.doubleValue=result; \
next: \
	NEXT; \
} while(0)

#define TwoArgFloatingFunc(FUNC) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(second.dataType==kTypeDouble) { \
		switch(tos.dataType) { \
			case kTypeInt: /* double x int -> double */ \
				second.doubleValue=FUNC(second.doubleValue,(double)tos.intValue); \
				break; \
			case kTypeLong: /* double x long -> double */ \
				second.doubleValue=FUNC(second.doubleValue,(double)tos.longValue); \
				break; \
			case kTypeFloat: /* double x float -> double */ \
				second.doubleValue=FUNC(second.doubleValue,(double)tos.floatValue); \
				break; \
			case kTypeDouble: /* double x double -> dobule */ \
				second.doubleValue=FUNC(second.doubleValue,tos.doubleValue); \
				break; \
			case kTypeBigInt: { /* double x bigInt -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(second.doubleValue), \
								   BigFloat(*tos.bigIntPtr)); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigFloat: { /* double x bigFloat -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(second.doubleValue),*tos.bigFloatPtr); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError; \
		} \
	} else if(second.dataType==kTypeFloat) { \
		switch(tos.dataType) { \
			case kTypeInt: /* float x int -> float */ \
				second.floatValue=(float)FUNC((double)second.floatValue, \
											  (double)tos.intValue); \
				break; \
			case kTypeLong: /* float x long -> float*/ \
				second.floatValue=(float)FUNC((double)second.floatValue, \
											  (double)tos.longValue); \
				break; \
			case kTypeFloat: /* float x float -> float */ \
				second.floatValue=(float)FUNC((double)second.floatValue, \
											  (double)tos.floatValue); \
				break; \
			case kTypeDouble: /* float x double -> dobule */ \
				second.doubleValue=FUNC((double)second.floatValue,tos.doubleValue); \
				second.dataType=kTypeDouble; \
				break; \
			case kTypeBigInt: { /* float x bigInt -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(second.floatValue), \
								   BigFloat(*tos.bigIntPtr)); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigFloat: { /* float x bigFloat -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(second.floatValue),*tos.bigFloatPtr); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError; \
		} \
	} else if(second.dataType==kTypeBigFloat) { \
		switch(tos.dataType) { \
			case kTypeInt: /* bigFloat x int -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr,BigFloat(tos.intValue)); \
				break; \
			case kTypeLong: /* bigFloat x long -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr,BigFloat(tos.longValue)); \
				break; \
			case kTypeFloat: /* bigFloat x float -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr, \
										 BigFloat(tos.floatValue)); \
				break; \
			case kTypeDouble: /* bigFloat x double -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr, \
										 BigFloat(tos.doubleValue)); \
				break; \
			case kTypeBigInt: /* bigFloat x bigInt -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr, \
										 BigFloat(*tos.bigIntPtr)); \
				break; \
			case kTypeBigFloat: /* bigFloat x bigFloat -> bigFloat */ \
				*second.bigFloatPtr=FUNC(*second.bigFloatPtr, *tos.bigFloatPtr); \
				break; \
			default: goto onError; \
		} \
	} else if(second.dataType==kTypeBigInt) { \
		switch(tos.dataType) { \
			case kTypeInt:	/* bigInt x int -> bigInt */ \
				*second.bigIntPtr=FUNC(*second.bigIntPtr,tos.intValue); \
				break; \
			case kTypeLong:	/* bigInt x long -> bigInt */ \
				*second.bigIntPtr=FUNC(*second.bigIntPtr,tos.longValue); \
				break; \
			case kTypeFloat: {	/* bigInt x float -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(*second.bigIntPtr), \
								   BigFloat(tos.floatValue)); \
					delete(second.bigIntPtr); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeDouble: {	/* bigInt x double -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(*second.bigIntPtr), \
								   BigFloat(tos.doubleValue)); \
					delete(second.bigIntPtr); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigInt: /* bigInt x bigInt -> bigInt */ \
				*second.bigIntPtr=FUNC(*second.bigIntPtr,*tos.bigIntPtr); \
				break; \
			case kTypeBigFloat: { /* bigInt x bigFloat -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=FUNC(BigFloat(*second.bigIntPtr),*tos.bigFloatPtr); \
					delete(second.bigIntPtr); \
					second.bigFloatPtr=bigFloat; \
					second.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError; \
		} \
	} else { \
onError: \
		return inContext.Error(E_INVALID_DATA_TYPE_TOS_SECOND,tos,second); \
	} \
} while(0)

#define OneParamFunc(FUNC) do { \
	if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	switch(tos.dataType) { \
		case kTypeInt: \
			tos.doubleValue=FUNC((double)tos.intValue); \
			tos.dataType=kTypeDouble; \
			break; \
		case kTypeLong: \
			tos.doubleValue=FUNC((double)tos.longValue); \
			tos.dataType=kTypeDouble; \
			break; \
		case kTypeFloat: \
			tos.doubleValue=FUNC((double)tos.floatValue); \
			tos.dataType=kTypeDouble; \
			break; \
		case kTypeDouble: \
			tos.doubleValue=FUNC(tos.doubleValue); \
			break; \
		case kTypeBigInt: { \
				BigFloat *bigFloat=new BigFloat(*tos.bigIntPtr); \
				*bigFloat=FUNC(*bigFloat); \
				delete tos.bigIntPtr; \
				tos.bigFloatPtr=bigFloat; \
				tos.dataType=kTypeBigFloat; \
			} \
			break; \
		case kTypeBigFloat: \
			*tos.bigFloatPtr=FUNC(*tos.bigFloatPtr); \
			break; \
		default: \
			return inContext.Error(E_TOS_NUMBER,tos); \
	} \
	NEXT; \
} while(0)

// c=a%b
// ModOp(c,a,b)
#define ModOp(outDest,inSrc1,inSrc2) do { \
	if(inSrc1.dataType==kTypeInt && inSrc2.dataType==kTypeInt) { \
		outDest=TypedValue(inSrc1.intValue % inSrc2.intValue); \
	} else if(inSrc1.dataType==kTypeLong && inSrc2.dataType==kTypeLong) { \
		outDest=TypedValue(inSrc1.longValue % inSrc2.longValue); \
	} else if(inSrc1.dataType==kTypeBigInt && inSrc2.dataType==kTypeBigInt) { \
		outDest=TypedValue(*(inSrc1.bigIntPtr) % *(inSrc2.bigIntPtr)); \
	} else { \
		if(inSrc1.dataType==kTypeInt) { \
			switch(inSrc2.dataType) { \
				case kTypeLong: \
					outDest=TypedValue((long)inSrc1.intValue % inSrc2.longValue); \
					break; \
				case kTypeBigInt: \
					outDest=TypedValue(BigInt(inSrc1.intValue) % *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		  } else if(inSrc1.dataType==kTypeLong) { \
			switch(inSrc2.dataType) { \
				case kTypeInt: \
					outDest=TypedValue(inSrc1.longValue % (long)inSrc2.intValue); \
					break; \
				case kTypeBigInt: \
					outDest=TypedValue(BigInt(inSrc1.longValue) \
								  		% *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		} else if(inSrc1.dataType==kTypeBigInt) { \
			switch(inSrc2.dataType) { \
				case kTypeInt: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.intValue); \
					break; \
				case kTypeLong: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.longValue); \
					break; \
				default: \
					return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		} else { \
			return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,inSrc1); \
		} \
	} \
} while(0)

// a %= b
// ModAssign(a,b);
#define ModAssign(inTarget,inRef) do { \
	if(inRef.dataType!=kTypeInt \
	  && inRef.dataType!=kTypeLong \
	  && inRef.dataType!=kTypeBigInt) { \
		return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inRef); \
	} \
	if(inTarget.dataType!=kTypeInt \
	  && inTarget.dataType!=kTypeLong \
	  && inTarget.dataType!=kTypeBigInt) { \
		return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,inTarget); \
	} \
	if(inTarget.dataType==kTypeInt && inRef.dataType==kTypeInt) { \
		inTarget.intValue %= inRef.intValue; \
	} else if(inTarget.dataType==kTypeLong && inRef.dataType==kTypeLong) { \
		inTarget.longValue %= inRef.longValue; \
	} else if(inTarget.dataType==kTypeBigInt && inRef.dataType==kTypeBigInt) { \
		*inTarget.bigIntPtr %= *inRef.bigIntPtr; \
	} else { \
		switch(inTarget.dataType) { \
			case kTypeInt: \
				assert(inRef.dataType==kTypeLong || inRef.dataType==kTypeBigInt); \
				if(inRef.dataType==kTypeLong) { \
					inTarget.dataType=kTypeLong; \
					inTarget.longValue=(long)inTarget.intValue % inRef.longValue; \
				} else { \
					assert(inRef.dataType==kTypeBigInt); \
					inTarget.dataType=kTypeBigInt; \
					inTarget.bigIntPtr=new BigInt(inTarget.intValue); \
					*inTarget.bigIntPtr %= *inRef.bigIntPtr; \
				} \
				break; \
			case kTypeLong: \
				assert(inRef.dataType==kTypeInt || inRef.dataType==kTypeBigInt); \
				if(inRef.dataType==kTypeInt) { \
					inTarget.longValue=inTarget.longValue % (long)inRef.intValue; \
				} else { \
					assert(inRef.dataType==kTypeBigInt); \
					inTarget.dataType=kTypeBigInt; \
					inTarget.bigIntPtr=new BigInt(inTarget.longValue); \
					*inTarget.bigIntPtr %= *inRef.bigIntPtr; \
				} \
				break; \
			case kTypeBigInt: \
				assert(inRef.dataType==kTypeInt || inRef.dataType==kTypeLong); \
				if(inRef.dataType==kTypeInt) { \
					*inTarget.bigIntPtr %= inRef.intValue; \
				} else { \
					assert(inRef.dataType==kTypeLong); \
					*inTarget.bigIntPtr %= inRef.longValue; \
				} \
				break; \
			default: \
				inContext.Error(E_SYSTEM_ERROR); \
				exit(-1); \
		} \
	} \
} while(0)

// Push(a % b)
// RefMod(inContext.DS,a,b);
#define RefMod(inStack,inSecond,inTos) do { \
	if(inTos.dataType!=kTypeInt \
	  && inTos.dataType!=kTypeLong \
	  && inTos.dataType!=kTypeBigInt) { \
		return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
	} \
	if(inSecond.dataType!=kTypeInt \
	  && inSecond.dataType!=kTypeLong \
	  && inSecond.dataType!=kTypeBigInt) { \
		return inContext.Error(E_SECOND_INT_OR_LONG_OR_BIGINT,inSecond); \
	} \
	if(inSecond.dataType==kTypeInt && inTos.dataType==kTypeInt) { \
		inStack.emplace_back(inSecond.intValue % inTos.intValue); \
	} else if(inSecond.dataType==kTypeLong && inTos.dataType==kTypeLong) { \
		inStack.emplace_back(inSecond.longValue % inTos.longValue); \
	} else if(inSecond.dataType==kTypeBigInt && inTos.dataType==kTypeBigInt) { \
		inStack.emplace_back(*(inSecond.bigIntPtr) % *(inTos.bigIntPtr)); \
	} else { \
		switch(inSecond.dataType) { \
			case kTypeInt: \
				assert(inTos.dataType==kTypeLong || inTos.dataType==kTypeBigInt); \
				if(inTos.dataType==kTypeLong) { /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue % inTos.longValue); \
				} else { \
					assert(inTos.dataType==kTypeBigInt); \
					/* int x bigIng -> bigInt */ \
					inStack.emplace_back(inSecond.intValue % *(inTos.bigIntPtr)); \
				} \
				break; \
			case kTypeLong: \
				assert(inTos.dataType==kTypeInt || inTos.dataType==kTypeBigInt); \
				if(inTos.dataType==kTypeInt) { /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue % (long)inTos.intValue); \
				} else { \
					assert(inTos.dataType==kTypeBigInt); \
					/* long x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.longValue % *(inTos.bigIntPtr)); \
				} \
				break; \
			case kTypeBigInt: \
				assert(inTos.dataType==kTypeInt || inTos.dataType==kTypeLong); \
				if(inTos.dataType==kTypeInt) { /* bigInt x int -> bigInt  */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.intValue); \
				} else { \
					assert(inTos.dataType==kTypeLong); \
					/* bigInt x long -> bigInt  */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.longValue); \
				} \
				break; \
			default: \
				inContext.Error(E_SYSTEM_ERROR); \
				exit(-1); \
		} \
	} \
} while(0)

#define FloorOrCeil(FUNC,ioTV) do { \
	switch(ioTV.dataType) { \
		case kTypeInt: \
		case kTypeLong: \
		case kTypeBigInt: \
			break; \
		case kTypeFloat: { \
				float f=FUNC(ioTV.floatValue); \
				if(INT_MIN<=f && f<=INT_MAX) { \
					ioTV.intValue=(int)f; \
					ioTV.dataType=kTypeInt; \
				} else if(LONG_MIN<=f && f<=LONG_MAX) { \
					ioTV.longValue=(long)f; \
					ioTV.dataType=kTypeLong; \
				} else { \
					ioTV.bigIntPtr=new BigInt(f); \
					ioTV.dataType=kTypeBigInt; \
				} \
			} \
			break; \
		case kTypeDouble: { \
				double t=FUNC(ioTV.doubleValue); \
				if(INT_MIN<=t && t<=INT_MAX) { \
					ioTV.intValue=(int)t; \
					ioTV.dataType=kTypeInt; \
				} else if(LONG_MIN<=t && t<=LONG_MAX) { \
					ioTV.longValue=(long)t; \
					ioTV.dataType=kTypeLong; \
				} else { \
					ioTV.bigIntPtr=new BigInt(t); \
					ioTV.dataType=kTypeBigInt; \
				} \
			} \
			break; \
		case kTypeBigFloat: \
			ioTV.bigIntPtr=new BigInt(FUNC(*ioTV.bigFloatPtr)); \
			ioTV.dataType=kTypeBigInt; \
			break; \
		default: \
			return inContext.Error(E_TOS_NUMBER,ioTV); \
	} \
} while(0)
