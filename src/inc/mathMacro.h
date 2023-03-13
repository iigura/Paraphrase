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

#define SetResultFromBF100(outDest,inX) do { \
	if(-FLT_MAX<=inX && inX<=FLT_MAX) { \
		outDest=TypedValue((float)inX); \
	} else if(-DBL_MAX<=inX && inX<=DBL_MAX) { \
		outDest=TypedValue((double)inX); \
	} else { \
		outDest=TypedValue((BigInt)inX); \
	} \
} while(0)

// c=a OP b
// MathOp(c,a,OP,b);
#define MathOp(outDest,inSrc1,OP,inSrc2) do { \
	int combinationType=GetMathOpType(inSrc1.dataType,inSrc2.dataType); \
	SKIP( if(combinationType>35) { \
		fprintf(stderr,"SYSTEM ERROR\n"); \
		exit(-1); \
	} ) \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::IntInt: \
			outDest.Set(inSrc1.intValue OP inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::IntLong: \
			outDest=TypedValue((long)inSrc1.intValue OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::IntBigInt: \
			outDest=TypedValue(inSrc1.intValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::IntFloat: \
			outDest.Set((float)inSrc1.intValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::IntDouble: \
			outDest.Set((double)inSrc1.intValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::LongInt: \
			outDest=TypedValue(inSrc1.longValue OP (long)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::LongLong: \
			outDest=TypedValue(inSrc1.longValue OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::LongBigInt: \
			outDest=TypedValue(inSrc1.longValue OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::LongFloat: \
			outDest.Set((float)inSrc1.longValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::LongDouble: \
			outDest.Set((double)inSrc1.longValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::BigIntInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::BigIntLong: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::BigIntBigInt: \
			outDest=TypedValue(*(inSrc1.bigIntPtr) OP *(inSrc2.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::BigIntFloat: \
			{ \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=BF100(*(inSrc1.bigIntPtr)) OP inSrc2.floatValue; \
				SetResultFromBF100(outDest,tmp); \
			} \
			break; \
		case TypeCombinationForMathOP::FloatInt: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::FloatLong: \
			outDest.Set(inSrc1.floatValue OP (float)inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::FloatBigInt: \
			{ \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=inSrc1.floatValue OP BF100(*(inSrc2.bigIntPtr)); \
				SetResultFromBF100(outDest,tmp); \
			}\
			break; \
		case TypeCombinationForMathOP::FloatFloat: \
			outDest.Set(inSrc1.floatValue OP inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::FloatDouble: \
			outDest.Set((double)inSrc1.floatValue OP inSrc2.doubleValue); \
			break; \
		case TypeCombinationForMathOP::DoubleInt: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.intValue); \
			break; \
		case TypeCombinationForMathOP::DoubleLong: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.longValue); \
			break; \
		case TypeCombinationForMathOP::DoubleBigInt: \
			{ \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=inSrc1.doubleValue OP BF100(*(inSrc2.bigIntPtr)); \
				SetResultFromBF100(outDest,tmp); \
			} \
			break; \
		case TypeCombinationForMathOP::DoubleFloat: \
			outDest.Set(inSrc1.doubleValue OP (double)inSrc2.floatValue); \
			break; \
		case TypeCombinationForMathOP::DoubleDouble: \
			outDest.Set(inSrc1.doubleValue OP inSrc2.doubleValue); \
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
	  && inTarget.dataType==DataType::String \
	  && inTV.dataType    ==DataType::String) { \
		inTarget=TypedValue(*inTarget.stringPtr+*inTV.stringPtr); \
	} else { \
		if( IS_DIV(OP) ) { \
			switch(inTV.dataType) { \
				case DataType::Int: \
					if(inTV.intValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
					break; \
				case DataType::Long: \
					if(inTV.longValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
					break; \
				case DataType::BigInt: \
					if(*(inTV.bigIntPtr)==0) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				case DataType::Float: \
					if(inTV.floatValue==0.0f) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				case DataType::Double: \
					if(inTV.doubleValue==0.0) { \
						goto DIVIDE_BY_ZERO_AssignOpToSecond; \
					} \
					break; \
				default: \
				 	return inContext.Error(InvalidTypeErrorID::TosNumber,inTV); \
DIVIDE_BY_ZERO_AssignOpToSecond: \
					return inContext.Error(NoParamErrorID::TosShouldBeNonZero); \
			} \
		} \
		const int combinationType=GetMathOpType(inTarget.dataType,inTV.dataType); \
		if(combinationType>=(int)TypeCombinationForMathOP::kInvalidMathOpTypeThreshold) { \
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,inTV,inTarget); \
		} \
		switch((TypeCombinationForMathOP)combinationType) { \
			case TypeCombinationForMathOP::IntInt:  \
				inTarget.intValue=inTarget.intValue OP inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::IntLong: \
				inTarget.longValue=(long)inTarget.intValue OP inTV.longValue; \
				inTarget.dataType=DataType::Long; \
				break; \
			case TypeCombinationForMathOP::IntBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.intValue OP *inTV.bigIntPtr); \
				inTarget.dataType=DataType::BigInt; \
				break; \
			case TypeCombinationForMathOP::IntFloat: \
				inTarget.floatValue=(float)inTarget.intValue OP inTV.floatValue; \
				inTarget.dataType=DataType::Float; \
				break; \
			case TypeCombinationForMathOP::IntDouble: \
				inTarget.doubleValue=(double)inTarget.intValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::LongInt: \
				inTarget.longValue=inTarget.longValue OP (long)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::LongLong: \
				inTarget.longValue=inTarget.longValue OP inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::LongBigInt: \
				inTarget.bigIntPtr=new BigInt(inTarget.longValue OP *inTV.bigIntPtr); \
				inTarget.dataType=DataType::BigInt; \
				break; \
			case TypeCombinationForMathOP::LongFloat: \
				inTarget.floatValue=(float)inTarget.longValue OP inTV.floatValue; \
				inTarget.dataType=DataType::Float; \
				break; \
			case TypeCombinationForMathOP::LongDouble: \
				inTarget.doubleValue=(double)inTarget.longValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::BigIntInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::BigIntLong: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::BigIntBigInt: \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP *inTV.bigIntPtr; \
				break; \
			case TypeCombinationForMathOP::BigIntFloat: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*(inTarget.bigIntPtr)) OP inTV.floatValue; \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::BigIntDouble: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*(inTarget.bigIntPtr)) OP inTV.doubleValue; \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::FloatInt: \
				inTarget.floatValue=inTarget.floatValue OP (float)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::FloatLong: \
				inTarget.floatValue=inTarget.floatValue OP (float)inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::FloatBigInt: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inTarget.floatValue OP BF100(*(inTV.bigIntPtr)); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::FloatFloat: \
				inTarget.floatValue=inTarget.floatValue OP inTV.floatValue; \
				break; \
			case TypeCombinationForMathOP::FloatDouble: \
				inTarget.doubleValue=(double)inTarget.floatValue OP inTV.doubleValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::DoubleInt: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.intValue; \
				break; \
			case TypeCombinationForMathOP::DoubleLong: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.longValue; \
				break; \
			case TypeCombinationForMathOP::DoubleBigInt: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inTarget.doubleValue OP BF100(*(inTV.bigIntPtr)); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::DoubleFloat: \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inTV.floatValue; \
				break; \
			case TypeCombinationForMathOP::DoubleDouble: \
				inTarget.doubleValue=inTarget.doubleValue OP inTV.doubleValue; \
				break; \
			default: \
				return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
												   std::string(#OP),inTarget,inTV); \
		} \
	} \
} while(0)

// a b --- a c(=a+b)
// ex: b = a + b
// 	   AssignOpToTOS(a,+,b)
#define AssignOpToTOS(inContext,inTV,OP,inTarget) do { \
	if(IS_PLUS(IO) /* #OP=="+" */ \
	   && inTV.dataType==DataType::String \
	   && inTarget.dataType==DataType::String) { \
		inTarget=TypedValue(*inTV.stringPtr+*inTarget.stringPtr); \
	} else { \
		const int combinationType=GetMathOpType(inTV.dataType,inTarget.dataType); \
		if(combinationType>=(int)TypeCombinationForMathOP::kInvalidMathOpTypeThreshold) { \
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,inTarget,inTV); \
		} \
		switch((TypeCombinationForMathOP)combinationType) { \
			case TypeCombinationForMathOP::IntInt:  \
				inTarget.intValue=inTV.intValue OP inTarget.intValue; \
				break; \
			case TypeCombinationForMathOP::IntLong: \
				inTarget.longValue=(long)inTV.intValue OP inTarget.longValue; \
				break; \
			case TypeCombinationForMathOP::IntBigInt: \
				*inTarget.bigIntPtr=inTV.intValue OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::IntFloat: \
				inTarget.floatValue=inTV.intValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::IntDouble: \
				inTarget.doubleValue=inTV.intValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::LongInt: \
				inTarget.longValue=inTV.longValue OP inTarget.intValue; \
				inTarget.dataType=DataType::Long; \
				break; \
			case TypeCombinationForMathOP::LongLong: \
				inTarget.longValue=inTV.longValue OP inTarget.longValue; \
				break; \
			case TypeCombinationForMathOP::LongBigInt: \
				*inTarget.bigIntPtr=inTV.longValue OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::LongFloat: \
				inTarget.floatValue=(float)inTV.longValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::LongDouble: \
				inTarget.doubleValue=(double)inTV.longValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::BigIntInt: \
				inTarget.bigIntPtr=new BigInt(*inTV.bigIntPtr OP inTarget.intValue); \
				inTarget.dataType=DataType::BigInt; \
				break; \
			case TypeCombinationForMathOP::BigIntLong: \
				inTarget.bigIntPtr=new BigInt(*inTV.bigIntPtr OP inTarget.longValue); \
				inTarget.dataType=DataType::BigInt; \
				break; \
			case TypeCombinationForMathOP::BigIntBigInt: \
				*inTarget.bigIntPtr=(*inTV.bigIntPtr) OP (*inTarget.bigIntPtr); \
				break; \
			case TypeCombinationForMathOP::BigIntFloat: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*inTV.bigIntPtr) OP BF100(inTarget.floatValue); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::BigIntDouble: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*inTV.bigIntPtr) OP BF100(inTarget.doubleValue); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::FloatInt: \
				inTarget.floatValue=inTV.floatValue OP (float)inTarget.intValue; \
				inTarget.dataType=DataType::Float; \
				break; \
			case TypeCombinationForMathOP::FloatLong: \
				inTarget.floatValue=inTV.floatValue OP (float)inTarget.longValue; \
				inTarget.dataType=DataType::Float; \
				break; \
			case TypeCombinationForMathOP::FloatBigInt: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inTV.floatValue OP BF100(*inTarget.bigIntPtr); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::FloatFloat: \
				inTarget.floatValue=inTV.floatValue OP inTarget.floatValue; \
				break; \
			case TypeCombinationForMathOP::FloatDouble: \
				inTarget.doubleValue=inTV.floatValue OP inTarget.doubleValue; \
				break; \
			case TypeCombinationForMathOP::DoubleInt: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.intValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::DoubleLong: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.longValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::DoubleBigInt: { \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inTV.doubleValue OP BF100(*inTarget.bigIntPtr); \
					SetResultFromBF100(inTarget,tmp); \
				} \
				break; \
			case TypeCombinationForMathOP::DoubleFloat: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.floatValue; \
				inTarget.dataType=DataType::Double; \
				break; \
			case TypeCombinationForMathOP::DoubleDouble: \
				inTarget.doubleValue=inTV.doubleValue OP inTarget.doubleValue; \
				break; \
			default: \
				return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
												   std::string(#OP),inTV,inTarget); \
		} \
	} \
} while(0)

#define TwoOpAssignTOS(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	AssignOpToTOS(inContext,second,OP,tos); \
	NEXT; \
} while(0)

// Push(a + b)
// PushOp(inContext.DS,a,+,b)
#define PushOp(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==DataType::Int \
	   && inTos.dataType==DataType::Int) { \
		/* int x int -> int */ \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==DataType::Long \
			  && inTos.dataType==DataType::Long) { \
		/* long x long -> long */ \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==DataType::Float \
			  && inTos.dataType==DataType::Float) { \
		/* float x float -> float */ \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==DataType::Double \
			  && inTos.dataType==DataType::Double) { \
		/* double x double -> double */ \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==DataType::BigInt \
			  && inTos.dataType==DataType::BigInt) { \
		/* bigInt x bigInt -> bigInt */ \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else if(inSecond.dataType==DataType::Int) { \
		switch(inTos.dataType) { \
			case DataType::Long:		/* int x long -> long */ \
				inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
				break; \
			case DataType::Float:	/* int x float -> float */ \
				inStack.emplace_back((float)inSecond.intValue OP inTos.floatValue); \
				break; \
			case DataType::Double:	/* int x double -> double */ \
				inStack.emplace_back((double)inSecond.intValue OP inTos.doubleValue); \
				break; \
			case DataType::BigInt: 	/* int x BigInt -> BigInt */ \
				inStack.emplace_back(BigInt(inSecond.intValue OP *inTos.bigIntPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::Long) { \
		switch(inTos.dataType) { \
			case DataType::Int:		/* long x int -> long */ \
				inStack.emplace_back(inSecond.longValue OP (long)inTos.intValue); \
				break; \
			case DataType::Float:	/* long x float -> float */ \
				inStack.emplace_back((float)inSecond.longValue OP inTos.floatValue); \
				break; \
			case DataType::Double:	/* long x double -> double */ \
				inStack.emplace_back((double)inSecond.longValue OP inTos.doubleValue); \
				break; \
			case DataType::BigInt: 	/* long x bigInt -> bigInt */ \
				inStack.emplace_back(BigInt(inSecond.longValue OP *inTos.bigIntPtr)); \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::Float) { \
		switch(inTos.dataType) { \
			case DataType::Int:		/* float x int -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.intValue); \
				break; \
			case DataType::Long:		/* float x long -> float */ \
				inStack.emplace_back(inSecond.floatValue OP (float)inTos.longValue); \
				break; \
			case DataType::Double:	/* float x double -> double */ \
				inStack.emplace_back( \
					(double)inSecond.floatValue OP inTos.doubleValue); \
				break; \
			case DataType::BigInt: 	/* float x bigInt -> bigFloat */ \
				{ \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inSecond.floatValue OP BF100(*inTos.bigIntPtr); \
					TypedValue tv; \
					SetResultFromBF100(tv,tmp); \
					inStack.emplace_back(tv); \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::Double) { \
		switch(inTos.dataType) { \
			case DataType::Int:		/* double x int -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.intValue); \
				break; \
			case DataType::Long:		/* double x long -> double */ \
				inStack.emplace_back(inSecond.doubleValue OP (double)inTos.longValue); \
				break; \
			case DataType::Float:	/* double x float -> double */ \
				inStack.emplace_back( \
					inSecond.doubleValue OP (double)inTos.floatValue); \
				break; \
			case DataType::BigInt: 	/* double x bigInt -> bigFloat */ \
				{ \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=inSecond.doubleValue OP BF100(*inTos.bigIntPtr); \
					TypedValue tv; \
					SetResultFromBF100(tv,tmp); \
					inStack.emplace_back(tv); \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if(inSecond.dataType==DataType::BigInt) { \
		switch(inTos.dataType) { \
			case DataType::Int:		/* bigInt x int -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
				break; \
			case DataType::Long: 	/* bigInt x long -> bigInt */ \
				inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
				break; \
			case DataType::Float:	/* bigInt x float -> bigFloat */ \
				{ \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*inSecond.bigIntPtr) OP BF100(inTos.floatValue); \
					TypedValue tv; \
					SetResultFromBF100(tv,tmp); \
					inStack.emplace_back(tv); \
				} \
				break; \
			case DataType::Double:	/* bigInt x double -> bigFloat */ \
				{ \
					typedef boost::multiprecision::cpp_dec_float_100 BF100; \
					BF100 tmp=BF100(*inSecond.bigIntPtr) OP BF100(inTos.doubleValue); \
					TypedValue tv; \
					SetResultFromBF100(tv,tmp); \
					inStack.emplace_back(tv); \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if((#OP[0]=='+' && #OP[1]=='\0') \
			 && inSecond.dataType==DataType::String \
			 && inTos.dataType==DataType::String) { \
		inStack.emplace_back(TypedValue(*(inSecond.stringPtr)+*(inTos.stringPtr))); \
	} else { \
onError: \
		return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
										   std::string(#OP),inSecond,inTos); \
	} \
} while(0)

#define PushFuncResult(inStack,OP,inSecond,inTos) do { \
	if(inSecond.dataType==inTos.dataType) { \
		/* same data type */ \
		switch(inSecond.dataType) { \
			case DataType::Int: \
				inStack.emplace_back(OP(inSecond.intValue,inTos.intValue)); \
				break; \
			case DataType::Long: \
				inStack.emplace_back(OP(inSecond.longValue,inTos.longValue)); \
				break; \
			case DataType::Float: \
				inStack.emplace_back(OP(inSecond.floatValue,inTos.floatValue)); \
				break; \
			case DataType::Double: \
				inStack.emplace_back(OP(inSecond.doubleValue,inTos.doubleValue)); \
				break; \
			case DataType::BigInt: \
				inStack.emplace_back(OP(*(inSecond.bigIntPtr),*(inTos.bigIntPtr))); \
				break; \
			default: \
				inContext.Error(NoParamErrorID::SystemError); \
		}	\
	} else { \
		/* different data type */ \
		if(inSecond.dataType==DataType::Int) { \
			switch(inTos.dataType) { \
				case DataType::Long: /* (int,long) */ \
					inStack.emplace_back(OP((long)inSecond.intValue, \
											inTos.longValue)); \
					break; \
				case DataType::Float: /* (int,float) */ \
					inStack.emplace_back(OP((float)inSecond.intValue, \
											inTos.floatValue)); \
					break; \
				case DataType::Double: /* (int,double) */ \
					inStack.emplace_back(OP((double)inSecond.intValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::BigInt: /* (int,big-ing) */ \
					inStack.emplace_back(OP(BigInt(inSecond.intValue), \
											*(inTos.bigIntPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::SystemError); \
			} \
		} else if(inSecond.dataType==DataType::Long) { \
			switch(inTos.dataType) { \
				case DataType::Int: /* (long,int) */ \
					inStack.emplace_back(OP(inSecond.longValue, \
											(long)inTos.intValue)); \
					break; \
				case DataType::Float: /* (long,float) */ \
					inStack.emplace_back(OP((float)inSecond.longValue, \
											inTos.floatValue)); \
					break; \
				case DataType::Double: /* (long,double) */ \
					inStack.emplace_back(OP((double)inSecond.longValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::BigInt: /* (long,big-int) */ \
					inStack.emplace_back(OP(BigInt(inSecond.longValue), \
											*(inTos.bigIntPtr))); \
					break; \
				default: \
					inContext.Error(NoParamErrorID::SystemError); \
			} \
		} else if(inSecond.dataType==DataType::Float) { \
			switch(inTos.dataType) { \
				case DataType::Int: /* (float,int) */ \
					inStack.emplace_back(OP(inSecond.floatValue, \
											(float)inTos.intValue)); \
					break; \
				case DataType::Long: /* (float,long) */ \
					inStack.emplace_back(OP(inSecond.floatValue, \
											(float)inTos.longValue)); \
					break; \
				case DataType::Double: /* (float,double) */ \
					inStack.emplace_back(OP((double)inSecond.floatValue, \
											inTos.doubleValue)); \
					break; \
				case DataType::BigInt: /* (float,big-int) */ {\
						typedef boost::multiprecision::cpp_dec_float_100 BF100; \
						BF100 tmp=OP(BF100(inSecond.floatValue), \
									 BF100(*inTos.bigIntPtr)); \
						TypedValue tv; \
						SetResultFromBF100(tv,tmp); \
						inStack.emplace_back(tv); \
					} \
					break; \
				default: \
					inContext.Error(NoParamErrorID::SystemError); \
			} \
		} else if(inSecond.dataType==DataType::Double) { \
			switch(inTos.dataType) { \
				case DataType::Int: /* (double,int) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.intValue)); \
					break; \
				case DataType::Long: /* (double,long) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.longValue)); \
					break; \
				case DataType::Float: /* (double,float) */ \
					inStack.emplace_back(OP(inSecond.doubleValue, \
											(double)inTos.floatValue)); \
					break; \
				case DataType::BigInt: /* (double,big-int) */ { \
						typedef boost::multiprecision::cpp_dec_float_100 BF100; \
						BF100 tmp=OP(BF100(inSecond.doubleValue), \
									 BF100(*inTos.bigIntPtr)); \
						TypedValue tv; \
						SetResultFromBF100(tv,tmp); \
						inStack.emplace_back(tv); \
					} \
					break; \
				default: \
					inContext.Error(NoParamErrorID::SystemError); \
			} \
		} else if(inSecond.dataType==DataType::BigInt) { \
			switch(inTos.dataType) { \
				case DataType::Int: /* (big-int,int) */ \
					inStack.emplace_back(OP(*(inSecond.bigIntPtr), \
											BigInt(inTos.intValue))); \
					break; \
				case DataType::Long: /* (big-int,long) */ \
					inStack.emplace_back(OP(*(inSecond.bigIntPtr), \
											BigInt(inTos.longValue))); \
					break; \
				case DataType::Float: { /* (big-int,float) */ \
						typedef boost::multiprecision::cpp_dec_float_100 BF100; \
						BF100 tmp=OP(BF100(*inSecond.bigIntPtr), \
									 BF100(inTos.floatValue)); \
						TypedValue tv; \
						SetResultFromBF100(tv,tmp); \
						inStack.emplace_back(tv); \
					} \
					break; \
				case DataType::Double: { /* (big-int,double) */ \
						typedef boost::multiprecision::cpp_dec_float_100 BF100; \
						BF100 tmp=OP(BF100(*inSecond.bigIntPtr), \
									 BF100(inTos.doubleValue)); \
						TypedValue tv; \
						SetResultFromBF100(tv,tmp); \
						inStack.emplace_back(tv); \
					} \
					break; \
				default: \
					inContext.Error(NoParamErrorID::SystemError); \
			} \
		} else { \
			inContext.Error(NoParamErrorID::SystemError); \
		}\
	} \
} while(0)

#define DIVIDE_BY_ZERO_CheckIntegerAndNonZero MakeLabel(divideByZero,__LINE__)
#define CheckIntegerAndNonZero(inTV) do { \
	switch(inTV.dataType) { \
		case DataType::Int: \
			if(inTV.intValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		case DataType::Long: \
			if(inTV.longValue==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		case DataType::BigInt: \
			if(*(inTV.bigIntPtr)==0) { goto DIVIDE_BY_ZERO_AssignOpToSecond; } \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inTV); \
DIVIDE_BY_ZERO_CheckIntegerAndNonZero: \
		return inContext.Error(NoParamErrorID::TosShouldBeNonZero); \
	} \
} while(0)

/* TwoOp use inContext */
#define TwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	AssignOpToSecond(inContext,second,OP,tos); \
	inContext.DS.pop_back(); \
	NEXT; \
} while(0)

#define RefTwoOp(inStack,OP) do { \
	if(inStack.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos   =ReadTOS(inStack); \
	TypedValue& second=ReadSecond(inStack); \
	PushOp(inStack,second,OP,tos); \
	NEXT; \
} while(0)

#define CmpOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(tos.dataType==DataType::Int \
	   && second.dataType==DataType::Int) { \
		second.dataType=DataType::Bool; \
		second.boolValue=second.intValue OP tos.intValue; \
	} else if(tos.dataType==DataType::Long \
			  && second.dataType==DataType::Long) { \
		second.dataType=DataType::Bool; \
		second.boolValue=second.longValue OP tos.longValue; \
	} else if(tos.dataType==DataType::Double \
			  && second.dataType==DataType::Double) { \
		second.dataType=DataType::Bool; \
		second.boolValue=second.doubleValue OP tos.doubleValue; \
	} else if(tos.dataType==DataType::Float \
			  && second.dataType==DataType::Float) { \
		second.dataType=DataType::Bool; \
		second.boolValue=second.floatValue OP tos.floatValue; \
	} else if(tos.dataType==DataType::String \
			  && second.dataType==DataType::String) { \
		second.dataType=DataType::Bool; \
		second.boolValue=*second.stringPtr OP *tos.stringPtr; \
	} else if(tos.dataType==DataType::BigInt \
			  && second.dataType==DataType::BigInt) { \
		second.dataType=DataType::Bool; \
		second.boolValue=*(second.bigIntPtr) OP *(tos.bigIntPtr); \
	} else if(second.dataType==DataType::Bool \
			  && tos.dataType==DataType::Bool) { \
DISABLE_WARNING_ARRAY_BOUNDS \
		if(#OP[0]=='=' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="==" */ \
			second.boolValue = second.boolValue == tos.boolValue; \
		} else if(#OP[0]=='!' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="!=" */ \
			second.boolValue = second.boolValue != tos.boolValue; \
		} else { \
			goto onError; \
		} \
ENABLE_WARNING_ARRAY_BOUNDS \
	} else if(second.dataType==DataType::List \
			  && tos.dataType==DataType::List) { \
		if(#OP[0]=='=' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="==" */ \
			bool result = IsSameValue(second,tos); \
			second.listPtr.reset();	\
			second.dataType=DataType::Bool; \
			second.boolValue=result; \
		} else if(#OP[0]=='!' && #OP[1]=='=' && #OP[2]=='\0') { /* #OP=="!=" */ \
			bool result = !(IsSameValue(second,tos)); \
			second.listPtr.reset();	\
			second.dataType=DataType::Bool; \
			second.boolValue=result; \
		} else { \
			goto onError; \
		} \
	} else { \
		switch(second.dataType) { \
			case DataType::Int: \
				switch(tos.dataType) { \
					case DataType::Long: \
						second.dataType=DataType::Bool; \
						second.boolValue=(long)second.intValue  OP tos.longValue; \
						break; \
					case DataType::BigInt: { \
							BigInt bi(second.intValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case DataType::Float: \
						second.dataType=DataType::Bool; \
						second.boolValue=(float)second.intValue OP tos.floatValue; \
						break; \
					case DataType::Double: \
						second.dataType=DataType::Bool; \
						second.boolValue=((double)second.intValue) \
											OP (tos.doubleValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Long: \
				switch(tos.dataType) { \
					case DataType::Int: \
						second.dataType=DataType::Bool; \
						second.boolValue=second.longValue OP (long)tos.intValue; \
						break; \
					case DataType::BigInt: { \
							BigInt bi(second.longValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=bi OP *tos.bigIntPtr; \
						} \
						break; \
					case DataType::Float: \
						second.dataType=DataType::Bool; \
						second.boolValue=(float)second.longValue OP tos.floatValue; \
						break; \
					case DataType::Double: \
						second.dataType=DataType::Bool; \
						second.boolValue=(double)second.longValue OP tos.doubleValue; \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::BigInt: \
				switch(tos.dataType) { \
					case DataType::Int: { \
							BigInt bi(tos.intValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case DataType::Long: { \
							BigInt bi(tos.longValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=*second.bigIntPtr OP bi; \
						} \
						break; \
					case DataType::Float: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							BF100 s(*second.bigIntPtr); \
							BF100 t(tos.floatValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::Double: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							BF100 s(*second.bigIntPtr); \
							BF100 t(tos.doubleValue); \
							second.dataType=DataType::Bool; \
							second.boolValue=s OP t; \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Float: \
				switch(tos.dataType) { \
					case DataType::Int: \
						second.dataType=DataType::Bool; \
						second.boolValue=second.floatValue OP (float)tos.intValue; \
						break; \
					case DataType::Long: \
						second.dataType=DataType::Bool; \
						second.boolValue=(double)second.floatValue \
											OP (double)tos.longValue; \
						break; \
					case DataType::BigInt: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							BF100 s(second.floatValue); \
							BF100 t(*tos.bigIntPtr); \
							second.dataType=DataType::Bool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::Double: \
						second.dataType=DataType::Bool; \
						second.boolValue=(double)second.floatValue \
											OP tos.doubleValue; \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Double: \
				switch(tos.dataType) { \
					case DataType::Int: \
						second.dataType=DataType::Bool; \
						second.boolValue=second.doubleValue OP (double)tos.intValue; \
						break; \
					case DataType::Long: \
						second.dataType=DataType::Bool; \
						second.boolValue=second.doubleValue OP (double)tos.longValue; \
						break; \
					case DataType::BigInt: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							BF100 s(second.doubleValue); \
							BF100 t(*tos.bigIntPtr); \
							second.dataType=DataType::Bool; \
							second.boolValue=s OP t; \
						} \
						break; \
					case DataType::Float: \
						second.dataType=DataType::Bool; \
						second.boolValue=second.doubleValue \
											OP (double)tos.floatValue; \
						break; \
					default: goto onError; \
				} \
				break; \
			default: goto onError; \
		} \
	} \
	NEXT; \
onError: \
	return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
									   std::string(#OP),second,tos); \
} while(0)

#define RefCmpOpMain(inStack,inSecond,OP,inTos) do { \
	if(inSecond.dataType==DataType::Int \
	   && inTos.dataType==DataType::Int) { \
		inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
	} else if(inSecond.dataType==DataType::Long \
			  && inTos.dataType==DataType::Long) { \
		inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
	} else if(inSecond.dataType==DataType::Double \
			  || inTos.dataType==DataType::Double) { \
		inStack.emplace_back(inSecond.doubleValue OP inTos.doubleValue); \
	} else if(inSecond.dataType==DataType::Float \
			  || inTos.dataType==DataType::Float) { \
		inStack.emplace_back(inSecond.floatValue OP inTos.floatValue); \
	} else if(inSecond.dataType==DataType::String \
			  && inTos.dataType==DataType::String) { \
		inStack.emplace_back(*(inSecond.stringPtr) OP *(inTos.stringPtr)); \
	} else if(inSecond.dataType==DataType::BigInt \
			  && inTos.dataType==DataType::BigInt) { \
		inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
	} else { \
		switch(inSecond.dataType) { \
			case DataType::Int: \
				switch(inTos.dataType) { \
					case DataType::Long: /* int x long -> bool */ \
						inStack.emplace_back( \
							(long)inSecond.intValue  OP inTos.longValue); \
						break; \
					case DataType::BigInt: /* int x bigInt -> bool */ \
						inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
						break; \
					case DataType::Float: /* int x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.intValue OP inTos.floatValue); \
						break; \
					case DataType::Double: /* int x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.intValue OP inTos.doubleValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Long: \
				switch(inTos.dataType) { \
					case DataType::Int: /* long x int -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP (long)inTos.intValue); \
						break; \
					case DataType::BigInt: /* long x bigIng -> bool */ \
						inStack.emplace_back( \
							inSecond.longValue OP *(inTos.bigIntPtr)); \
						break; \
					case DataType::Float: /* long x float -> bool */ \
						inStack.emplace_back( \
							(float)inSecond.longValue OP inTos.floatValue); \
						break; \
					case DataType::Double: /* long x double -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.longValue OP inTos.doubleValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::BigInt: \
				switch(inTos.dataType) { \
					case DataType::Int: /* bigInt x int -> bool */ \
						inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
						break; \
					case DataType::Long: /* bigInt x long -> bool */ \
						inStack.emplace_back( \
							*(inSecond.bigIntPtr) OP inTos.longValue); \
						break; \
					case DataType::Float: /* bigInt x float -> bool */ \
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
							inContext.Error(NoParamErrorID::SystemError); \
						} \
						break; \
					case DataType::Double: /* bigInt x double -> bool */ \
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
							inContext.Error(NoParamErrorID::SystemError); \
						} \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Float: \
				switch(inTos.dataType) { \
					case DataType::Int: /* float x int -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP (float)inTos.intValue); \
						break; \
					case DataType::Long: /* float x long -> bool */ \
						inStack.emplace_back( \
							(double)inSecond.floatValue OP (double)inTos.longValue); \
						break; \
					case DataType::BigInt: /* float x bigInt -> bool */ \
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
							inContext.Error(NoParamErrorID::SystemError); \
						} \
						break; \
					case DataType::Double: /* float x double -> bool */ \
						inStack.emplace_back( \
							inSecond.floatValue OP inTos.doubleValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			case DataType::Double: \
				switch(inTos.dataType) { \
					case DataType::Int: /* double x int -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP tos.intValue); \
						break; \
					case DataType::Long: /* double x long -> bool */ \
						inStack.emplace_back(inSecond.doubleValue OP inTos.longValue); \
						break; \
					case DataType::BigInt: /* double x bigInt -> bool */ \
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
							inContext.Error(NoParamErrorID::SystemError); \
						} \
						break; \
					case DataType::Float: /* double x float -> bool */ \
						inStack.emplace_back( \
							inSecond.doubleValue OP inTos.floatValue); \
						break; \
					default: goto onError; \
				} \
				break; \
			default: goto onError; \
		} \
	} \
	goto onSuccess; \
onError: \
	return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
									   std::string(#OP),inSecond,inTos); \
onSuccess: ; \
} while(0)

#define RefCmpOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefCmpOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

// result=tv1 OP tv2
// ex: GetIntegerCmpOpResult_AndConvert(a,second,<,tos)
// 	   equivalent to: a=second<tos;
#define GetIntegerCmpOpResult_AndConvert(result,tv1,OP,tv2) do { \
	if(tv1.dataType==DataType::Int && tv2.dataType==DataType::Int) { \
		result=tv1.intValue OP tv2.intValue; \
	} else if(tv1.dataType==DataType::Long \
			  && tv2.dataType==DataType::Long) { \
		result=tv1.longValue OP tv2.longValue; \
	} else if(tv1.dataType==DataType::BigInt \
			  && tv2.dataType==DataType::BigInt) { \
		result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
	} else { \
		switch(tv1.dataType) { \
			case DataType::Int: \
				if(tv2.dataType==DataType::Long) { \
					tv1.dataType=DataType::Long; \
					tv1.longValue=(long)tv1.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(tv2.dataType==DataType::BigInt); \
					tv1.dataType=DataType::BigInt; \
					tv1.bigIntPtr=new BigInt(tv1.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case DataType::Long: \
				if(tv2.dataType==DataType::Int) { \
					tv2.dataType=DataType::Long; \
					tv2.longValue=(long)tv2.intValue; \
					result=tv1.longValue OP tv2.longValue; \
				} else { \
					assert(rsSecond.dataType==DataType::BigInt); \
					tv1.dataType=DataType::BigInt; \
					tv1.bigIntPtr=new BigInt(tv1.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			case DataType::BigInt: \
				if(tv2.dataType==DataType::Int) { \
					tv2.dataType=DataType::BigInt; \
					tv2.bigIntPtr=new BigInt(tv2.intValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} else { \
					assert(tv2.dataType==DataType::Long); \
					tv2.dataType=DataType::BigInt; \
					tv2.bigIntPtr=new BigInt(tv2.longValue); \
					result=(*tv1.bigIntPtr) OP (*tv2.bigIntPtr); \
				} \
				break; \
			default: \
				return inContext.Error(NoParamErrorID::SystemError); \
		} \
	} \
} while(0)

// result=tv1 OP tv2
// ex: GetCmpOpResult(t,second,<,tos);
//     equivalent to t=second<tos;
#define GetCmpOpResult(result,tv1,OP,tv2) do { \
	if(tv1.dataType==tv2.dataType) { \
		switch(tv1.dataType) { \
			case DataType::Int: \
				result=tv1.intValue OP tv2.intValue; \
				break; \
			case DataType::Long: \
				result=tv1.longValue OP tv2.longValue; \
				break; \
			case DataType::BigInt: \
				result=*(tv1.bigIntPtr) OP *(tv2.bigIntPtr); \
				break; \
			case DataType::Float: \
				result=tv1.floatValue OP tv2.floatValue; \
	  	 		break; \
			case DataType::Double:	\
				result=tv1.doubleValue OP tv2.doubleValue; \
				break; \
			default: \
				result=false; \
		} \
	} else { \
		switch(tv1.dataType) { \
			case DataType::Int: \
				switch(tv2.dataType) { \
					case DataType::Long: \
						result=((long)tv1.intValue) OP tv2.longValue; \
						break; \
					case DataType::BigInt: \
						result=tv1.intValue OP *(tv2.bigIntPtr); \
						break; \
					case DataType::Float: \
						result=((float)tv1.intValue) OP tv2.floatValue; \
						break; \
					case DataType::Double: \
						result=((double)tv1.intValue) OP tv2.doubleValue; \
						break; \
					default: \
						result=false; \
				} \
				break; \
			case DataType::Long: \
				switch(tv2.dataType) { \
					case DataType::Int: \
						result=tv1.longValue OP ((long)tv2.intValue); \
						break; \
					case DataType::BigInt: \
						result=tv1.longValue OP *(tv2.bigIntPtr); \
						break; \
					case DataType::Float: \
						result=((float)(tv1.longValue)) OP tv2.floatValue; \
						break; \
					case DataType::Double: \
						result=((double)tv1.longValue) OP tv2.doubleValue; \
						break; \
					default: \
						result=false; \
				} \
				break; \
			case DataType::BigInt: \
				switch(tv2.dataType) { \
					case DataType::Int: \
						result=*(tv1.bigIntPtr) OP tv2.intValue; break; \
					case DataType::Long: \
						result=*(tv1.bigIntPtr) OP tv2.longValue; break; \
					case DataType::Float: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							result=BF100(*tv1.bigIntPtr) OP tv2.floatValue; \
						} \
						break; \
					case DataType::Double: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							result=BF100(*tv1.bigIntPtr) OP tv2.doubleValue; \
						} \
						break; \
					default: result=false; \
				} \
				break; \
			case DataType::Float: \
				switch(tv2.dataType) { \
					case DataType::Int: \
						result=tv1.floatValue OP (float)tv2.intValue; break; \
					case DataType::Long: \
						result=tv1.floatValue OP (float)tv2.longValue; break; \
					case DataType::BigInt: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							result=tv1.floatValue OP BF100(*tv2.bigIntPtr); \
						} \
						break; \
					case DataType::Double: \
						result=(double)tv1.floatValue OP tv2.doubleValue; break; \
					default: result=false; \
				} \
				break; \
			case DataType::Double: \
				switch(tv2.dataType) { \
					case DataType::Int: \
						result=tv1.doubleValue OP (double)tv2.intValue; break; \
					case DataType::Long: \
						result=tv1.doubleValue OP (double)tv2.longValue; break; \
					case DataType::BigInt: { \
							typedef boost::multiprecision::cpp_dec_float_100 BF100; \
							result=tv1.doubleValue OP BF100(*tv2.bigIntPtr); \
						} \
						break; \
					case DataType::Float: \
						result=tv1.doubleValue OP (double)tv2.floatValue; break; \
					default: result=false; \
				} \
				break; \
			default: /* never comes here */ \
				result=false; \
		} \
	} \
} while(0);

#define BitOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			switch(tos.dataType) { \
				case DataType::Int: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case DataType::Long: \
					second.dataType=DataType::Long; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case DataType::BigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=DataType::BigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::Long: \
			switch(tos.dataType) { \
				case DataType::Int: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case DataType::Long: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case DataType::BigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						*biPtr=(*biPtr) OP (*tos.bigIntPtr); \
						second.dataType=DataType::BigInt; \
						second.bigIntPtr=biPtr; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::BigInt: \
			switch(tos.dataType) { \
				case DataType::Int: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case DataType::Long: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case DataType::BigInt: \
					*second.bigIntPtr=(*second.bigIntPtr) OP (*tos.bigIntPtr); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case DataType::Int: \
			switch(inTos.dataType) { \
				case DataType::Int: /* int x int -> int  */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case DataType::Long: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* int x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.intValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::Long: \
			switch(inTos.dataType) { \
				case DataType::Int: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case DataType::Long: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* long x bigInt -> bigInt */ \
					inStack.emplace_back(inSecond.longValue OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::BigInt: \
			switch(inTos.dataType) { \
				case DataType::Int: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case DataType::Long: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* bigInt x bigInt -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP *(inTos.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,second); \
	} \
	NEXT; \
} while(0)

#define RefBitOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			switch(tos.dataType) { \
				case DataType::Int: \
					second.intValue=second.intValue OP tos.intValue; \
					break; \
				case DataType::Long: \
					second.dataType=DataType::Long; \
					second.longValue=(long)second.intValue OP tos.longValue; \
					break; \
				case DataType::BigInt: { \
						BigInt *biPtr=new BigInt(second.intValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=DataType::BigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::Long: \
			switch(tos.dataType) { \
				case DataType::Int: \
					second.longValue=second.longValue OP tos.intValue; \
					break; \
				case DataType::Long: \
					second.longValue=second.longValue OP tos.longValue; \
					break; \
				case DataType::BigInt: { \
						BigInt *biPtr=new BigInt(second.longValue); \
						if(*tos.bigIntPtr<=LONG_MAX) { \
							*biPtr=(*biPtr) OP static_cast<long>(*tos.bigIntPtr); \
							second.dataType=DataType::BigInt; \
							second.bigIntPtr=biPtr; \
						} else { \
							inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
							printf("TOS   : "); tos.Dump(); \
							printf("SECOND: "); second.Dump(); \
							return false; \
						} \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		case DataType::BigInt: \
			switch(tos.dataType) { \
				case DataType::Int: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.intValue; \
					break; \
				case DataType::Long: \
					*second.bigIntPtr=*second.bigIntPtr OP tos.longValue; \
					break; \
				case DataType::BigInt: \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						*second.bigIntPtr=(*second.bigIntPtr) \
											OP static_cast<long>(*tos.bigIntPtr); \
					} else { \
						inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
						printf("TOS   : "); tos.Dump(); \
						printf("SECOND: "); second.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,second); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOpMain(inStack,inSecond,OP,inTos) do { \
	switch(inSecond.dataType) { \
		case DataType::Int: \
			switch(inTos.dataType) { \
				case DataType::Int: /* int x int -> int */ \
					inStack.emplace_back(inSecond.intValue OP inTos.intValue); \
					break; \
				case DataType::Long: /* int x long -> long */ \
					inStack.emplace_back((long)inSecond.intValue OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* int x bigInt -> bigInt */ \
					if(*inTos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.intValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inTos); \
			} \
			break; \
		case DataType::Long: \
			switch(inTos.dataType) { \
				case DataType::Int: /* long x int -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.intValue); \
					break; \
				case DataType::Long: /* long x long -> long */ \
					inStack.emplace_back(inSecond.longValue OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* long x bigInt -> bigInt */ \
					if(*tos.bigIntPtr<=LONG_MAX) { \
						inStack.emplace_back( \
							BigInt(inSecond.longValue) OP \
							static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inTos); \
			} \
			break; \
		case DataType::BigInt: \
			switch(inTos.dataType) { \
				case DataType::Int: /* bigInt x int -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.intValue); \
					break; \
				case DataType::Long: /* bigInt x long -> bigInt */ \
					inStack.emplace_back(*(inSecond.bigIntPtr) OP inTos.longValue); \
					break; \
				case DataType::BigInt: /* bigInt x bigInt -> bigInt */ \
					if(*(inTos.bigIntPtr)<=LONG_MAX) { \
						inStack.emplace_back(*(inSecond.bigIntPtr) \
											OP static_cast<long>(*inTos.bigIntPtr)); \
					} else { \
						inContext.Error(NoParamErrorID::SorryNotSupportThisOperation); \
						printf("TOS   : "); inTos.Dump(); \
						printf("SECOND: "); inSecond.Dump(); \
						return false; \
					} \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inTos); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,inSecond); \
	} \
	NEXT; \
} while(0)

#define RefBitShiftOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	TypedValue& second=ReadSecond(inContext.DS); \
	RefBitShiftOpMain(inContext.DS,second,OP,tos); \
	NEXT; \
} while(0)

#define BoolOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	if(tos.dataType!=DataType::Bool) { \
		return inContext.Error(InvalidTypeErrorID::TosBool,tos); \
	} \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(second.dataType!=DataType::Bool) { \
		return inContext.Error(InvalidTypeErrorID::SecondBool,second); \
	} \
	second.boolValue=tos.boolValue OP second.boolValue; \
	NEXT; \
} while(0)

#define OneArgFloatingFunc(FUNC) do { \
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	if(tos.dataType==DataType::BigInt) { \
		typedef boost::multiprecision::cpp_dec_float_100 BF100; \
		BF100 tmp=FUNC(BF100(*tos.bigIntPtr)); \
		SetResultFromBF100(tos,tmp); \
		goto next; \
	} \
	double result; \
	switch(tos.dataType) { \
		case DataType::Int: 	result=FUNC((double)tos.intValue);		break; \
		case DataType::Long:	result=FUNC((double)tos.longValue);		break; \
		case DataType::Float:	result=FUNC((double)tos.floatValue);	break; \
		case DataType::Double:	result=FUNC(tos.doubleValue);			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::TosDoublable,tos); \
	} \
	tos.dataType=DataType::Double; \
	tos.doubleValue=result; \
next: \
	NEXT; \
} while(0)

#define RefOneArgFloatingFunc(FUNC) do { \
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	if(tos.dataType==DataType::BigInt) { \
		typedef boost::multiprecision::cpp_dec_float_100 BF100; \
		BF100 tmp=FUNC(BF100(*tos.bigIntPtr)); \
		TypedValue tv; \
		SetResultFromBF100(tv,tmp); \
		inContext.DS.emplace_back(tv); \
		goto next; \
	} \
	double result; \
	switch(tos.dataType) { \
		case DataType::Int: 	result=FUNC((double)tos.intValue);		break; \
		case DataType::Long:	result=FUNC((double)tos.longValue);		break; \
		case DataType::Float:	result=FUNC((double)tos.floatValue);	break; \
		case DataType::Double:	result=FUNC(tos.doubleValue);			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::TosDoublable,tos); \
	} \
	inContext.DS.emplace_back(result); \
next: \
	NEXT; \
} while(0)

#define OneParamFunc(FUNC) do { \
	if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); } \
	TypedValue& tos=ReadTOS(inContext.DS); \
	switch(tos.dataType) { \
		case DataType::Int: \
			tos.doubleValue=FUNC((double)tos.intValue); \
			tos.dataType=DataType::Double; \
			break; \
		case DataType::Long: \
			tos.doubleValue=FUNC((double)tos.longValue); \
			tos.dataType=DataType::Double; \
			break; \
		case DataType::Float: \
			tos.doubleValue=FUNC((double)tos.floatValue); \
			tos.dataType=DataType::Double; \
			break; \
		case DataType::Double: \
			tos.doubleValue=FUNC(tos.doubleValue); \
			break; \
		case DataType::BigInt: { \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=FUNC(BF100(*tos.bigIntPtr)); \
				SetResultFromBF100(tos,tmp); \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::TosNumber,tos); \
	} \
	NEXT; \
} while(0)

// c=a%b
// ModOp(c,a,b)
#define ModOp(outDest,inSrc1,inSrc2) do { \
	if(inSrc1.dataType==DataType::Int && inSrc2.dataType==DataType::Int) { \
		outDest=TypedValue(inSrc1.intValue % inSrc2.intValue); \
	} else if(inSrc1.dataType==DataType::Long \
			  && inSrc2.dataType==DataType::Long) { \
		outDest=TypedValue(inSrc1.longValue % inSrc2.longValue); \
	} else if(inSrc1.dataType==DataType::BigInt \
			  && inSrc2.dataType==DataType::BigInt) { \
		outDest=TypedValue(*(inSrc1.bigIntPtr) % *(inSrc2.bigIntPtr)); \
	} else { \
		if(inSrc1.dataType==DataType::Int) { \
			switch(inSrc2.dataType) { \
				case DataType::Long: \
					outDest=TypedValue((long)inSrc1.intValue % inSrc2.longValue); \
					break; \
				case DataType::BigInt: \
					outDest=TypedValue(BigInt(inSrc1.intValue) % *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inSrc2); \
			} \
		  } else if(inSrc1.dataType==DataType::Long) { \
			switch(inSrc2.dataType) { \
				case DataType::Int: \
					outDest=TypedValue(inSrc1.longValue % (long)inSrc2.intValue); \
					break; \
				case DataType::BigInt: \
					outDest=TypedValue(BigInt(inSrc1.longValue) \
								  		% *(inSrc2.bigIntPtr)); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inSrc2); \
			} \
		} else if(inSrc1.dataType==DataType::BigInt) { \
			switch(inSrc2.dataType) { \
				case DataType::Int: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.intValue); \
					break; \
				case DataType::Long: \
					outDest=TypedValue(*(inSrc1.bigIntPtr) % inSrc2.longValue); \
					break; \
				default: \
					return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,inSrc2); \
			} \
		} else { \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,inSrc1); \
		} \
	} \
} while(0)

// a %= b
// ModAssign(a,b);
#define ModAssign(inTarget,inTV) do { \
	const int combinationType=GetMathOpType(inTarget.dataType,inTV.dataType); \
	switch((TypeCombinationForMathOP)combinationType) {\
		case TypeCombinationForMathOP::IntInt:  \
			inTarget.intValue %= inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::IntLong: \
			inTarget.longValue=(long)inTarget.intValue % inTV.longValue; \
			inTarget.dataType=DataType::Long; \
			break; \
		case TypeCombinationForMathOP::IntBigInt: \
			inTarget.bigIntPtr=new BigInt(inTarget.intValue % *inTV.bigIntPtr); \
			inTarget.dataType=DataType::BigInt; \
			break; \
		case TypeCombinationForMathOP::LongInt: \
			inTarget.longValue %= (long)inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::LongLong: \
			inTarget.longValue %= inTV.longValue; \
			break; \
		case TypeCombinationForMathOP::LongBigInt: \
			inTarget.bigIntPtr=new BigInt(inTarget.longValue % *inTV.bigIntPtr); \
			inTarget.dataType=DataType::BigInt; \
			break; \
		case TypeCombinationForMathOP::BigIntInt: \
			*inTarget.bigIntPtr %= inTV.intValue; \
			break; \
		case TypeCombinationForMathOP::BigIntLong: \
			*inTarget.bigIntPtr %= inTV.longValue; \
			break; \
		case TypeCombinationForMathOP::BigIntBigInt: \
			*inTarget.bigIntPtr %= (*inTV.bigIntPtr); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,inTarget); \
	} \
} while(0)

// Push(a % b)
// RefMod(inContext.DS,a,b);
#define RefMod(inStack,inSecond,inTos) do { \
	const int combinationType=GetMathOpType(inSecond.dataType,inTos.dataType); \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::IntInt:  \
			inStack.emplace_back(inSecond.intValue % inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::IntLong: \
			inStack.emplace_back((long)inSecond.intValue % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::IntBigInt: \
			inStack.emplace_back(inSecond.intValue % *(inTos.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::LongInt: \
			inStack.emplace_back(inSecond.longValue % (long)inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::LongLong: \
			inStack.emplace_back(inSecond.longValue % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::LongBigInt: \
			inStack.emplace_back(inSecond.longValue % *(inTos.bigIntPtr)); \
			break; \
		case TypeCombinationForMathOP::BigIntInt: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.intValue); \
			break; \
		case TypeCombinationForMathOP::BigIntLong: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % inTos.longValue); \
			break; \
		case TypeCombinationForMathOP::BigIntBigInt: \
			inStack.emplace_back(*(inSecond.bigIntPtr) % *(inTos.bigIntPtr)); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,inSecond); \
	} \
} while(0)

// ModAssignTOS
// tos = second % tos
#define DIVIDE_BY_ZERO_ModAssignTOS MakeLabel(divideByZero,__LINE__)
#define ModAssignTOS(inContext,inSecond,inTOS) { \
	const int combinationType=GetMathOpType(inSecond.dataType,inTOS.dataType); \
	switch((TypeCombinationForMathOP)combinationType) { \
		case TypeCombinationForMathOP::IntInt:  \
			inTOS.intValue=inSecond.intValue % inTOS.intValue; \
			break; \
		case TypeCombinationForMathOP::IntLong: \
			inTOS.longValue=(long)inSecond.intValue % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::IntBigInt: \
			*(inTOS.bigIntPtr)=inSecond.intValue % (*inTOS.bigIntPtr); \
			break; \
		case TypeCombinationForMathOP::LongInt: \
			inTOS.longValue=inSecond.longValue % (long)inTOS.intValue; \
			inTOS.dataType=DataType::Long; \
			break; \
		case TypeCombinationForMathOP::LongLong: \
			inTOS.longValue=inSecond.longValue % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::LongBigInt: \
			*inTOS.bigIntPtr=(*inSecond.bigIntPtr) % inTOS.longValue; \
			break; \
		case TypeCombinationForMathOP::BigIntInt: \
			inTOS.bigIntPtr=new BigInt((*inSecond.bigIntPtr) % inTOS.intValue); \
			inTOS.dataType=DataType::BigInt; \
			break; \
		case TypeCombinationForMathOP::BigIntLong: \
			inTOS.bigIntPtr=new BigInt((*inSecond.bigIntPtr) % inTOS.longValue); \
			inTOS.dataType=DataType::BigInt; \
			break; \
		case TypeCombinationForMathOP::BigIntBigInt: \
			*inTOS.bigIntPtr=(*inSecond.bigIntPtr) % (*inTOS.bigIntPtr); \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::SecondIntOrLongOrBigint,inSecond); \
	} \
} while(0)

#define FloorOrCeil(FUNC,ioTV) do { \
	switch(ioTV.dataType) { \
		case DataType::Int: \
		case DataType::Long: \
		case DataType::BigInt: \
			break; \
		case DataType::Float: { \
				float f=FUNC(ioTV.floatValue); \
				if(INT_MIN<=f && f<=INT_MAX) { \
					ioTV.intValue=(int)f; \
					ioTV.dataType=DataType::Int; \
				} else if(LONG_MIN<=f && f<=LONG_MAX) { \
					ioTV.longValue=(long)f; \
					ioTV.dataType=DataType::Long; \
				} else { \
					ioTV.bigIntPtr=new BigInt(f); \
					ioTV.dataType=DataType::BigInt; \
				} \
			} \
			break; \
		case DataType::Double: { \
				double t=FUNC(ioTV.doubleValue); \
				if(INT_MIN<=t && t<=INT_MAX) { \
					ioTV.intValue=(int)t; \
					ioTV.dataType=DataType::Int; \
				} else if(LONG_MIN<=t && t<=LONG_MAX) { \
					ioTV.longValue=(long)t; \
					ioTV.dataType=DataType::Long; \
				} else { \
					ioTV.bigIntPtr=new BigInt(t); \
					ioTV.dataType=DataType::BigInt; \
				} \
			} \
			break; \
		default: \
			return inContext.Error(InvalidTypeErrorID::TosNumber,ioTV); \
	} \
} while(0)

// X int OP
#define IntTwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	const int n=tos.intValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			second.intValue=second.intValue OP n; \
			break; \
		case DataType::Long: \
			second.longValue=second.longValue OP n; \
			break; \
		case DataType::Float: \
			second.floatValue=second.floatValue OP n; \
			break; \
		case DataType::Double: \
			second.doubleValue=second.doubleValue OP n; \
			break; \
		case DataType::BigInt: \
			*(second.bigIntPtr)=*(second.bigIntPtr) OP n; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X long OP
#define LongTwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	const long n=tos.longValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			second.dataType=DataType::Long; \
			second.longValue=second.intValue OP n; \
  			break; \
		case DataType::Long: \
			second.longValue=second.longValue OP n; \
			break; \
		case DataType::Float: \
			second.floatValue=second.floatValue OP n; \
			break; \
		case DataType::Double: \
			second.doubleValue=second.doubleValue OP n; \
			break; \
		case DataType::BigInt:	\
			*(second.bigIntPtr)=*(second.bigIntPtr) OP n; \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X float OP
#define FloatTwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	const float t=tos.floatValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			second.dataType=DataType::Float; \
			second.floatValue=second.intValue OP t; \
  			break; \
		case DataType::Long: \
			second.dataType=DataType::Float; \
			second.floatValue=second.longValue OP t; \
			break; \
		case DataType::Float: \
			second.floatValue=second.floatValue OP t; \
			break; \
		case DataType::Double:	\
			second.doubleValue=second.doubleValue OP t; \
			break; \
		case DataType::BigInt: { \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=BF100(*second.bigIntPtr) OP t; \
				SetResultFromBF100(second,tmp); \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X double OP
#define DoubleTwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	const double t=tos.doubleValue; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: \
			second.dataType=DataType::Double; \
			second.doubleValue=second.intValue OP t; \
  			break; \
		case DataType::Long: \
			second.dataType=DataType::Double; \
			second.doubleValue=second.longValue OP t; \
			break; \
		case DataType::Float: \
			second.dataType=DataType::Double; \
			second.doubleValue=second.floatValue OP t; \
			break; \
		case DataType::Double:	\
			second.doubleValue=second.doubleValue OP t; \
			break; \
		case DataType::BigInt: { \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=BF100(*second.bigIntPtr) OP t; \
				SetResultFromBF100(second,tmp); \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

// X bigInt OP
#define BigIntTwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(NoParamErrorID::DsAtLeast2); } \
	TypedValue tos=Pop(inContext.DS); \
	const BigInt *t=tos.bigIntPtr; \
	TypedValue& second=ReadTOS(inContext.DS); \
	switch(second.dataType) { \
		case DataType::Int: { \
				BigInt *bigInt=new BigInt(second.intValue); \
				*bigInt=*bigInt OP (*t); \
				second.bigIntPtr=bigInt; \
				second.dataType=DataType::BigInt; \
			} \
  			break; \
		case DataType::Long: { \
				BigInt *bigInt=new BigInt(second.longValue); \
				*bigInt=*bigInt OP (*t); \
				second.bigIntPtr=bigInt; \
				second.dataType=DataType::BigInt; \
			} \
			break; \
		case DataType::Float: { \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=second.floatValue OP BF100(*tos.bigIntPtr); \
				SetResultFromBF100(second,tmp); \
			} \
			break; \
		case DataType::Double: { \
				typedef boost::multiprecision::cpp_dec_float_100 BF100; \
				BF100 tmp=second.doubleValue OP BF100(*tos.bigIntPtr); \
				SetResultFromBF100(second,tmp); \
			} \
			break; \
		case DataType::BigInt: \
			*(second.bigIntPtr)=*(second.bigIntPtr) OP (*t); \
			break; \
		default: \
			return inContext.Error_InvalidType(InvalidTypeStrTvTvErrorID::ForOp, \
											   std::string(#OP),second,tos); \
	} \
	NEXT; \
} while(0)

