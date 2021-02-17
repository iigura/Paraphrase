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

static double deg2rad(double inTheta) { return inTheta/180.0*M_PI; }
static BigFloat deg2rad(BigFloat inTheta) { return inTheta/180.0*M_PI; }
static double rad2deg(double inTheta) { return inTheta/M_PI*180.0; }
static BigFloat rad2deg(BigFloat inTheta) { return inTheta/M_PI*180.0; }

static int    maxOp(int     inA,int inB)     { return std::max(inA,inB); }
static long   maxOp(long    inA,long inB)    { return std::max(inA,inB); }
static float  maxOp(float   inA,float inB)   { return std::max(inA,inB); }
static double maxOp(double  inA,double inB)  { return std::max(inA,inB); }
static BigInt maxOp(BigInt inA,BigInt inB) {
	return inA>inB ? inA : inB;
}
static BigFloat maxOp(BigFloat inA,BigFloat inB) {
	return inA>inB ? inA : inB;
}

static int    minOp(int     inA,int inB)     { return std::min(inA,inB); }
static long   minOp(long    inA,long inB)    { return std::min(inA,inB); }
static float  minOp(float   inA,float inB)   { return std::min(inA,inB); }
static double minOp(double  inA,double inB)  { return std::min(inA,inB); }
static BigInt minOp(BigInt inA,BigInt inB) {
	return inA<inB ? inA : inB;
}
static BigFloat minOp(BigFloat inA,BigFloat inB) {
	return inA<inB ? inA : inB;
}

void InitDict_Math() {
	Install(new Word("true",WORD_FUNC {
		inContext.DS.emplace_back(true);
		NEXT;
	}));

	Install(new Word("false",WORD_FUNC {
		inContext.DS.emplace_back(false);
		NEXT;
	}));

	Install(new Word("+",WORD_FUNC { TwoOp(+); },LVOP::LVOpSupported2args| LVOP::ADD));
	Install(new Word("-",WORD_FUNC { TwoOp(-); },LVOP::LVOpSupported2args| LVOP::SUB));
	Install(new Word("*",WORD_FUNC { TwoOp(*); },LVOP::LVOpSupported2args| LVOP::MUL));
	Install(new Word("/",WORD_FUNC { TwoOp(/); },LVOP::LVOpSupported2args| LVOP::DIV));
	Install(new Word("%",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		CheckIntegerAndNonZero(tos);
		TypedValue& second=ReadTOS(inContext.DS);
		ModAssign(second,tos); 	// second %= tos;
		NEXT;
	}));


	Install(new Word("2*",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		tos.intValue	   *= 2;	break;
			case DataType::kTypeLong:		tos.longValue	   *= 2L;	break;
			case DataType::kTypeBigInt:		*(tos.bigIntPtr)   *= 2;	break;
			case DataType::kTypeFloat:		tos.floatValue	   *= 2.0f;	break;
			case DataType::kTypeDouble:		tos.doubleValue	   *= 2.0;	break;
			case DataType::kTypeBigFloat:	*(tos.bigFloatPtr) *= 2;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args | LVOP::TWC));

	Install(new Word("@+",WORD_FUNC { RefTwoOp(inContext.DS,+); }));
	Install(new Word("@-",WORD_FUNC { RefTwoOp(inContext.DS,-); }));
	Install(new Word("@*",WORD_FUNC { RefTwoOp(inContext.DS,*); }));
	Install(new Word("@/",WORD_FUNC { RefTwoOp(inContext.DS,/); }));
	Install(new Word("@%",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		TypedValue& second=ReadSecond(inContext.DS);
		CheckIntegerAndNonZero(tos);
		RefMod(inContext.DS,second,tos);
		NEXT;
	}));


	Install(new Word("+@",WORD_FUNC { TwoOpAssignTOS(+); }));
	Install(new Word("-@",WORD_FUNC { TwoOpAssignTOS(-); }));
	Install(new Word("*@",WORD_FUNC { TwoOpAssignTOS(*); }));
	Install(new Word("/@",WORD_FUNC { TwoOpAssignTOS(/); }));
	Install(new Word("%@",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		TypedValue& second=ReadSecond(inContext.DS);
		CheckIntegerAndNonZero(tos);
		ModAssignTOS(inContext,second,tos);
		NEXT;
	}));

	Install(new Word("&",WORD_FUNC { BitOp(&); }));
	Install(new Word("|",WORD_FUNC { BitOp(|); }));
	Install(new Word("^",WORD_FUNC { BitOp(^); }));
	Install(new Word(">>",WORD_FUNC { BitShiftOp(>>); }));
	Install(new Word("<<",WORD_FUNC { BitShiftOp(<<); }));

	Install(new Word("@&",WORD_FUNC { RefBitOp(&); }));
	Install(new Word("@|",WORD_FUNC { RefBitOp(|); }));
	Install(new Word("@^",WORD_FUNC { RefBitOp(^); }));
	Install(new Word("@>>",WORD_FUNC { RefBitShiftOp(>>); }));
	Install(new Word("@<<",WORD_FUNC { RefBitShiftOp(<<); }));

	// bitwise NOT.
	Install(new Word("~",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt: 	tos.intValue=~tos.intValue;		break;
			case DataType::kTypeLong:	tos.longValue=~tos.longValue;	break;
			case DataType::kTypeBigInt:	*tos.bigIntPtr=~*tos.bigIntPtr;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos);
		}
		NEXT;
	}));

	Install(new Word("@~",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				inContext.DS.emplace_back(~tos.intValue);
				break;
			case DataType::kTypeLong:
				inContext.DS.emplace_back(~tos.longValue);
				break;
			case DataType::kTypeBigInt:
				inContext.DS.emplace_back(~*tos.bigIntPtr);	
				break;
			default: 
				return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos);
		}
		NEXT;
	}));

	Install(new Word(">",WORD_FUNC { CmpOp(>); }));
	Install(new Word("<",WORD_FUNC { CmpOp(<); }));
	Install(new Word(">=",WORD_FUNC { CmpOp(>=); }));
	Install(new Word("<=",WORD_FUNC { CmpOp(<=); }));
	Install(new Word("==",WORD_FUNC { CmpOp(==); }));
	Install(new Word("!=",WORD_FUNC { CmpOp(!=); }));

	Install(new Word("@>",WORD_FUNC { RefCmpOp(>); }));
	Install(new Word("@<",WORD_FUNC { RefCmpOp(<); }));
	Install(new Word("@>=",WORD_FUNC { RefCmpOp(>=); }));
	Install(new Word("@<=",WORD_FUNC { RefCmpOp(<=); }));
	Install(new Word("@==",WORD_FUNC { RefCmpOp(==); }));
	Install(new Word("@!=",WORD_FUNC { RefCmpOp(!=); }));

	Install(new Word("&&",WORD_FUNC { BoolOp(&&); }));
	Install(new Word("||",WORD_FUNC { BoolOp(||); }));
	Install(new Word("xor",WORD_FUNC { BoolOp(!=); }));
	Install(new Word("not",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeBool) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_BOOL,tos);
		}
		tos.boolValue = tos.boolValue != true;
		NEXT;
	}));
	Install(new Word("not-true?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(!(tos.dataType==DataType::kTypeBool
									&& tos.boolValue==true));
		NEXT;
	}));

	Install(new Word("sqrt",WORD_FUNC { OneArgFloatingFunc(sqrt); }));

	Install(new Word("square",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.intValue*=tos.intValue;
				break;
			case DataType::kTypeLong:
				tos.longValue*=tos.longValue;
				break;
			case DataType::kTypeBigInt:
				*(tos.bigIntPtr) *= *(tos.bigIntPtr);
				break;
			case DataType::kTypeFloat:
				tos.floatValue*=tos.floatValue;
				break;
			case DataType::kTypeDouble:
				tos.doubleValue*=tos.doubleValue;
				break;
			case DataType::kTypeBigFloat:
				*(tos.bigFloatPtr) *= *(tos.bigFloatPtr);
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("exp", WORD_FUNC { OneParamFunc(exp);  }));
	Install(new Word("log", WORD_FUNC { OneParamFunc(log);  }));
	Install(new Word("log10", WORD_FUNC { OneParamFunc(log10);  }));

	Install(new Word("sin",WORD_FUNC { OneParamFunc(sin); }));
	Install(new Word("cos",WORD_FUNC { OneParamFunc(cos); }));
	Install(new Word("tan",WORD_FUNC { OneParamFunc(tan); }));

	Install(new Word("asin",WORD_FUNC { OneParamFunc(asin); }));
	Install(new Word("acos",WORD_FUNC { OneParamFunc(acos); }));
	Install(new Word("atan",WORD_FUNC { OneParamFunc(atan); }));

	Install(new Word("abs",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				inContext.DS.emplace_back(abs(tos.intValue));
				break;
			case DataType::kTypeLong:
				inContext.DS.emplace_back(abs(tos.longValue));
				break;
			case DataType::kTypeBigInt:
				inContext.DS.emplace_back(abs(*(tos.bigIntPtr)));
				break;
			case DataType::kTypeFloat:
				inContext.DS.emplace_back(abs(tos.floatValue));
				break;
			case DataType::kTypeDouble:
				inContext.DS.emplace_back(abs(tos.doubleValue));
				break;
			case DataType::kTypeBigFloat:
				inContext.DS.emplace_back(abs(*(tos.bigIntPtr)));
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("floor",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(floor,tos);
		NEXT;
	}));

	Install(new Word("ceil",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(ceil,tos);
		NEXT;
	}));

	Install(new Word("deg-to-rad",WORD_FUNC { OneParamFunc(deg2rad); }));
	Install(new Word("rad-to-deg",WORD_FUNC { OneParamFunc(rad2deg); }));

	Install(new Word("pow",WORD_FUNC { 
		using namespace boost::multiprecision;

		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}

		TypedValue tos=Pop(inContext.DS);
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType==DataType::kTypeDouble) {
			switch(tos.dataType) {
				case DataType::kTypeInt: /* double x int -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.intValue);
					break;
				case DataType::kTypeLong: /* double x long -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.longValue);
					break;
				case DataType::kTypeFloat: /* double x float -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.floatValue);
					break;
				case DataType::kTypeDouble: /* double x double -> dobule */
					second.doubleValue=pow(second.doubleValue,tos.doubleValue);
					break;
				case DataType::kTypeBigInt: { /* double x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),
									  BigFloat(*tos.bigIntPtr)); 
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				case DataType::kTypeBigFloat: { /* double x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::kTypeBigInt) {
			switch(tos.dataType) {
				case DataType::kTypeInt:	/* bigInt x int -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.intValue);
					break;
				case DataType::kTypeLong:	/* bigInt x long -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.longValue);
					break;
				case DataType::kTypeFloat: {	/* bigInt x float -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.floatValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				case DataType::kTypeDouble: {	/* bigInt x double -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.doubleValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				case DataType::kTypeBigInt: /* bigInt x bigInt -> bigInt */
					return inContext.Error(InvalidTypeTosSecondErrorID::E_OUT_OF_SUPPORT_TOS_SECOND,tos,second);
				case DataType::kTypeBigFloat: { /* bigInt x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),*tos.bigFloatPtr);
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::kTypeBigFloat) {
			switch(tos.dataType) {
				case DataType::kTypeInt: /* bigFloat x int -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.intValue));
					break;
				case DataType::kTypeLong: /* bigFloat x long -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											 BigFloat(tos.longValue));
					break;
				case DataType::kTypeFloat: /* bigFloat x float -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.floatValue));
					break;
				case DataType::kTypeDouble: /* bigFloat x double -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.doubleValue));
					break;
				case DataType::kTypeBigInt: /* bigFloat x bigInt -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(*tos.bigIntPtr));
					break;
				case DataType::kTypeBigFloat: /* bigFloat x bigFloat -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,*tos.bigFloatPtr);
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::kTypeInt) {
			switch(tos.dataType) {
				case DataType::kTypeInt:	/* int x int -> int */
					second.intValue=(int)pow(second.intValue,tos.intValue);
					break;
				case DataType::kTypeLong:	/* int x long -> long */
					second.longValue=(long)pow((long)second.intValue,tos.longValue);
					second.dataType=DataType::kTypeLong;
					break;
				case DataType::kTypeFloat: /* int x float -> float */
					second.floatValue=(float)pow((float)second.intValue,
												 tos.floatValue);
					second.dataType=DataType::kTypeFloat;
					break;
				case DataType::kTypeDouble: /* int x double -> double */
					second.doubleValue=(double)pow((double)second.intValue,
												   tos.doubleValue);
					second.dataType=DataType::kTypeDouble;
					break;
				case DataType::kTypeBigInt: /* int x bigInt -> OutOfSupport */
					return inContext.Error(InvalidTypeTosSecondErrorID::E_OUT_OF_SUPPORT_TOS_SECOND,tos,second);
				case DataType::kTypeBigFloat: { /* int x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.intValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;				
				default: goto onError;
			}
		} else if(second.dataType==DataType::kTypeLong) {
			switch(tos.dataType) {
				case DataType::kTypeInt:	/* long x int -> long */
					second.longValue=(long)pow(second.longValue,(long)tos.intValue);
					break;
				case DataType::kTypeLong:	/* long x long -> long */
					second.longValue=(long)pow(second.longValue,tos.longValue);
					break;
				case DataType::kTypeFloat:	/* long x float -> float */
					second.floatValue=(float)pow((float)second.longValue,
												 tos.floatValue);
					second.dataType=DataType::kTypeFloat;
					break;
				case DataType::kTypeDouble:	/* long x double -> double */
					second.doubleValue=(double)pow((double)second.longValue,
												   tos.doubleValue);
					second.dataType=DataType::kTypeDouble;
					break;
				case DataType::kTypeBigInt: /* long x bigInt -> OutOfSupport */
					return inContext.Error(InvalidTypeTosSecondErrorID::E_OUT_OF_SUPPORT_TOS_SECOND,tos,second);
				case DataType::kTypeBigFloat: { /* long x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.longValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::kTypeFloat) {
			switch(tos.dataType) {
				case DataType::kTypeInt: /* float x int -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.intValue);
					break;
				case DataType::kTypeLong: /* float x long -> float*/
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.longValue);
					break;
				case DataType::kTypeFloat: /* float x float -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.floatValue);
					break;
				case DataType::kTypeDouble: /* float x double -> dobule */
					second.doubleValue=pow((double)second.floatValue,tos.doubleValue);
					second.dataType=DataType::kTypeDouble;
					break;
				case DataType::kTypeBigInt: { /* float x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),
									  BigFloat(*tos.bigIntPtr));
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				case DataType::kTypeBigFloat: { /* float x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else { 
onError: 
			return inContext.Error(InvalidTypeTosSecondErrorID::E_INVALID_DATA_TYPE_TOS_SECOND,tos,second);
		} 
		NEXT;
	}));

	Install(new Word("max",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		PushFuncResult(inContext.DS,maxOp,second,tos);
		NEXT;
	}));

	Install(new Word("min",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		PushFuncResult(inContext.DS,minOp,second,tos);
		NEXT;
	}));

	Install(new Word("0?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.dataType=DataType::kTypeBool;
				tos.boolValue=tos.intValue==0;
				break;
			case DataType::kTypeLong:
				tos.dataType=DataType::kTypeBool;
				tos.boolValue=tos.longValue==0;
				break;
			case DataType::kTypeBigInt: {
					BigInt *biPtr=tos.bigIntPtr;
					tos.dataType=DataType::kTypeBool;
					tos.boolValue=*biPtr==0;
					delete(biPtr);
				}
				break;
			case DataType::kTypeFloat:
				tos.dataType=DataType::kTypeBool;
				tos.boolValue=tos.floatValue==0;
				break;
			case DataType::kTypeDouble:
				tos.dataType=DataType::kTypeBool;
				tos.boolValue=tos.doubleValue==0;
				break;
			case DataType::kTypeBigFloat: {
					BigFloat *bfPtr=tos.bigFloatPtr;
					tos.dataType=DataType::kTypeBool;
					tos.boolValue=*bfPtr==0;
					delete(bfPtr);
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	// equivalent to '1 +'.
	Install(new Word("1+",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		tos.intValue+=1;		break;
			case DataType::kTypeLong:		tos.longValue+=1;		break;
			case DataType::kTypeFloat:		tos.floatValue+=1;		break;
			case DataType::kTypeDouble:		tos.doubleValue+=1;		break;
			case DataType::kTypeBigInt:		*tos.bigIntPtr+=1;		break;
			case DataType::kTypeBigFloat:	*tos.bigFloatPtr+=1;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args | LVOP::INC));

	// equivalent to '1 -'.
	Install(new Word("1-",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		tos.intValue-=1;	break;
			case DataType::kTypeLong:		tos.longValue-=1;	break;
			case DataType::kTypeFloat:		tos.floatValue-=1;	break;
			case DataType::kTypeDouble:		tos.doubleValue-=1;	break;
			case DataType::kTypeBigInt:		*tos.bigIntPtr-=1;	break;
			case DataType::kTypeBigFloat:	*tos.bigFloatPtr-=1;break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args | LVOP::DEC));

	Install(new Word("2/",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		tos.intValue/=2;		break;
			case DataType::kTypeLong:		tos.longValue/=2;		break;
			case DataType::kTypeFloat:		tos.floatValue/=2.0f;	break;
			case DataType::kTypeDouble:		tos.doubleValue/=2.0;	break;
			case DataType::kTypeBigInt:		*tos.bigIntPtr/=2;		break;
			case DataType::kTypeBigFloat:	*tos.bigFloatPtr/=2.0;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("even?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.boolValue = (tos.intValue & 0x01)==0;
				tos.dataType=DataType::kTypeBool;
				break;
			case DataType::kTypeLong:
				tos.boolValue = (tos.longValue & 0x01)==0;
				tos.dataType=DataType::kTypeBool;
				break;
				case DataType::kTypeBigInt: {
					bool result=(*tos.bigIntPtr & 0x01)==0;
					delete tos.bigIntPtr;
					tos.dataType=DataType::kTypeBool;
					tos.boolValue=result;
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos);
		}
		NEXT;
	}));

	Install(new Word("@even?",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		bool result=false;
		switch(tos.dataType) {
			case DataType::kTypeInt: 		result = (tos.intValue & 0x01)==0; 	break;
			case DataType::kTypeLong:		result = (tos.longValue & 0x01)==0; break;
			case DataType::kTypeBigInt: 	result = (*tos.bigIntPtr & 0x01)==0;break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,tos);
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));

	Install(new Word("rand-max",WORD_FUNC {
		inContext.DS.emplace_back(RAND_MAX);
		NEXT;
	}));

	// I ---
	Install(new Word("set-random-seed",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		srand((unsigned int)tos.intValue);
		NEXT;
	}));
	
	// [0,RAND_MAX]
	Install(new Word("rand",WORD_FUNC {
		inContext.DS.emplace_back(rand());
		NEXT;
	}));

	// [0,1]
	Install(new Word("random",WORD_FUNC {
		inContext.DS.emplace_back((float)rand()/(float)RAND_MAX);
		NEXT;
	}));

	Install(new Word("randomize",WORD_FUNC {
		srand((unsigned int)time(NULL));
		NEXT;
	}));

	// n1 --- n2 
	// s.t. n2 is an integer where n2 in [0,n1).
	Install(new Word("rand-to",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}

		const int n=tos.intValue;
		if(n<0 || RAND_MAX<n) {
			return inContext.Error(ErrorIdWithInt::E_TOS_POSITIVE_INT,n);
		}
		int t=(int)(rand()/((float)RAND_MAX+1)*n);
		inContext.DS.emplace_back(t);
		NEXT;
	}));

	Install(new Word("pi",WORD_FUNC {
		inContext.DS.emplace_back(M_PI);
		NEXT;
	}));

	Install(new Word(">int",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType==DataType::kTypeFloat) {
			if(tos.floatValue<INT_MIN || INT_MAX<tos.floatValue) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.floatValue;
		} else if(tos.dataType==DataType::kTypeDouble) {
			if(tos.doubleValue<INT_MIN || INT_MAX<tos.doubleValue) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.doubleValue;
		} else if(tos.dataType==DataType::kTypeLong) {
			if(tos.longValue<(long)INT_MIN || (long)INT_MAX<tos.longValue) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.longValue;
		} else if(tos.dataType==DataType::kTypeBigInt) {
			if(*tos.bigIntPtr<INT_MIN || INT_MAX<*tos.bigIntPtr) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=static_cast<int>(*tos.bigIntPtr);
		} else if(tos.dataType==DataType::kTypeAddress) {
			tos.dataType=DataType::kTypeInt;
		} else if(tos.dataType!=DataType::kTypeInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		tos.dataType=DataType::kTypeInt;
		NEXT;
	}));

	Install(new Word(">long",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType==DataType::kTypeFloat) {
			if(tos.floatValue<LONG_MIN || LONG_MAX<tos.floatValue) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=(long)tos.floatValue;
		} else if(tos.dataType==DataType::kTypeDouble) {
			if(tos.doubleValue<LONG_MIN || LONG_MAX<tos.doubleValue) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=(long)tos.doubleValue;
		} else if(tos.dataType==DataType::kTypeInt) {
			tos.longValue=(long)tos.intValue;
		} else if(tos.dataType==DataType::kTypeBigInt) {
			if(*tos.bigIntPtr<LONG_MIN || LONG_MAX<*tos.bigIntPtr) {
				return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=static_cast<long>(*tos.bigIntPtr);
		} else if(tos.dataType!=DataType::kTypeLong) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		tos.dataType=DataType::kTypeLong;
		NEXT;
	}));

	Install(new Word(">INT",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		BigInt bigInt;
		switch(tos.dataType) {
			case DataType::kTypeInt:
				bigInt=tos.intValue;
				break;
			case DataType::kTypeLong:
				bigInt=tos.longValue;
				break;
			case DataType::kTypeFloat:
				bigInt=static_cast<BigInt>(tos.floatValue);
				break;
			case DataType::kTypeDouble:
				bigInt=static_cast<BigInt>(tos.doubleValue);
				break;
			case DataType::kTypeString:
				bigInt=BigInt(*tos.stringPtr);
				break;
			case DataType::kTypeBigInt:
				// do nothing
				break;
			case DataType::kTypeBigFloat:
				bigInt=static_cast<BigInt>(*tos.bigFloatPtr);
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER_OR_STRING,tos);
		}
		if(tos.dataType!=DataType::kTypeBigInt) {
			inContext.DS.emplace_back(bigInt);
		} else {
			inContext.DS.emplace_back(*tos.bigIntPtr);
		}
		NEXT;
	}));

	Install(new Word(">float",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.floatValue=(float)tos.intValue;
				tos.dataType=DataType::kTypeFloat;
				break;
			case DataType::kTypeLong:
				tos.floatValue=(float)tos.longValue;
				tos.dataType=DataType::kTypeFloat;
				break;
				case DataType::kTypeBigInt: {
					if(*tos.bigIntPtr>kBigInt_FLT_MAX
					  || *tos.bigIntPtr<kBigInt_Minus_FLT_MAX) {
						return inContext.Error(
								NoParamErrorID::E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigIntPtr;
					tos.floatValue=f;
					tos.dataType=DataType::kTypeFloat;
				}
				break;
			case DataType::kTypeFloat:
				// do nothing
				break;
			case DataType::kTypeDouble:
				if(tos.doubleValue>FLT_MAX || tos.doubleValue<-FLT_MAX) {
					return inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
				}
				tos.floatValue=(float)tos.doubleValue;
				tos.dataType=DataType::kTypeFloat;
				break;
			case DataType::kTypeBigFloat: {
					if(*tos.bigFloatPtr>FLT_MAX || *tos.bigFloatPtr<-FLT_MAX) {
						return inContext.Error(
								NoParamErrorID::E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigFloatPtr;
					tos.floatValue=f;
					tos.dataType=DataType::kTypeFloat;
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word(">double",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.doubleValue=(double)tos.intValue;
				tos.dataType=DataType::kTypeDouble;
				break;
			case DataType::kTypeLong:
				tos.doubleValue=(double)tos.longValue;
				tos.dataType=DataType::kTypeDouble;
				break;
			case DataType::kTypeBigInt: {
					if(*tos.bigIntPtr>kBigInt_DBL_MAX
					  || *tos.bigIntPtr<kBigInt_Minus_DBL_MAX) {
						return inContext.Error(
								NoParamErrorID::E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigIntPtr;
					tos.doubleValue=t;
					tos.dataType=DataType::kTypeDouble;
				}
				break;
			case DataType::kTypeFloat:
				tos.doubleValue=(double)tos.floatValue;
				tos.dataType=DataType::kTypeDouble;
				break;
			case DataType::kTypeDouble:
				// do nothing
				break;
			case DataType::kTypeBigFloat: {
					if(*tos.bigFloatPtr>DBL_MAX || *tos.bigFloatPtr<-DBL_MAX) {
						return inContext.Error(
								NoParamErrorID::E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigFloatPtr;
					tos.doubleValue=t;
					tos.dataType=DataType::kTypeDouble;
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word(">FLOAT",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tos=Pop(inContext.DS);
		BigFloat bigFloat;
		switch(tos.dataType) {
			case DataType::kTypeInt:
				bigFloat=tos.intValue;
				break;
			case DataType::kTypeLong:
				bigFloat=tos.longValue;
				break;
			case DataType::kTypeFloat:  
				bigFloat=tos.floatValue;
				break;
			case DataType::kTypeDouble:
				bigFloat=tos.doubleValue;
				break;
			case DataType::kTypeString:
				bigFloat=BigFloat(*tos.stringPtr);
				break;
			case DataType::kTypeBigInt:
				bigFloat=static_cast<BigFloat>(*tos.bigIntPtr);
				break;
			case DataType::kTypeBigFloat:
			   	/* do nothing */
				break;
			default:
	  			return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER_OR_STRING,tos);
		}
		if(tos.dataType!=DataType::kTypeBigFloat) {
			inContext.DS.emplace_back(bigFloat);
		} else {
			inContext.DS.emplace_back(*tos.bigFloatPtr);
		}
		NEXT;
	}));

	Install(new Word(">address",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::kTypeInt && tos.dataType!=DataType::kTypeAddress) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT,tos);
		}
		tos.dataType=DataType::kTypeAddress;
		NEXT;
	}));
}

