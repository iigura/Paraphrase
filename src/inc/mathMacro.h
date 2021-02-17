#pragma once

#ifdef _WIN32
	#define DISABLE_WARNING_ARRAY_BOUNDS
	#define ENABLE_WARNING_ARRAY_BOUNDS
#else
	#define DISABLE_WARNING_ARRAY_BOUNDS \
				_Pragma("GCC diagnostic ignored \"-Warray-bounds\"") 
	#define ENABLE_WARNING_ARRAY_BOUNDS \
				_Pragma("GCC diagnostic warning \"-Warray-bounds\"")
#endif

extern BigInt G_BI_DBL_MAX, G_BI_MINUS_DBL_MAX;

#define MakeLabel(name,lineNo) __##name##lineNo

#define SKIP(x)

#define IS_PLUS(OP) (#OP[0]=='+' && #OP[1]=='\0')
#define IS_DIV(OP)  (#OP[0]=='/' && #OP[1]=='\0')
#define IS_GT(OP)	(#OP[0]=='>' && #OP[1]=='\0')
#define IS_GE(OP)	(#OP[0]=='>' && #OP[1]=='=' && #OP[2]=='\0')
#define IS_LT(OP)	(#OP[0]=='<' && #OP[1]=='\0')
#define IS_LE(OP)	(#OP[0]=='<' && #OP[1]=='=' && #OP[2]=='\0')
#define IS_EQ(OP)	(#OP[0]=='=' && #OP[1]=='=' && #OP[2]=='\0')
#define IS_NE(OP)	(#OP[0]=='!' && #OP[1]=='=' && #OP[2]=='\0')

// c=a OP b
// MathOp(c,a,OP,b);
#define MathOp(outDest,inSrc1,OP,inSrc2) do { \
	int combinationType=GetMathOpType(inSrc1.dataType,inSrc2.dataType); \
	SKIP( if(combinationType>35) { \
		fprintf(stderr,"SYSTEM ERROR\n"); \
		exit(-1); \
	} ) \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::kIntInt: \
			outDest.Set(inSrc1.intValue OP inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kIntLong: \
			outDest=TypedValue((long)inSrc1.intValue OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kIntBigInt: \
			outDest=TypedValue(inSrc1.intValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::kIntFloat: \
			outDest.Set((float)inSrc1.intValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::kIntDouble: \
			outDest.Set((double)inSrc1.intValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::kIntBigFloat: \
			outDest=TypedValue(inSrc1.intValue OP *(inSrc2.bigFloatPtr)); \
			break; \
		case TypeCombinationForMathOP::kLongInt: \
			outDest=TypedValue(inSrc1.longValue OP (long)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kLongLong: \
			outDest=TypedValue(inSrc1.longValue OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kLongBigInt: \
			outDest=TypedValue(inSrc1.longValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::kLongFloat: \
			outDest.Set((float)inSrc1.longValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::kLongDouble: \
			outDest.Set((double)inSrc1.longValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::kLongBigFloat: \
			outDest=TypedValue(inSrc1.longValue OP *(inSrc2.bigFloatPtr)); \
			break; \
		case TypeCombinationForMathOP::kBigIntInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kBigIntLong: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kBigIntBigInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::kBigIntFloat: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP BigFloat(inSrc2.floatValue)); \
			break; \
		case TypeCombinationForMathOP::kBigIntDouble: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP BigFloat(inSrc2.doubleValue)); \
			break; \
		case TypeCombinationForMathOP::kBigIntBigFloat: \
			outDest=TypedValue(BigFloat(*(inSrc1.bigIntPtr)) \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case TypeCombinationForMathOP::kFloatInt: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kFloatLong: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kFloatBigInt: \
			outDest=TypedValue(BigFloat(inSrc1.floatValue) \
								OP BigFloat(*(inSrc2.bigIntPtr))); \
			break; \
		case TypeCombinationForMathOP::kFloatFloat: \
			outDest.Set(inSrc1.floatValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::kFloatDouble: \
			outDest.Set((double)inSrc1.floatValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::kFloatBigFloat: \
			outDest=TypedValue(BigFloat(inSrc1.floatValue) \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case TypeCombinationForMathOP::kDoubleInt: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kDoubleLong: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kDoubleBigInt: \
			outDest=TypedValue(inSrc1.doubleValue \
								OP BigFloat(*(inSrc2.bigFloatPtr))); \
			break; \
		case TypeCombinationForMathOP::kDoubleFloat: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::kDoubleDouble: \
			outDest.Set(inSrc1.doubleValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::kDoubleBigFloat: \
			outDest=TypedValue(inSrc1.doubleValue \
								OP *(inSrc2.bigFloatPtr)); \
			break; \
		case TypeCombinationForMathOP::kBigFloatInt: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::kBigFloatLong: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::kBigFloatBigInt: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) \
								OP BigFloat(*(inSrc2.bigIntPtr))); \
			break; \
		case TypeCombinationForMathOP::kBigFloatFloat: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::kBigFloatDouble: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::kBigFloatBigFloat: \
			outDest=TypedValue(*(inSrc1.bigFloatPtr) OP *(inSrc2.bigFloatPtr)); \
			break; \
		default: \
			fprintf(stderr,"SYSTEM ERROR\n"); \
			exit(-1); \
	} \
} while(0)


#define DIVIDE_BY_ZERO_AssignOpToSecond MakeLabel(divideByZero,__LINE__)
// ex: a += b
// 	   AssignOpToSecond(a,+,b)
#define AssignOpToSecond(inContext,inTarget,OP,inTV) do { \
	if(IS_PLUS(OP) /* #OP=="+" */ \
	  && inTarget.dataType==DataType::kTypeString \
	  && inTV.dataType    ==DataType::kTypeString) { \
		inTarget=TypedValue(*inTarget.stringPtr+*inTV.stringPtr); \
	} else { \
		if( IS_DIV(OP) ) { \
			switch(inTV.dataType) { \
				case DataType::kTypeInt: \
					if(inTV.intValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
					break; \
				case DataType::kTypeLong: \
					if(inTV.longValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
					break; \
				case DataType::kTypeBigInt: \
					if(*(inTV.bigIntPtr)==0) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				case DataType::kTypeFloat: \
					if(inTV.floatValue==0.0f) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				case DataType::kTypeDouble: \
					if(inTV.doubleValue==0.0) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				case DataType::kTypeBigFloat: \
					if(*(inTV.bigFloatPtr)==0.0) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				default: \
				 	return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,inTV); \
DIVIDE_BY_ZERO_AssignOpToSecond: \
					return inContext.Error(NoParamErrorID::E_TOS_SHOULD_BE_NONZERO); \
			} \
		} \
		const int combinationType=GetMathOpType(inTarget.dataType,inTV.dataType); \
		if(combinationType>=(int)TypeCombinationForMathOP::kInvalidMathOpTypeThreshold) { \
			return inContext.Error(InvalidTypeTosSecondErrorID::E_INVALID_DATA_TYPE_TOS_SECOND,inTV,inTarget); \
		} \
		switch((TypeCombinationForMathOP)combinationType) { \
			case TypeCombinationForMathOP::kIntInt:  \
				inTarget.intValue=inTarget.intValue OP inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::kIntLong: \
				inTarget.longValue=(long)inTarget.intValue OP inTV.longValue; \
				inTarget.dataType=DataType::kTypeLong; \
				break; \
			case TypeCombinationForMathOP::kIntBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.intValue OP *inTV.bigIntPtr); \
				inTarget.dataType=DataType::kTypeBigInt; \
				break; \
			case TypeCombinationForMathOP::kIntFloat: \
				inTarget.floatValue=(float)inTarget.intValue OP inTV.floatValue; \
				inTarget.dataType=DataType::kTypeFloat; \
				break; \
			case TypeCombinationForMathOP::kIntDouble: \
				inTarget.doubleValue=(double)inTarget.intValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kIntBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.intValue OP *inTV.bigFloatPtr); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kLongInt: \
				inTarget.longValue=inTarget.longValue OP (long)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::kLongLong: \
				inTarget.longValue=inTarget.longValue OP inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::kLongBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.longValue OP *inTV.bigIntPtr); \
				inTarget.dataType=DataType::kTypeBigInt; \
			case TypeCombinationForMathOP::kLongFloat: \
				inTarget.floatValue=(float)inTarget.longValue OP inTV.floatValue; \
				inTarget.dataType=DataType::kTypeFloat; \
				break; \
			case TypeCombinationForMathOP::kLongDouble: \
				inTarget.doubleValue=(double)inTarget.longValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kLongBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.longValue OP *inTV.bigFloatPtr); \
				inTarget.dataType=DataType::kTypeBigFloat; \
			case TypeCombinationForMathOP::kBigIntInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::kBigIntLong: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::kBigIntBigInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP *inTV.bigIntPtr; \
				break; \
			case TypeCombinationForMathOP::kBigIntFloat: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inTV.floatValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigIntDouble: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inTV.doubleValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigIntBigFloat: { \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=BigFloat(*inTarget.bigIntPtr) OP *inTV.bigFloatPtr; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kFloatInt: \
				inTarget.floatValue=inTarget.floatValue OP (float)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::kFloatLong: \
				inTarget.floatValue=inTarget.floatValue OP (float)inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::kFloatBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					*bigFloat=inTarget.floatValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kFloatFloat: \
				inTarget.floatValue=inTarget.floatValue OP inTV.floatValue; \
				break; \
			case TypeCombinationForMathOP::kFloatDouble: \
				inTarget.doubleValue=(double)inTarget.floatValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kFloatBigFloat: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					*bigFloat=inTarget.floatValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kDoubleInt: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::kDoubleLong: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::kDoubleBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					*bigFloat=inTarget.doubleValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
				} \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kDoubleFloat: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.floatValue; \
				break; \
			case TypeCombinationForMathOP::kDoubleDouble: \
				inTarget.doubleValue=inTarget.doubleValue OP inTV.doubleValue; \
				break; \
			case TypeCombinationForMathOP::kDoubleBigFloat: \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.doubleValue OP *inTV.bigFloatPtr); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigFloatInt: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inTV.intValue); \
				break; \
			case TypeCombinationForMathOP::kBigFloatLong: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inTV.longValue); \
				break; \
			case TypeCombinationForMathOP::kBigFloatBigInt: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(*inTV.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::kBigFloatFloat: \
				*inTarget.bigFloatPtr \
					= *inTarget.bigFloatPtr OP BigFloat(inTV.floatValue); \
				break; \
			case TypeCombinationForMathOP::kBigFloatDouble: \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inTV.doubleValue); \
				break; \
			case TypeCombinationForMathOP::kBigFloatBigFloat: \
				*inTarget.bigFloatPtr = *inTarget.bigFloatPtr OP *inTV.bigFloatPtr; \
				break; \
			default: \
				return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
												   std::string(#OP),inTarget,inTV); \
		} \
	} \
} while(0)

// a b --- a c(=a+b)
// ex: b = a + b
// 	   AssignOpToTOS(a,+,b)
#define AssignOpToTOS(inContext,inTV,OP,inTarget) do { \
	if(IS_PLUS(IO) /* #OP=="+" */ \
	   && inTV.dataType==DataType::kTypeString \
	   && inTarget.dataType==DataType::kTypeString) { \
		inTarget=TypedValue(*inTV.stringPtr+*inTarget.stringPtr); \
	} else { \
		const int combinationType=GetMathOpType(inTV.dataType,inTarget.dataType); \
		if(combinationType>=(int)TypeCombinationForMathOP::kInvalidMathOpTypeThreshold) { \
			return inContext.Error(InvalidTypeTosSecondErrorID::E_INVALID_DATA_TYPE_TOS_SECOND,inTarget,inTV); \
		} \
		switch((TypeCombinationForMathOP)combinationType) { \
			case TypeCombinationForMathOP::kIntInt:  \
				inTarget.intValue=inTV.intValue OP inTarget.intValue; \
				break; \
			case TypeCombinationForMathOP::kIntLong: \
				inTarget.longValue=(long)inTV.intValue OP inTarget.longValue; \
				break; \
			case TypeCombinationForMathOP::kIntBigInt: \
				*inTarget.bigIntPtr=inTV.intValue OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::kIntFloat: \
				inTarget.floatValue=inTV.intValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::kIntDouble: \
				inTarget.doubleValue=inTV.intValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::kIntBigFloat: \
				*inTarget.bigFloatPtr=inTV.intValue OP (*inTarget.bigFloatPtr); \
				break; \
			case TypeCombinationForMathOP::kLongInt: \
				inTarget.longValue=inTV.longValue OP inTarget.intValue; \
				inTarget.dataType=DataType::kTypeLong; \
				break; \
			case TypeCombinationForMathOP::kLongLong: \
				inTarget.longValue=inTV.longValue OP inTarget.longValue; \
				break; \
			case TypeCombinationForMathOP::kLongBigInt: \
				*inTarget.bigIntPtr=inTV.longValue OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::kLongFloat: \
				inTarget.floatValue=(float)inTV.longValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::kLongDouble: \
				inTarget.doubleValue=(double)inTV.longValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::kLongBigFloat: \
				*inTarget.bigFloatPtr=inTV.longValue OP (*inTarget.bigFloatPtr); \
				break; \
			case TypeCombinationForMathOP::kBigIntInt: \
				inTarget.bigIntPtr=new BigInt(*inTV.bigIntPtr OP inTarget.intValue); \
				inTarget.dataType=DataType::kTypeBigInt; \
				break; \
			case TypeCombinationForMathOP::kBigIntLong: \
				inTarget.bigIntPtr=new BigInt(*inTV.bigIntPtr OP inTarget.longValue); \
				inTarget.dataType=DataType::kTypeBigInt; \
				break; \
			case TypeCombinationForMathOP::kBigIntBigInt: \
				*inTarget.bigIntPtr=(*inTV.bigIntPtr) OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::kBigIntFloat: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					inTarget.bigFloatPtr=new BigFloat(*bigFloat OP inTarget.floatValue); \
					delete(bigFloat); \
					inTarget.dataType=DataType::kTypeBigFloat; \
				} \
				break; \
			case TypeCombinationForMathOP::kBigIntDouble: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					inTarget.bigFloatPtr=new BigFloat(*bigFloat OP inTarget.doubleValue); \
					delete(bigFloat); \
					inTarget.dataType=DataType::kTypeBigFloat; \
				} \
				break; \
			case TypeCombinationForMathOP::kBigIntBigFloat: { \
					BigFloat *bigFloat=new BigFloat(*inTV.bigIntPtr); \
					*inTarget.bigFloatPtr=(*bigFloat) OP (*inTarget.bigFloatPtr); \
					delete(bigFloat); \
				} \
				break; \
			case TypeCombinationForMathOP::kFloatInt: \
				inTarget.floatValue=inTV.floatValue OP (float)inTarget.intValue; \
				inTarget.dataType=DataType::kTypeFloat; \
				break; \
			case TypeCombinationForMathOP::kFloatLong: \
				inTarget.floatValue=inTV.floatValue OP (float)inTarget.longValue; \
				inTarget.dataType=DataType::kTypeFloat; \
				break; \
			case TypeCombinationForMathOP::kFloatBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=new BigFloat(inTV.floatValue OP *bigFloat); \
					delete(bigFloat); \
					inTarget.dataType=DataType::kTypeBigFloat; \
				} \
				break; \
			case TypeCombinationForMathOP::kFloatFloat: \
				inTarget.floatValue=inTV.floatValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::kFloatDouble: \
				inTarget.doubleValue=inTV.floatValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::kFloatBigFloat: \
				*inTarget.bigFloatPtr=inTV.floatValue OP (*inTarget.bigFloatPtr); \
				break; \
			case TypeCombinationForMathOP::kDoubleInt: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.intValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kDoubleLong: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.longValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kDoubleBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
					*inTarget.bigFloatPtr=inTV.doubleValue OP *bigFloat; \
					inTarget.dataType=DataType::kTypeBigFloat; \
				} \
				break; \
			case TypeCombinationForMathOP::kDoubleFloat: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.floatValue; \
				inTarget.dataType=DataType::kTypeDouble; \
				break; \
			case TypeCombinationForMathOP::kDoubleDouble: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::kDoubleBigFloat: \
				*inTarget.bigFloatPtr=inTV.doubleValue OP *inTarget.bigFloatPtr; \
				break; \
			case TypeCombinationForMathOP::kBigFloatInt: \
				inTarget.bigFloatPtr=new BigFloat(*inTV.bigFloatPtr \
												  OP inTarget.intValue); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigFloatLong: \
				inTarget.bigFloatPtr=new BigFloat(*inTV.bigFloatPtr \
												  OP inTarget.longValue); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigFloatBigInt: { \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
					*inTarget.bigFloatPtr=*inTV.bigFloatPtr OP *inTarget.bigFloatPtr; \
					inTarget.dataType=DataType::kTypeBigFloat; \
				} \
				break; \
			case TypeCombinationForMathOP::kBigFloatFloat: \
				inTarget.bigFloatPtr=new BigFloat(*inTV.bigFloatPtr \
												  OP inTarget.floatValue); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigFloatDouble: \
				inTarget.bigFloatPtr=new BigFloat(*inTV.bigFloatPtr \
												  OP inTarget.doubleValue); \
				inTarget.dataType=DataType::kTypeBigFloat; \
				break; \
			case TypeCombinationForMathOP::kBigFloatBigFloat: \
				*inTarget.bigFloatPtr=*inTV.bigFloatPtr OP *inTarget.bigFloatPtr; \
				break; \
			default: \
				return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
												   std::string(#OP),inTV,inTarget); \
		} \
	} \
} while(0)

#define TwoOpAssignTOS(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	AssignOpToTOS(inContext,second,OP,tos); \
	NEXT; \
} while(0)

// Push(a + b)
// PushOp(inContext.DS,a,+,b)
#define PushOp(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==DataType::kTypeInt \
	   && inTos.dataType==DataType::kTypeInt) { \
		/* int x int -> int */ \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==DataType::kTypeLong \
			  && inTos.dataType==DataType::kTypeLong) { \
		/* long x long -> long */ \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==DataType::kTypeFloat \
			  && inTos.dataType==DataType::kTypeFloat) { \
		/* float x float -> float */ \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==DataType::kTypeDouble \
			  && inTos.dataType==DataType::kTypeDouble) { \
		/* double x double -> double */ \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==DataType::kTypeBigInt \
			  && inTos.dataType==DataType::kTypeBigInt) { \
		/* bigInt x bigInt -> bigInt */ \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else if(inSecond.dataType==DataType::kTypeBigFloat \
			  && inTos.dataType==DataType::kTypeBigFloat) { \
		/* bigFloat x bigFloat -> bigFloat */ \
		inStack.emplace_back(*(inSecond.bigFloatPtr) OP *(inTos.bigFloatPtr)); \
	} else if(inSecond.dataType==DataType::kTypeInt) { \
		switch(inTos.dataType) { \
			case DataType::kTypeLong:		/* int x long -> long */ \
				inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
				break; \
			case DataType::kTypeFloat:	/* int x float -> float */ \
				inStack.emplace_back((float)inSecond.intValue OP inTos.floatValue); \
				break; \
			case DataType::kTypeDouble:	/* int x double -> double */ \
				inStack.emplace_back((double)inSecond.intValue OP inTos.doubleValue); \
				break; \
			case DataType::kTypeBigInt: 	/* int x BigInt -> BigInt */ \
				inStack.emplace_back(BigInt(inSecond.intValue OP *inTos.bigIntPtr)); \
				break; \
			case DataType::kTypeBigFloat: /* int x BigFloat -> BigFloat */ \
				inStack.emplace_back( \
							BigFloat(inSecond.intValue OP *inTos.bigFloatPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::kTypeLong) { \
		switch(inTos.dataType) { \
			case DataType::kTypeInt:		/* long x int -> long */ \
				inStack.emplace_back(inSecond.longValue OP (long)inTos.intValue); \
				break; \
			case DataType::kTypeFloat:	/* long x float -> float */ \
				inStack.emplace_back((float)inSecond.longValue OP inTos.floatValue); \
				break; \
			case DataType::kTypeDouble:	/* long x double -> double */ \
				inStack.emplace_back((double)inSecond.longValue OP inTos.doubleValue); \
				break; \
			case DataType::kTypeBigInt: 	/* long x bigInt -> bigInt */ \
				inStack.emplace_back(BigInt(inSecond.longValue OP *inTos.bigIntPtr)); \
				break; \
			case DataType::kTypeBigFloat:	/* long x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.longValue) OP *inTos.bigFloatPtr); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::kTypeFloat) { \
		switch(inTos.dataType) { \
			case DataType::kTypeInt:		/* float x int -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.intValue); \
				break; \
			case DataType::kTypeLong:		/* float x long -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.longValue); \
				break; \
			case DataType::kTypeDouble:	/* float x double -> double */ \
				inStack.emplace_back( \
					(double)inSecond.floatValue OP inTos.doubleValue); \
				break; \
			case DataType::kTypeBigInt: 	/* float x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					inSecond.floatValue OP BigFloat(*inTos.bigIntPtr)); \
				break; \
			case DataType::kTypeBigFloat:	/* float x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.floatValue OP *(inTos.bigFloatPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::kTypeDouble) { \
		switch(inTos.dataType) { \
			case DataType::kTypeInt:		/* double x int -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.intValue); \
				break; \
			case DataType::kTypeLong:		/* double x long -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.longValue); \
				break; \
			case DataType::kTypeFloat:	/* double x float -> double */ \
				inStack.emplace_back( \
					inSecond.doubleValue OP (double)inTos.floatValue); \
				break; \
			case DataType::kTypeBigInt: 	/* double x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.doubleValue) OP BigFloat(*(inTos.bigIntPtr))); \
				break; \
			case DataType::kTypeBigFloat:	/* double x bigFloat -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(inSecond.doubleValue OP *(inTos.bigFloatPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::kTypeBigInt) { \
		switch(inTos.dataType) { \
			case DataType::kTypeInt:		/* bigInt x int -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
				break; \
			case DataType::kTypeLong: 	/* bigInt x long -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
				break; \
			case DataType::kTypeFloat:	/* bigInt x float -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP BigFloat(inTos.floatValue)); \
				break; \
			case DataType::kTypeDouble:	/* bigInt x double -> bigFloat */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP inTos.doubleValue); \
				break; \
			case DataType::kTypeBigFloat:	/* bigInt x bigFloat -> bigFlaot */ \
				inStack.emplace_back( \
					BigFloat(*(inSecond.bigIntPtr)) OP *(inTos.bigFloatPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::kTypeBigFloat) { \
		switch(inTos.dataType) { \
			case DataType::kTypeInt: 		/* bigFloat x int -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.intValue); \
				break; \
			case DataType::kTypeLong: 	/* bigFloat x long -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.longValue); \
				break; \
			case DataType::kTypeFloat:	/* bigFloat x float -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.floatValue); \
				break; \
			case DataType::kTypeDouble:	/* bigFloat x double -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP inTos.doubleValue); \
				break; \
			case DataType::kTypeBigInt:	/* bigFloat x bigInt -> bigFloat */ \
				inStack.emplace_back( \
					*(inSecond.bigFloatPtr) OP BigFloat(*(inTos.bigIntPtr))); \
				break; \
			default: goto onError;	\
		} \
	} else if((#OP[0]=='+' && #OP[1]=='\0') \
			 && inSecond.dataType==DataType::kTypeString \
			 && inTos.dataType==DataType::kTypeString) { \
		inStack.emplace_back(TypedValue(*(inSecond.stringPtr)+*(inTos.stringPtr))); \
	} else { \
onError: \
		return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
										   std::string(#OP),inSecond,inTos); \
	} \
} while(0)

// PushFuncResult(inStack, OP, a,b) == push(func(a,b))
// usage:
// 	TypedValue tos=Pop(inContext.DS);
// 	TypedValue second=Pop(inContext.DS);
// 	PushOutOfOrderFunc(inContext.DS,maxOp,second,tos)
//
#define PushFuncResult(inStack,OP,inSecond,inTos) do { \
	if(inSecond.dataType==inTos.dataType) { \
		/* same data type */ \
		switch(inSecond.dataType) { \
			case DataType::kTypeInt: \
				inStack.emplace_back(OP(inSecond.intValue,inTos.intValue)); \
				break; \
			case DataType::kTypeLong: \
				inStack.emplace_back(OP(inSecond.longValue,inTos.longValue)); \
				break; \
			case DataType::kTypeFloat: \
				inStack.emplace_back(OP(inSecond.floatValue,inTos.floatValue)); \
				break; \
			case DataType::kTypeDouble: \
				inStack.emplace_back(OP(inSecond.doubleValue,inTos.doubleValue)); \
				break; \
			case DataType::kTypeBigInt: \
				inStack.emplace_back(OP(*(inSecond.bigIntPtr),*(inTos.bigIntPtr))); \
				break; \
			case DataType::kTypeBigFloat: \
				inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
										*(inTos.bigFloatPtr))); \
				break; \
			default: \
				inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
		}	\
	} else { \
		/* different data type */ \
		if(inSecond.dataType==DataType::kTypeInt) { \
			switch(inTos.dataType) { \
				case DataType::kTypeLong: /* (int,long) */ \
					inStack.emplace_back(OP((long)inSecond.intValue, \
											inTos.longValue)); \
					break; \
				case DataType::kTypeFloat: /* (int,float) */ \
					inStack.emplace_back(OP((float)inSecond.intValue, \
											inTos.floatValue)); \
					break; \
				case DataType::kTypeDouble: /* (int,double) */ \
					inStack.emplace_back(OP((double)inSecond.intValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::kTypeBigInt: /* (int,big-ing) */ \
					inStack.emplace_back(OP(BigInt(inSecond.intValue), \
											*(inTos.bigIntPtr))); \
					break; \
				case DataType::kTypeBigFloat: /* (int,big-float) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.intValue), \
											*(inTos.bigFloatPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else if(inSecond.dataType==DataType::kTypeLong) { \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* (long,int) */ \
					inStack.emplace_back(OP(inSecond.longValue, \
											(long)inTos.intValue)); \
					break; \
				case DataType::kTypeFloat: /* (long,float) */ \
					inStack.emplace_back(OP((float)inSecond.longValue, \
											inTos.floatValue)); \
					break; \
				case DataType::kTypeDouble: /* (long,double) */ \
					inStack.emplace_back(OP((double)inSecond.longValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::kTypeBigInt: /* (long,big-int) */ \
					inStack.emplace_back(OP(BigInt(inSecond.longValue), \
											*(inTos.bigIntPtr))); \
					break; \
				case DataType::kTypeBigFloat: /* (long,big-float) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.longValue), \
											*(inTos.bigFloatPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else if(inSecond.dataType==DataType::kTypeFloat) { \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* (float,int) */ \
					inStack.emplace_back(OP(inSecond.floatValue, \
											(float)inTos.intValue)); \
					break; \
				case DataType::kTypeLong: /* (float,long) */ \
					inStack.emplace_back(OP(inSecond.floatValue, \
											(float)inTos.longValue)); \
					break; \
				case DataType::kTypeDouble: /* (float,double) */ \
					inStack.emplace_back(OP((double)inSecond.floatValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::kTypeBigInt: /* (float,big-int) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.floatValue), \
											BigFloat(*(inTos.bigIntPtr)))); \
					break; \
				case DataType::kTypeBigFloat: /* (float,big-float) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.floatValue), \
											*(inTos.bigFloatPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else if(inSecond.dataType==DataType::kTypeDouble) { \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* (double,int) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.intValue)); \
					break; \
				case DataType::kTypeLong: /* (double,long) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.longValue)); \
					break; \
				case DataType::kTypeFloat: /* (double,float) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.floatValue)); \
					break; \
				case DataType::kTypeBigInt: /* (double,big-int) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.doubleValue), \
											BigFloat(*(inTos.bigIntPtr)))); \
					break; \
				case DataType::kTypeBigFloat: /* (double,big-float) */ \
					inStack.emplace_back(OP(BigFloat(inSecond.doubleValue), \
											*(inTos.bigFloatPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else if(inSecond.dataType==DataType::kTypeBigInt) { \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* (big-int,int) */ \
					inStack.emplace_back(OP(*(inSecond.bigIntPtr), \
											BigInt(inTos.intValue))); \
					break; \
				case DataType::kTypeLong: /* (big-int,long) */ \
					inStack.emplace_back(OP(*(inSecond.bigIntPtr), \
											BigInt(inTos.longValue))); \
					break; \
				case DataType::kTypeFloat: /* (big-int,float) */ \
					inStack.emplace_back(OP(BigFloat(*(inSecond.bigIntPtr)), \
											BigFloat(inTos.floatValue))); \
					break; \
				case DataType::kTypeDouble: /* (big-int,double) */ \
					inStack.emplace_back(OP(BigFloat(*(inSecond.bigIntPtr)), \
											BigFloat(inTos.doubleValue))); \
					break; \
				case DataType::kTypeBigFloat: /* (big-int,big-float) */ \
					inStack.emplace_back(OP(BigFloat(*(inSecond.bigIntPtr)), \
											*(inTos.bigFloatPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else if(inSecond.dataType==DataType::kTypeBigFloat) { \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* (big-float,int) */ \
					inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
											BigFloat(inTos.intValue))); \
					break; \
				case DataType::kTypeLong: /* (big-float,long) */ \
					inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
											BigFloat(inTos.longValue))); \
					break; \
				case DataType::kTypeFloat: /* (big-float,float) */ \
					inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
											BigFloat(inTos.floatValue))); \
					break; \
				case DataType::kTypeDouble: /* (big-float,double) */ \
					inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
											BigFloat(inTos.doubleValue))); \
					break; \
				case DataType::kTypeBigInt: /* (big-float,big-int) */ \
					inStack.emplace_back(OP(*(inSecond.bigFloatPtr), \
											BigFloat(*(inTos.bigIntPtr)))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
			} \
		} else { \
			inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
		}\
	} \
} while(0)

#define DIVIDE_BY_ZERO_CheckIntegerAndNonZero MakeLabel(divideByZero,__LINE__)
#define CheckIntegerAndNonZero(inTV) do { \
	switch(inTV.dataType) { \
		case DataType::kTypeInt: \
			if(inTV.intValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		case DataType::kTypeLong: \
			if(inTV.longValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		case DataType::kTypeBigInt: \
			if(*(inTV.bigIntPtr)==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inTV); \
DIVIDE_BY_ZERO_CheckIntegerAndNonZero: \
		return inContext.Error(NoParamErrorID::E_TOS_SHOULD_BE_NONZERO); \
	} \
} while(0)

/* TwoOp use inContext */
#define TwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	AssignOpToSecond(inContext,second,OP,tos); \
	inContext.DS.pop_back(); \
	NEXT; \
} while(0)

#define RefTwoOp(inStack,OP) do { \
	if(inStack.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos   =ReadTOS(inStack); \
	TypedValue& second=ReadSecond(inStack); \
	PushOp(inStack,second,OP,tos); \
	NEXT; \
} while(0)

#define CmpOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(tos.dataType==DataType::kTypeInt \
	   && second.dataType==DataType::kTypeInt) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=second.intValue OP tos.intValue; \
	} else if(tos.dataType==DataType::kTypeLong \
			  && second.dataType==DataType::kTypeLong) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=second.longValue OP tos.longValue; \
	} else if(tos.dataType==DataType::kTypeDouble \
			  && second.dataType==DataType::kTypeDouble) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=second.doubleValue OP tos.doubleValue; \
	} else if(tos.dataType==DataType::kTypeFloat \
			  && second.dataType==DataType::kTypeFloat) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=second.floatValue OP tos.floatValue; \
	} else if(tos.dataType==DataType::kTypeString \
			  && second.dataType==DataType::kTypeString) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=*second.stringPtr OP *tos.stringPtr; \
	} else if(tos.dataType==DataType::kTypeBigInt \
			  && second.dataType==DataType::kTypeBigInt) { \
		second.dataType=DataType::kTypeBool; \
		second.boolValue=*(second.bigIntPtr) OP *(tos.bigIntPtr); \
	} else if(second.dataType==DataType::kTypeBool \
			  && tos.dataType==DataType::kTypeBool) { \
DISABLE_WARNING_ARRAY_BOUNDS \
		if(#OP[0]=='=' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="==" */ \
			second.boolValue = second.boolValue == tos.boolValue; \
		} else if(#OP[0]=='!' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="!=" */ \
			second.boolValue = second.boolValue != tos.boolValue; \
		} else { \
			goto onError; \
		} \
ENABLE_WARNING_ARRAY_BOUNDS \
	} else if(second.dataType==DataType::kTypeList \
			  && tos.dataType==DataType::kTypeList) { \
		if(#OP[0]=='=' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="==" */ \
			bool result = second==tos; \
			second.listPtr.reset();	\
			second.dataType=DataType::kTypeBool; \
			second.boolValue=result; \
		} else if(#OP[0]=='!' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="!=" */ \
			bool result = !(second==tos); \
			second.listPtr.reset();	\
			second.dataType=DataType::kTypeBool; \
			second.boolValue=result; \
		} else { \
			goto onError; \
		} \
	} else { \
		switch(second.dataType) { \
			case DataType::kTypeInt: \
				switch(tos.dataType) { \
					case DataType::kTypeLong: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(long)second.intValue  OP tos.longValue; \
						break; \
					case DataType::kTypeBigInt: { \
							BigInt bi(second.intValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case DataType::kTypeFloat: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(float)second.intValue OP tos.floatValue; \
						break; \
					case DataType::kTypeDouble: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=((double)second.intValue) \
											OP (tos.doubleValue); \
						break; \
					case DataType::kTypeBigFloat: { \
							BigFloat bf(second.intValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeLong: \
				switch(tos.dataType) { \
					case DataType::kTypeInt: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=second.longValue OP (long)tos.intValue; \
						break; \
					case DataType::kTypeBigInt: { \
							BigInt bi(second.longValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case DataType::kTypeFloat: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(float)second.longValue OP tos.floatValue; \
						break; \
					case DataType::kTypeDouble: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(double)second.longValue OP tos.doubleValue; \
						break; \
					case DataType::kTypeBigFloat: { \
							BigFloat bf(second.longValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeBigInt: \
				switch(tos.dataType) { \
					case DataType::kTypeInt: { \
							BigInt bi(tos.intValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case DataType::kTypeLong: { \
							BigInt bi(tos.longValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case DataType::kTypeFloat: { \
							BigFloat s(*second.bigIntPtr); \
							BigFloat t(tos.floatValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::kTypeDouble: { \
							BigFloat s(*second.bigIntPtr); \
							BigFloat t(tos.doubleValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::kTypeBigFloat: { \
							BigFloat bf(*second.bigIntPtr); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeFloat: \
				switch(tos.dataType) { \
					case DataType::kTypeInt: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=second.floatValue OP (float)tos.intValue; \
						break; \
					case DataType::kTypeLong: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(double)second.floatValue \
											OP (double)tos.longValue; \
						break; \
					case DataType::kTypeBigInt: { \
							BigFloat s(second.floatValue); \
							BigFloat t(*tos.bigIntPtr); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::kTypeDouble: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=(double)second.floatValue \
											OP tos.doubleValue; \
						break; \
					case DataType::kTypeBigFloat: { \
							BigFloat bf(second.floatValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeDouble: \
				switch(tos.dataType) { \
					case DataType::kTypeInt: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=second.doubleValue OP (double)tos.intValue; \
						break; \
					case DataType::kTypeLong: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=second.doubleValue OP (double)tos.longValue; \
						break; \
					case DataType::kTypeBigInt: { \
							BigFloat s(second.doubleValue); \
							BigFloat t(*tos.bigIntPtr); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::kTypeFloat: \
						second.dataType=DataType::kTypeBool; \
						second.boolValue=second.doubleValue \
											OP (double)tos.floatValue; \
						break; \
					case DataType::kTypeBigFloat: { \
							BigFloat bf(second.doubleValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeBigFloat: \
				switch(tos.dataType) { \
					case DataType::kTypeInt: { \
							BigFloat bf(tos.intValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case DataType::kTypeLong: { \
							BigFloat bf(tos.longValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case DataType::kTypeBigInt: { \
							BigFloat bf(*tos.bigIntPtr); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case DataType::kTypeFloat: { \
							BigFloat bf(tos.floatValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					case DataType::kTypeDouble: { \
							BigFloat bf(tos.doubleValue); \
							second.dataType=DataType::kTypeBool; \
							second.boolValue=*second.bigFloatPtr OP bf; \
						} \
						break; \
					default: \
						return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
				} \
				break; \
			default: goto onError; \
		} \
	} \
	NEXT; \
onError: \
	return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
									   std::string(#OP),second,tos); \
} while(0)

#define RefCmpOpMain(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==DataType::kTypeInt \
	   && inTos.dataType==DataType::kTypeInt) { \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==DataType::kTypeLong \
			  && inTos.dataType==DataType::kTypeLong) { \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==DataType::kTypeDouble \
			  || inTos.dataType==DataType::kTypeDouble) { \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==DataType::kTypeFloat \
			  || inTos.dataType==DataType::kTypeFloat) { \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==DataType::kTypeString \
			  && inTos.dataType==DataType::kTypeString) { \
		inStack.emplace_back(*(inSecond.stringPtr) OP *(inTos.stringPtr)); \
	} else if(inSecond.dataType==DataType::kTypeBigInt \
			  && inTos.dataType==DataType::kTypeBigInt) { \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else { \
		switch(inSecond.dataType) { \
			case DataType::kTypeInt: \
				switch(inTos.dataType) { \
					case DataType::kTypeLong: /* int x long -> bool */ \
						inStack.emplace_back( \
							(long)inSecond.intValue  OP inTos.longValue); \
						break; \
					case DataType::kTypeBigInt: /* int x bigInt -> bool */ \
						inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
						break; \
					case DataType::kTypeFloat: /* int x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.intValue OP inTos.floatValue); \
						break; \
					case DataType::kTypeDouble: /* int x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.intValue OP inTos.doubleValue); \
						break; \
					case DataType::kTypeBigFloat: /* int x bigInt -> bool */ \
						inStack.emplace_back( \
							inSecond.intValue OP *(inTos.bigIntPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeLong: \
				switch(inTos.dataType) { \
					case DataType::kTypeInt: /* long x int -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP (long)inTos.intValue); \
						break; \
					case DataType::kTypeBigInt: /* long x bigIng -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP *(inTos.bigIntPtr)); \
						break; \
					case DataType::kTypeFloat: /* long x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.longValue OP inTos.floatValue); \
						break; \
					case DataType::kTypeDouble: /* long x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.longValue OP inTos.doubleValue); \
						break; \
					case DataType::kTypeBigFloat: /* long x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeBigInt: \
				switch(inTos.dataType) { \
					case DataType::kTypeInt: /* bigInt x int -> bool */ \
						inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
						break; \
					case DataType::kTypeLong: /* bigInt x long -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigIntPtr) OP inTos.longValue); \
						break; \
					case DataType::kTypeFloat: /* bigInt x float -> bool */ \
						if(IS_GT(OP) /* #OP==">" */ \
						  && *(inSecond.bigIntPtr) > G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GT(OP) /* #OP==">" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_GT(OP) /* #OP==">" */ ) { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_GE(OP) /* #OP==">=" */ ) { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_LT(OP) /* #OP=="<" */) { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && *(inSecond.bigIntPtr) <= G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && G_BI_DBL_MAX < *(inSecond.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_LE(OP) /* #OP=="<=" */) { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else if(IS_EQ(OP) /* #OP=="==" */ \
						  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
							  || *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(false); \
						} else if(IS_EQ(OP) /* #OP=="==" */) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_NE(OP) /* #OP=="!=" */ \
						  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
								|| *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(true); \
						} else if(IS_NE(OP) /* #OP=="!=" */ ) { \
							inStack.emplace_back( \
								static_cast<float>(*(inSecond.bigIntPtr)) \
								OP inTos.floatValue); \
						} else { \
							inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
						} \
						break; \
					case DataType::kTypeDouble: /* bigInt x double -> bool */ \
						if(IS_GT(OP) /* #OP==">" */ \
					  	  && *(inSecond.bigIntPtr) > G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GT(OP) /* #OP==">" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_GT(OP) /* #OP==">" */ ) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_GE(OP) /* #OP==">=" */) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && *(inSecond.bigIntPtr) >= G_BI_DBL_MAX) { \
							inStack.emplace_back(false); \
						} else if(IS_LT(OP) /* #OP=="<" */ ) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && *(inSecond.bigIntPtr) <= G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && G_BI_DBL_MAX < *(inSecond.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_LE(OP) /*  #OP=="<=" */ ) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_EQ(OP) /* #OP=="==" */ \
						  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
							  || *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(false); \
						} else if(IS_EQ(OP) /* #OP=="==" */ ) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else if(IS_NE(OP) /* #OP=="!=" */ \
						  && (*(inSecond.bigIntPtr) > G_BI_DBL_MAX \
							  || *(inSecond.bigIntPtr) < G_BI_MINUS_DBL_MAX)) { \
							inStack.emplace_back(true); \
						} else if(IS_NE(OP) /* #OP=="!=" */ ) { \
							inStack.emplace_back( \
								static_cast<double>(*(inSecond.bigIntPtr)) \
								OP inTos.doubleValue); \
						} else { \
							inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
						} \
						break; \
					case DataType::kTypeBigFloat: /* bigInt x bigFloat -> bool */ \
						/* bigInt が bigFloat の範囲を超えた場合の挙動は未定とする */ \
						inStack.emplace_back( \
							static_cast<BigFloat>(*(inSecond.bigIntPtr)) \
							OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeFloat: \
				switch(inTos.dataType) { \
					case DataType::kTypeInt: /* float x int -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP (float)inTos.intValue); \
						break; \
					case DataType::kTypeLong: /* float x long -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.floatValue OP (double)inTos.longValue); \
						break; \
					case DataType::kTypeBigInt: /* float x bigInt -> bool */ \
						if(IS_GT(OP) /* #OP==">" */ \
						  && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_GT(OP) /* #OP==">" */ \
						  && *(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GT(OP) /* #OP==">" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_GE(OP) /* #OP==">=" */ \
						  && *(inTos.bigIntPtr)<=G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GE(OP) /* #OP==">=" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && *(inTos.bigIntPtr) <= G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(IS_LT(OP) /* #OP=="<" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && *(inTos.bigIntPtr) < G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(IS_LE(OP) /*  #OP=="<=" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(IS_EQ(OP) /* #OP=="==" */ \
						  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
							  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(false); \
						} else if(IS_EQ(OP) /* #OP=="==" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else if(IS_EQ(OP) /* #OP=="!=" */ \
						  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
							  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(true); \
						} else if(IS_EQ(OP) /* #OP=="==" */ ) { \
							inStack.emplace_back( \
								inSecond.floatValue \
								OP static_cast<float>(*(inTos.bigIntPtr))); \
						} else { \
							inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
						} \
						break; \
					case DataType::kTypeDouble: /* float x double -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP inTos.doubleValue); \
						break; \
					case DataType::kTypeBigFloat: /* float x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeDouble: \
				switch(inTos.dataType) { \
					case DataType::kTypeInt: /* double x int -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP tos.intValue); \
						break; \
					case DataType::kTypeLong: /* double x long -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP inTos.longValue); \
						break; \
					case DataType::kTypeBigInt: /* double x bigInt -> bool */ \
						if(IS_GT(OP) /* #OP==">" */ \
						  && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_GT(OP) /* #OP==">" */ \
						  && *(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GT(OP) /* #OP==">" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(IS_GT(OP) /* #OP==">=" */ \
						  && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(false); \
						} else if(IS_GT(OP) /* #OP==">=" */ \
						  && *(inTos.bigIntPtr)<=G_BI_MINUS_DBL_MAX) { \
							inStack.emplace_back(true); \
						} else if(IS_GT(OP) /* #OP==">=" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(IS_LT(OP) /* #OP=="<" */ \
						  && G_BI_DBL_MAX < *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(IS_LT(OP) /*  #OP=="<" */ \
						  && *(inTos.bigIntPtr) <= G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(IS_LT(OP) /* #OP=="<" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && G_BI_DBL_MAX <= *(inTos.bigIntPtr)) { \
							inStack.emplace_back(true); \
						} else if(IS_LE(OP) /* #OP=="<=" */ \
						  && *(inTos.bigIntPtr) < G_BI_MINUS_DBL_MAX ) { \
							inStack.emplace_back(false); \
						} else if(IS_LE(OP) /* #OP=="<=" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(IS_EQ(OP) /* #OP=="==" */ \
						  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
							  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(false); \
						} else if(IS_EQ(OP) /* #OP=="==" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else if(IS_NE(OP) /* #OP=="!=" */ \
						  && (*(inTos.bigIntPtr)<G_BI_MINUS_DBL_MAX \
							  || G_BI_DBL_MAX<*(inTos.bigIntPtr))) { \
							inStack.emplace_back(true); \
						} else if(IS_NE(OP) /* #OP=="!=" */ ) { \
							inStack.emplace_back( \
								inSecond.doubleValue \
								OP static_cast<double>(*(inTos.bigIntPtr))); \
						} else { \
							inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
						} \
						break; \
					case DataType::kTypeFloat: /* double x float -> bool */ \
						inStack.emplace_back( \
							inSecond.doubleValue OP inTos.floatValue); \
						break; \
					case DataType::kTypeBigFloat: /* double x bigFloat -> bool */ \
						inStack.emplace_back( \
							inSecond.doubleValue OP *(inTos.bigFloatPtr)); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::kTypeBigFloat: \
				switch(inTos.dataType) { \
					case DataType::kTypeInt: /* bigFloat x int -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.intValue); \
						break; \
					case DataType::kTypeLong: /* bigFloat x long -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.longValue); \
						break; \
					case DataType::kTypeBigInt: /* bigFloat x bigInt -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP \
							static_cast<BigFloat>(*(inTos.bigIntPtr))); \
						break; \
					case DataType::kTypeFloat: /* bigFloat x float -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigFloatPtr) OP inTos.floatValue); \
						break; \
					case DataType::kTypeDouble: /* bigFloat x double -> bool */ \
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
	return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
									   std::string(#OP),inSecond,inTos); \
onSuccess: ; \
} while(0)

#define RefCmpOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefCmpOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

// result=tv1 OP tv2
// ex: GetIntegerCmpOpResult_AndConvert(a,second,<,tos)
// 	   equivalent to: a=second<tos;
#define GetIntegerCmpOpResult_AndConvert(result,tv1,OP,tv2) do { \
	if(tv1.dataType==DataType::kTypeInt && tv2.dataType==DataType::kTypeInt) { \
		result=tv1.intValue OP tv2.intValue; \
	} else if(tv1.dataType==DataType::kTypeLong \
			  && tv2.dataType==DataType::kTypeLong) { \
		result=tv1.longValue OP tv2.longValue; \
	} else if(tv1.dataType==DataType::kTypeBigInt \
			  && tv2.dataType==DataType::kTypeBigInt) { \
		result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
	} else { \
		switch(tv1.dataType) { \
			case DataType::kTypeInt: \
				if(tv2.dataType==DataType::kTypeLong) { \
					tv1.dataType=DataType::kTypeLong; \
					tv1.longValue=(long)tv1.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(tv2.dataType==DataType::kTypeBigInt); \
					tv1.dataType=DataType::kTypeBigInt; \
					tv1.bigIntPtr=new BigInt(tv1.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case DataType::kTypeLong: \
				if(tv2.dataType==DataType::kTypeInt) { \
					tv2.dataType=DataType::kTypeLong; \
					tv2.longValue=(long)tv2.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(rsSecond.dataType==DataType::kTypeBigInt); \
					tv1.dataType=DataType::kTypeBigInt; \
					tv1.bigIntPtr=new BigInt(tv1.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case DataType::kTypeBigInt: \
				if(tv2.dataType==DataType::kTypeInt) { \
					tv2.dataType=DataType::kTypeBigInt; \
					tv2.bigIntPtr=new BigInt(tv2.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} else { \
					assert(tv2.dataType==DataType::kTypeLong); \
					tv2.dataType=DataType::kTypeBigInt; \
					tv2.bigIntPtr=new BigInt(tv2.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			default: \
				return inContext.Error(NoParamErrorID::E_SYSTEM_ERROR); \
		} \
	} \
} while(0)

// result=tv1 OP tv2
// ex: GetCmpOpResult(t,second,<,tos);
//     equivalent to t=second<tos;
#define GetCmpOpResult(result,tv1,OP,tv2) do { \
	if(tv1.dataType==tv2.dataType) { \
		switch(tv1.dataType) { \
			case DataType::kTypeInt: \
				result=tv1.intValue OP tv2.intValue; \
				break; \
			case DataType::kTypeLong: \
				result=tv1.longValue OP tv2.longValue; \
				break; \
			case DataType::kTypeBigInt: \
				result=*(tv1.bigIntPtr) OP *(tv2.bigIntPtr); \
				break; \
			case DataType::kTypeFloat: \
				result=tv1.floatValue OP tv2.floatValue; \
	  	 		break; \
			case DataType::kTypeDouble:	\
				result=tv1.doubleValue OP tv2.doubleValue; \
				break; \
			case DataType::kTypeBigFloat: \
				result=*(tv1.bigFloatPtr) OP *(tv2.bigFloatPtr); \
				break; \
			default: \
				result=false; \
		} \
	} else { \
		switch(tv1.dataType) { \
			case DataType::kTypeInt: \
				switch(tv2.dataType) { \
					case DataType::kTypeLong: \
						result=((long)tv1.intValue) OP tv2.longValue; \
						break; \
					case DataType::kTypeBigInt: \
						result=tv1.intValue OP *(tv2.bigIntPtr); \
						break; \
					case DataType::kTypeFloat: \
						result=((float)tv1.intValue) OP tv2.floatValue; \
						break; \
					case DataType::kTypeDouble: \
						result=((double)tv1.intValue) OP tv2.doubleValue; \
						break; \
					case DataType::kTypeBigFloat: \
						result=tv1.intValue OP *(tv2.bigFloatPtr); \
						break; \
					default: \
						result=false; \
				} \
				break; \
			case DataType::kTypeLong: \
				switch(tv2.dataType) { \
					case DataType::kTypeInt: \
						result=tv1.longValue OP ((long)tv2.intValue); \
						break; \
					case DataType::kTypeBigInt: \
						result=tv1.longValue OP *(tv2.bigFloatPtr); \
						break; \
					case DataType::kTypeFloat: \
						result=((float)(tv1.longValue)) OP tv2.floatValue; \
						break; \
					case DataType::kTypeDouble: \
						result=((double)tv1.longValue) OP tv2.doubleValue; \
						break; \
					case DataType::kTypeBigFloat: \
						result=tv1.longValue OP *(tv2.bigFloatPtr); \
						break; \
					default: \
						result=false; \
				} \
				break; \
			case DataType::kTypeBigInt: \
				switch(tv2.dataType) { \
					case DataType::kTypeInt: \
						result=*(tv1.bigIntPtr) OP tv2.intValue; break; \
					case DataType::kTypeLong: \
						result=*(tv1.bigIntPtr) OP tv2.longValue; break; \
					case DataType::kTypeFloat: \
						result=BigFloat(*tv1.bigIntPtr) OP tv2.floatValue; break; \
					case DataType::kTypeDouble: \
						result=BigFloat(*tv1.bigIntPtr) OP tv2.doubleValue; break; \
					case DataType::kTypeBigFloat: \
						result=BigFloat(*tv1.bigIntPtr) OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case DataType::kTypeFloat: \
				switch(tv2.dataType) { \
					case DataType::kTypeInt: \
						result=tv1.floatValue OP (float)tv2.intValue; break; \
					case DataType::kTypeLong: \
						result=tv1.floatValue OP (float)tv2.longValue; break; \
					case DataType::kTypeBigInt: \
						result=tv1.floatValue OP BigFloat(*(tv2.bigIntPtr)); break; \
					case DataType::kTypeDouble: \
						result=(double)tv1.floatValue OP tv2.doubleValue; break; \
					case DataType::kTypeBigFloat: \
						result=tv1.floatValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case DataType::kTypeDouble: \
				switch(tv2.dataType) { \
					case DataType::kTypeInt: \
						result=tv1.doubleValue OP (double)tv2.intValue; break; \
					case DataType::kTypeLong: \
						result=tv1.doubleValue OP (double)tv2.longValue; break; \
					case DataType::kTypeBigInt: \
						result=tv1.doubleValue OP BigFloat(*(tv2.bigIntPtr)); break; \
					case DataType::kTypeFloat: \
						result=tv1.doubleValue OP (double)tv2.floatValue; break; \
					case DataType::kTypeBigFloat: \
						result=tv1.doubleValue OP *(tv2.bigFloatPtr); break; \
					default: result=false; \
				} \
				break; \
			case DataType::kTypeBigFloat: \
				switch(tv2.dataType) { \
					case DataType::kTypeInt: \
						result=*(tv1.bigFloatPtr) OP tv2.intValue; break; \
					case DataType::kTypeLong: \
						result=*(tv1.bigFloatPtr) OP tv2.longValue; break; \
					case DataType::kTypeFloat: \
						result=*(tv1.bigFloatPtr) OP tv2.floatValue; break; \
					case DataType::kTypeDouble: \
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
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					second.dataType=DataType::kTypeLong; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=DataType::kTypeBigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeLong: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=DataType::kTypeBigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeBigInt: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: \
					*second.bigIntPtr=(*second.bigIntPtr) OP (*tos.bigIntPtr); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case DataType::kTypeInt: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* int x int -> int  */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* int x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeLong: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* long x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.longValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeBigInt: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* bigInt x bigInt -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					second.dataType=DataType::kTypeLong; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=DataType::kTypeBigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeLong: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=DataType::kTypeBigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		case DataType::kTypeBigInt: \
			switch(tos.dataType) { \
				case DataType::kTypeInt: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case DataType::kTypeLong: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case DataType::kTypeBigInt: \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						*second.bigIntPtr=(*second.bigIntPtr) \
											OP static_cast<long>(*tos.bigIntPtr); \
					} else { \
						inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); tos.Dump(); \
						printf("SECOND: "); second.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,second); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case DataType::kTypeInt: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* int x int -> int */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* int x bigInt -> bigInt */ \
					if(*inTos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.intValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		case DataType::kTypeLong: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* long x bigInt -> bigInt */ \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.longValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		case DataType::kTypeBigInt: \
			switch(inTos.dataType) { \
				case DataType::kTypeInt: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case DataType::kTypeLong: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case DataType::kTypeBigInt: /* bigInt x bigInt -> bigInt */ \
					if(*(inTos.bigIntPtr)<=LONG_MAX) { \
						inStack.emplace_back(*(inSecond.bigIntPtr) \
											OP static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::E_SORRY_NOT_SUPPORT_THIS_OPERATION); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inTos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,inSecond); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitShiftOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BoolOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	if(tos.dataType!=DataType::kTypeBool) { \
		return inContext.Error(InvalidTypeErrorID::E_TOS_BOOL,tos); \
	} \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(second.dataType!=DataType::kTypeBool) { \
		return inContext.Error(InvalidTypeErrorID::E_SECOND_BOOL,second); \
	} \
	second.boolValue=tos.boolValue OP second.boolValue; \
	NEXT; \
} while(0)

#define OneArgFloatingFunc(FUNC) do { \
	if(inContext.DS.size()<1) { \
		return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	if(tos.dataType==DataType::kTypeBigInt) { \
		BigFloat *bigFloat=new BigFloat(); \
		*bigFloat=FUNC(BigFloat(*tos.bigIntPtr)); \
		delete(tos.bigIntPtr); \
		tos.bigFloatPtr=bigFloat; \
		tos.dataType=DataType::kTypeBigFloat; \
		goto next; \
	} else if(tos.dataType==DataType::kTypeBigFloat) { \
		*tos.bigFloatPtr=FUNC(*tos.bigFloatPtr); \
		goto next; \
	} \
	double result; \
	switch(tos.dataType) { \
		case DataType::kTypeInt: 	result=FUNC((double)tos.intValue);		break; \
		case DataType::kTypeLong:	result=FUNC((double)tos.longValue);		break; \
		case DataType::kTypeFloat:	result=FUNC((double)tos.floatValue);	break; \
		case DataType::kTypeDouble:	result=FUNC(tos.doubleValue);			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_TOS_DOUBLABLE,tos); \
	} \
	tos.dataType=DataType::kTypeDouble; \
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
	if(inContext.DS.size()<1) { \
		return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY); \
	} \
	TypedValue& tos=ReadTOS(inContext.DS); \
	switch(tos.dataType) { \
		case DataType::kTypeInt: \
			tos.doubleValue=FUNC((double)tos.intValue); \
			tos.dataType=DataType::kTypeDouble; \
			break; \
		case DataType::kTypeLong: \
			tos.doubleValue=FUNC((double)tos.longValue); \
			tos.dataType=DataType::kTypeDouble; \
			break; \
		case DataType::kTypeFloat: \
			tos.doubleValue=FUNC((double)tos.floatValue); \
			tos.dataType=DataType::kTypeDouble; \
			break; \
		case DataType::kTypeDouble: \
			tos.doubleValue=FUNC(tos.doubleValue); \
			break; \
		case DataType::kTypeBigInt: { \
				BigFloat *bigFloat=new BigFloat(*tos.bigIntPtr); \
				*bigFloat=FUNC(*bigFloat); \
				delete tos.bigIntPtr; \
				tos.bigFloatPtr=bigFloat; \
				tos.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigFloat: \
			*tos.bigFloatPtr=FUNC(*tos.bigFloatPtr); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos); \
	} \
	NEXT; \
} while(0)

// c=a%b
// ModOp(c,a,b)
#define ModOp(outDest,inSrc1,inSrc2) do { \
	if(inSrc1.dataType==DataType::kTypeInt && inSrc2.dataType==DataType::kTypeInt) { \
		outDest=TypedValue(inSrc1.intValue % inSrc2.intValue); \
	} else if(inSrc1.dataType==DataType::kTypeLong \
			  && inSrc2.dataType==DataType::kTypeLong) { \
		outDest=TypedValue(inSrc1.longValue % inSrc2.longValue); \
	} else if(inSrc1.dataType==DataType::kTypeBigInt \
			  && inSrc2.dataType==DataType::kTypeBigInt) { \
		outDest=TypedValue(*(inSrc1.bigIntPtr) % *(inSrc2.bigIntPtr)); \
	} else { \
		if(inSrc1.dataType==DataType::kTypeInt) { \
			switch(inSrc2.dataType) { \
				case DataType::kTypeLong: \
					outDest=TypedValue((long)inSrc1.intValue % inSrc2.longValue); \
					break; \
				case DataType::kTypeBigInt: \
					outDest=TypedValue(BigInt(inSrc1.intValue) % *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		  } else if(inSrc1.dataType==DataType::kTypeLong) { \
			switch(inSrc2.dataType) { \
				case DataType::kTypeInt: \
					outDest=TypedValue(inSrc1.longValue % (long)inSrc2.intValue); \
					break; \
				case DataType::kTypeBigInt: \
					outDest=TypedValue(BigInt(inSrc1.longValue) \
								  		% *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		} else if(inSrc1.dataType==DataType::kTypeBigInt) { \
			switch(inSrc2.dataType) { \
				case DataType::kTypeInt: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.intValue); \
					break; \
				case DataType::kTypeLong: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.longValue); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,inSrc2); \
			} \
		} else { \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,inSrc1); \
		} \
	} \
} while(0)

// a %= b
// ModAssign(a,b);
#define ModAssign(inTarget,inTV) do { \
	const int combinationType=GetMathOpType(inTarget.dataType,inTV.dataType); \
	switch((TypeCombinationForMathOP)combinationType) {\
		case TypeCombinationForMathOP::kIntInt:  \
			inTarget.intValue %= inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::kIntLong: \
			inTarget.longValue=(long)inTarget.intValue % inTV.longValue; \
			inTarget.dataType=DataType::kTypeLong; \
			break; \
		case TypeCombinationForMathOP::kIntBigInt: \
			inTarget.bigIntPtr=new BigInt(inTarget.intValue % *inTV.bigIntPtr); \
			inTarget.dataType=DataType::kTypeBigInt; \
			break; \
		case TypeCombinationForMathOP::kLongInt: \
			inTarget.longValue %= (long)inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::kLongLong: \
			inTarget.longValue %= inTV.longValue; \
			break; \
		case TypeCombinationForMathOP::kLongBigInt: \
			inTarget.bigIntPtr=new BigInt(inTarget.longValue % *inTV.bigIntPtr); \
			inTarget.dataType=DataType::kTypeBigInt; \
		case TypeCombinationForMathOP::kBigIntInt: \
			*inTarget.bigIntPtr %= inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::kBigIntLong: \
			*inTarget.bigIntPtr %= inTV.longValue; \
			break; \
		case TypeCombinationForMathOP::kBigIntBigInt: \
			*inTarget.bigIntPtr %= (*inTV.bigIntPtr); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,inTarget); \
	} \
} while(0)

// Push(a % b)
// RefMod(inContext.DS,a,b);
#define RefMod(inStack,inSecond,inTos) do { \
	const int combinationType=GetMathOpType(inSecond.dataType,inTos.dataType); \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::kIntInt:  \
			inStack.emplace_back(inSecond.intValue % inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::kIntLong: \
			inStack.emplace_back((long)inSecond.intValue % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::kIntBigInt: \
			inStack.emplace_back(inSecond.intValue % *(inTos.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::kLongInt: \
			inStack.emplace_back(inSecond.longValue % (long)inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::kLongLong: \
			inStack.emplace_back(inSecond.longValue % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::kLongBigInt: \
			inStack.emplace_back(inSecond.longValue % *(inTos.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::kBigIntInt: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::kBigIntLong: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::kBigIntBigInt: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % *(inTos.bigIntPtr)); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,inSecond); \
	} \
} while(0)

// ModAssignTOS
// tos = second % tos
#define DIVIDE_BY_ZERO_ModAssignTOS MakeLabel(divideByZero,__LINE__)
#define ModAssignTOS(inContext,inSecond,inTOS) { \
	const int combinationType=GetMathOpType(inSecond.dataType,inTOS.dataType); \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::kIntInt:  \
			inTOS.intValue=inSecond.intValue % inTOS.intValue; \
			break; \
		case TypeCombinationForMathOP::kIntLong: \
			inTOS.longValue=(long)inSecond.intValue % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::kIntBigInt: \
			*(inTOS.bigIntPtr)=inSecond.intValue % (*inTOS.bigIntPtr); \
			break; \
		case TypeCombinationForMathOP::kLongInt: \
			inTOS.longValue=inSecond.longValue % (long)inTOS.intValue; \
			inTOS.dataType=DataType::kTypeLong; \
			break; \
		case TypeCombinationForMathOP::kLongLong: \
			inTOS.longValue=inSecond.longValue % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::kLongBigInt: \
			*inTOS.bigIntPtr=(*inSecond.bigIntPtr) % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::kBigIntInt: \
			inTOS.bigIntPtr=new BigInt((*inSecond.bigIntPtr) % inTOS.intValue); \
			inTOS.dataType=DataType::kTypeBigInt; \
			break; \
		case TypeCombinationForMathOP::kBigIntLong: \
			inTOS.bigIntPtr=new BigInt((*inSecond.bigIntPtr) % inTOS.longValue); \
			inTOS.dataType=DataType::kTypeBigInt; \
			break; \
		case TypeCombinationForMathOP::kBigIntBigInt: \
			*inTOS.bigIntPtr=(*inSecond.bigIntPtr) % (*inTOS.bigIntPtr); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_SECOND_INT_OR_LONG_OR_BIGINT,inSecond); \
	} \
} while(0)

#define FloorOrCeil(FUNC,ioTV) do { \
	switch(ioTV.dataType) { \
		case DataType::kTypeInt: \
		case DataType::kTypeLong: \
		case DataType::kTypeBigInt: \
			break; \
		case DataType::kTypeFloat: { \
				float f=FUNC(ioTV.floatValue); \
				if(INT_MIN<=f && f<=INT_MAX) { \
					ioTV.intValue=(int)f; \
					ioTV.dataType=DataType::kTypeInt; \
				} else if(LONG_MIN<=f && f<=LONG_MAX) { \
					ioTV.longValue=(long)f; \
					ioTV.dataType=DataType::kTypeLong; \
				} else { \
					ioTV.bigIntPtr=new BigInt(f); \
					ioTV.dataType=DataType::kTypeBigInt; \
				} \
			} \
			break; \
		case DataType::kTypeDouble: { \
				double t=FUNC(ioTV.doubleValue); \
				if(INT_MIN<=t && t<=INT_MAX) { \
					ioTV.intValue=(int)t; \
					ioTV.dataType=DataType::kTypeInt; \
				} else if(LONG_MIN<=t && t<=LONG_MAX) { \
					ioTV.longValue=(long)t; \
					ioTV.dataType=DataType::kTypeLong; \
				} else { \
					ioTV.bigIntPtr=new BigInt(t); \
					ioTV.dataType=DataType::kTypeBigInt; \
				} \
			} \
			break; \
		case DataType::kTypeBigFloat: \
			ioTV.bigIntPtr=new BigInt(FUNC(*ioTV.bigFloatPtr)); \
			ioTV.dataType=DataType::kTypeBigInt; \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,ioTV); \
	} \
} while(0)

// X int OP
#define IntTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const int n=tos.intValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			second.intValue=second.intValue OP n; \
			break; \
		case DataType::kTypeLong: \
			second.longValue=second.longValue OP n; \
			break; \
		case DataType::kTypeFloat: \
			second.floatValue=second.floatValue OP n; \
			break; \
		case DataType::kTypeDouble: \
			second.doubleValue=second.doubleValue OP n; \
			break; \
		case DataType::kTypeBigInt: \
			*(second.bigIntPtr)=*(second.bigIntPtr) OP n; \
			break; \
		case DataType::kTypeBigFloat: \
			*(second.bigFloatPtr)=*(second.bigFloatPtr) OP n; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X long OP
#define LongTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const long n=tos.longValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			second.dataType=DataType::kTypeLong; \
			second.longValue=second.intValue OP n; \
  			break; \
		case DataType::kTypeLong: \
			second.longValue=second.longValue OP n; \
			break; \
		case DataType::kTypeFloat: \
			second.floatValue=second.floatValue OP n; \
			break; \
		case DataType::kTypeDouble: \
			second.doubleValue=second.doubleValue OP n; \
			break; \
		case DataType::kTypeBigInt:	\
			*(second.bigIntPtr)=*(second.bigIntPtr) OP n; \
			break; \
		case DataType::kTypeBigFloat: \
			*(second.bigFloatPtr)=*(second.bigFloatPtr) OP n; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X float OP
#define FloatTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const float t=tos.floatValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			second.dataType=DataType::kTypeFloat; \
			second.floatValue=second.intValue OP t; \
  			break; \
		case DataType::kTypeLong: \
			second.dataType=DataType::kTypeFloat; \
			second.floatValue=second.longValue OP t; \
			break; \
		case DataType::kTypeFloat: \
			second.floatValue=second.floatValue OP t; \
			break; \
		case DataType::kTypeDouble:	\
			second.doubleValue=second.doubleValue OP t; \
			break; \
		case DataType::kTypeBigInt: { \
				BigFloat *bigFloat=new BigFloat(*second.bigIntPtr); \
				*bigFloat=*bigFloat OP t; \
				delete(second.bigIntPtr); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigFloat: \
			*(second.bigFloatPtr)=*(second.bigFloatPtr) OP t; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X double OP
#define DoubleTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const double t=tos.doubleValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: \
			second.dataType=DataType::kTypeDouble; \
			second.doubleValue=second.intValue OP t; \
  			break; \
		case DataType::kTypeLong: \
			second.dataType=DataType::kTypeDouble; \
			second.doubleValue=second.longValue OP t; \
			break; \
		case DataType::kTypeFloat: \
			second.dataType=DataType::kTypeDouble; \
			second.doubleValue=second.floatValue OP t; \
			break; \
		case DataType::kTypeDouble:	\
			second.doubleValue=second.doubleValue OP t; \
			break; \
		case DataType::kTypeBigInt: { \
				BigFloat *bigFloat=new BigFloat(*second.bigIntPtr); \
				*bigFloat=*bigFloat OP t; \
				delete(second.bigIntPtr); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigFloat: \
			*(second.bigFloatPtr)=*(second.bigFloatPtr) OP t; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X bigInt OP
#define BigIntTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const BigInt *t=tos.bigIntPtr; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: { \
				BigInt *bigInt=new BigInt(second.intValue); \
				*bigInt=*bigInt OP (*t); \
				second.bigIntPtr=bigInt; \
				second.dataType=DataType::kTypeBigInt; \
			} \
  			break; \
		case DataType::kTypeLong: { \
				BigInt *bigInt=new BigInt(second.longValue); \
				*bigInt=*bigInt OP (*t); \
				second.bigIntPtr=bigInt; \
				second.dataType=DataType::kTypeBigInt; \
			} \
			break; \
		case DataType::kTypeFloat: { \
				BigFloat tmpBigFloat=BigFloat(*tos.bigIntPtr); \
				BigFloat *bigFloat=new BigFloat(second.floatValue); \
				*bigFloat=*bigFloat OP tmpBigFloat; \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeDouble: { \
				BigFloat tmpBigFloat=BigFloat(*tos.bigIntPtr); \
				BigFloat *bigFloat=new BigFloat(second.doubleValue); \
				*bigFloat=*bigFloat OP tmpBigFloat; \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigInt: \
			*(second.bigIntPtr)=*(second.bigIntPtr) OP (*t); \
			break; \
		case DataType::kTypeBigFloat: { \
				BigFloat tmpBigFloat=BigFloat(*tos.bigIntPtr); \
				*(second.bigFloatPtr)=*(second.bigFloatPtr) OP tmpBigFloat; \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X bigFloat OP
#define BigFloatTwoOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	const BigFloat *t=tos.bigFloatPtr; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::kTypeInt: { \
				BigFloat *bigFloat=new BigFloat(second.intValue); \
				*bigFloat=*bigFloat OP (*t); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
  			break; \
		case DataType::kTypeLong: { \
				BigFloat *bigFloat=new BigFloat(second.longValue); \
				*bigFloat=*bigFloat OP (*t); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeFloat: { \
				BigFloat *bigFloat=new BigFloat(second.floatValue); \
				*bigFloat=*bigFloat OP (*t); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeDouble: { \
				BigFloat *bigFloat=new BigFloat(second.doubleValue); \
				*bigFloat=*bigFloat OP (*t); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigInt: { \
				BigFloat *bigFloat=new BigFloat(*second.bigIntPtr); \
				*bigFloat=*bigFloat OP (*t); \
				second.bigFloatPtr=bigFloat; \
				second.dataType=DataType::kTypeBigFloat; \
			} \
			break; \
		case DataType::kTypeBigFloat: { \
				*(second.bigFloatPtr)=*(second.bigFloatPtr) OP (*t); \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::E_INVALID_DATA_TYPE_FOR_OP, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

