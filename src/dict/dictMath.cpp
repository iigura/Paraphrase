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

void InitDict_Math() {
	Install(new Word("true",WORD_FUNC {
		inContext.DS.emplace_back(true);
		NEXT;
	}));

	Install(new Word("false",WORD_FUNC {
		inContext.DS.emplace_back(false);
		NEXT;
	}));

	Install(new Word("+",WORD_FUNC { TwoOp(+); }));
	Install(new Word("-",WORD_FUNC { TwoOp(-); }));

	Install(new Word("*",WORD_FUNC { TwoOp(*); }));
	Install(new Word("/",WORD_FUNC { TwoOp(/); }));

	Install(new Word("%",WORD_FUNC {
		Stack& ds=inContext.DS;
		if(ds.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(ds);
		TypedValue& second=ds.back();

		// second %= tos;
		ModAssign(second,tos);
		NEXT;
   }));

	Install(new Word("&",WORD_FUNC { BitOp(&); }));
	Install(new Word("|",WORD_FUNC { BitOp(|); }));
	Install(new Word("^",WORD_FUNC { BitOp(^); }));
	Install(new Word(">>",WORD_FUNC { BitShiftOp(>>); }));
	Install(new Word("<<",WORD_FUNC { BitShiftOp(<<); }));

	// bitwise NOT.
	Install(new Word("~",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt: 		tos.intValue=~tos.intValue;		break;
			case kTypeLong:		tos.longValue=~tos.longValue;	break;
			case kTypeBigInt:	*tos.bigIntPtr=~*tos.bigIntPtr;	break;
			default:
				return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT,tos);
		}
		NEXT;
	}));

	Install(new Word(">",WORD_FUNC { CmpOp(>); }));
	Install(new Word("<",WORD_FUNC { CmpOp(<); }));
	
	Install(new Word(">=",WORD_FUNC { CmpOp(>=); }));
	Install(new Word("<=",WORD_FUNC { CmpOp(<=); }));

	Install(new Word("==",WORD_FUNC { CmpOp(==); }));
	Install(new Word("!=",WORD_FUNC { CmpOp(!=); }));

	Install(new Word("and",WORD_FUNC { BoolOp(&&); }));
	Install(new Word("or",WORD_FUNC { BoolOp(||); }));
	Install(new Word("xor",WORD_FUNC { BoolOp(!=); }));
	Install(new Word("not",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeBool) {
			return inContext.Error_InvalidType(E_TOS_BOOL,tos);
		}
		tos.boolValue = tos.boolValue != true;
		NEXT;
	}));

	Install(new Word("&&",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeBool && tos.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_BOOL_OR_WP,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=kTypeBool && second.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_SECOND_BOOL_OR_WP,second);
		}

		if(tos.dataType==kTypeBool) {
			if(tos.boolValue==false) {
				inContext.DS.emplace_back(false);
				goto next;
			}
		} else {
			if(inContext.Exec(tos)==false) { return false; }
			tos=Pop(inContext.DS);
			if(tos.dataType!=kTypeBool) {
				return inContext.Error(E_WP_AT_TOS_SHOULD_PUSH_A_BOOL);
			}
			if(tos.boolValue==false) {
				inContext.DS.emplace_back(false);
				goto next;
			}
		}

		if(second.dataType==kTypeBool) {
			if(second.boolValue==false) {
				inContext.DS.emplace_back(false);
				goto next;
			}
		} else {
			if(inContext.Exec(second)==false) {
				return false;
			}
			second=Pop(inContext.DS);
			if(second.dataType!=kTypeBool) {
				return inContext.Error(E_WP_AT_SECOND_SHOULD_PUSH_A_BOOL);
			}
			if(second.boolValue==false) {
				inContext.DS.emplace_back(false);
				goto next;
			}
		}
		inContext.DS.emplace_back(true);
next:
		NEXT;
	}));
	Install(new Word("||",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeBool && tos.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_TOS_BOOL_OR_WP,tos);
		}

		TypedValue second=Pop(inContext.DS);
		if(second.dataType!=kTypeBool && second.dataType!=kTypeWord) {
			return inContext.Error_InvalidType(E_SECOND_BOOL_OR_WP,tos);
		}

		if(tos.dataType==kTypeBool) {
			if(tos.boolValue==true) {
				inContext.DS.emplace_back(true);
				goto next;
			}
		} else {
			if(inContext.Exec(tos)==false) { return false; }
			tos=Pop(inContext.DS);
			if(tos.dataType!=kTypeBool) {
				return inContext.Error(E_WP_AT_TOS_SHOULD_PUSH_A_BOOL);
			}
			if(tos.boolValue==true) {
				inContext.DS.emplace_back(true);
				goto next;
			}
		}

		if(second.dataType==kTypeBool) {
			if(second.boolValue==true) {
				inContext.DS.emplace_back(true);
				goto next;
			}
		} else {
			if(inContext.Exec(second)==false) {
				return false;
			}
			second=Pop(inContext.DS);
			if(second.dataType!=kTypeBool) {
				return inContext.Error(E_WP_AT_SECOND_SHOULD_PUSH_A_BOOL);
			}
			if(second.boolValue==true) {
				inContext.DS.emplace_back(true);
				goto next;
			}
		}
		inContext.DS.emplace_back(false);
next:
		NEXT;
	}));

	Install(new Word("sqrt",WORD_FUNC { OneArgFloatingFunc(sqrt); }));
	Install(new Word("exp", WORD_FUNC { OneParamFunc(exp);  }));
	Install(new Word("log", WORD_FUNC { OneParamFunc(log);  }));
	Install(new Word("log10", WORD_FUNC { OneParamFunc(log10);  }));

	Install(new Word("sin",WORD_FUNC { OneParamFunc(sin); }));
	Install(new Word("cos",WORD_FUNC { OneParamFunc(cos); }));
	Install(new Word("tan",WORD_FUNC { OneParamFunc(tan); }));

	Install(new Word("asin",WORD_FUNC { OneParamFunc(asin); }));
	Install(new Word("acos",WORD_FUNC { OneParamFunc(acos); }));
	Install(new Word("atan",WORD_FUNC { OneParamFunc(atan); }));

	Install(new Word("floor",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(floor,tos);
		NEXT;
	}));

	Install(new Word("ceil",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(ceil,tos);
		NEXT;
	}));

	Install(new Word("deg-to-rad",WORD_FUNC { OneParamFunc(deg2rad); }));
	Install(new Word("rad-to-deg",WORD_FUNC { OneParamFunc(rad2deg); }));

	Install(new Word("pow",WORD_FUNC { 
		using namespace boost::multiprecision;

		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }

		TypedValue tos=Pop(inContext.DS);
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType==kTypeDouble) {
			switch(tos.dataType) {
				case kTypeInt: /* double x int -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.intValue);
					break;
				case kTypeLong: /* double x long -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.longValue);
					break;
				case kTypeFloat: /* double x float -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.floatValue);
					break;
				case kTypeDouble: /* double x double -> dobule */
					second.doubleValue=pow(second.doubleValue,tos.doubleValue);
					break;
				case kTypeBigInt: { /* double x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),
									  BigFloat(*tos.bigIntPtr)); 
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				case kTypeBigFloat: { /* double x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==kTypeBigInt) {
			switch(tos.dataType) {
				case kTypeInt:	/* bigInt x int -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.intValue);
					break;
				case kTypeLong:	/* bigInt x long -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.longValue);
					break;
				case kTypeFloat: {	/* bigInt x float -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.floatValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				case kTypeDouble: {	/* bigInt x double -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.doubleValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				case kTypeBigInt: /* bigInt x bigInt -> bigInt */
					return inContext.Error_InvalidType(E_OUT_OF_SUPPORT_TOS_SECOND,
													   tos,second);
				case kTypeBigFloat: { /* bigInt x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),*tos.bigFloatPtr);
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==kTypeBigFloat) {
			switch(tos.dataType) {
				case kTypeInt: /* bigFloat x int -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.intValue));
					break;
				case kTypeLong: /* bigFloat x long -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											 BigFloat(tos.longValue));
					break;
				case kTypeFloat: /* bigFloat x float -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.floatValue));
					break;
				case kTypeDouble: /* bigFloat x double -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.doubleValue));
					break;
				case kTypeBigInt: /* bigFloat x bigInt -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(*tos.bigIntPtr));
					break;
				case kTypeBigFloat: /* bigFloat x bigFloat -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,*tos.bigFloatPtr);
					break;
				default: goto onError;
			}
		} else if(second.dataType==kTypeInt) {
			switch(tos.dataType) {
				case kTypeInt:	/* int x int -> int */
					second.intValue=(int)pow(second.intValue,tos.intValue);
					break;
				case kTypeLong:	/* int x long -> long */
					second.longValue=(long)pow((long)second.intValue,tos.longValue);
					second.dataType=kTypeLong;
					break;
				case kTypeFloat: /* int x float -> float */
					second.floatValue=(float)pow((float)second.intValue,
												 tos.floatValue);
					second.dataType=kTypeFloat;
					break;
				case kTypeDouble: /* int x double -> double */
					second.doubleValue=(double)pow((double)second.intValue,
												   tos.doubleValue);
					second.dataType=kTypeDouble;
					break;
				case kTypeBigInt: /* int x bigInt -> OutOfSupport */
					return inContext.Error_InvalidType(E_OUT_OF_SUPPORT_TOS_SECOND,
													   tos,second);
				case kTypeBigFloat: { /* int x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.intValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;				
				default: goto onError;
			}
		} else if(second.dataType==kTypeLong) {
			switch(tos.dataType) {
				case kTypeInt:	/* long x int -> long */
					second.longValue=(long)pow(second.longValue,(long)tos.intValue);
					break;
				case kTypeLong:	/* long x long -> long */
					second.longValue=(long)pow(second.longValue,tos.longValue);
					break;
				case kTypeFloat:	/* long x float -> float */
					second.floatValue=(float)pow((float)second.longValue,
												 tos.floatValue);
					second.dataType=kTypeFloat;
					break;
				case kTypeDouble:	/* long x double -> double */
					second.doubleValue=(double)pow((double)second.longValue,
												   tos.doubleValue);
					second.dataType=kTypeDouble;
					break;
				case kTypeBigInt: /* long x bigInt -> OutOfSupport */
					return inContext.Error_InvalidType(E_OUT_OF_SUPPORT_TOS_SECOND,
													   tos,second);
				case kTypeBigFloat: { /* long x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.longValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==kTypeFloat) {
			switch(tos.dataType) {
				case kTypeInt: /* float x int -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.intValue);
					break;
				case kTypeLong: /* float x long -> float*/
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.longValue);
					break;
				case kTypeFloat: /* float x float -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.floatValue);
					break;
				case kTypeDouble: /* float x double -> dobule */
					second.doubleValue=pow((double)second.floatValue,tos.doubleValue);
					second.dataType=kTypeDouble;
					break;
				case kTypeBigInt: { /* float x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),
									  BigFloat(*tos.bigIntPtr));
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				case kTypeBigFloat: { /* float x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=kTypeBigFloat;
					}
					break;
				default: goto onError;
			}
		} else { 
onError: 
			return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_TOS_SECOND,
											   tos,second);
		} 
		NEXT;
	}));

	Install(new Word("0?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:
				tos.dataType=kTypeBool;
				tos.boolValue=tos.intValue==0;
				break;
			case kTypeLong:
				tos.dataType=kTypeBool;
				tos.boolValue=tos.longValue==0;
				break;
			case kTypeBigInt: {
					BigInt *biPtr=tos.bigIntPtr;
					tos.dataType=kTypeBool;
					tos.boolValue=*biPtr==0;
					delete(biPtr);
				}
				break;
			case kTypeFloat:
				tos.dataType=kTypeBool;
				tos.boolValue=tos.floatValue==0;
				break;
			case kTypeDouble:
				tos.dataType=kTypeBool;
				tos.boolValue=tos.doubleValue==0;
				break;
			case kTypeBigFloat: {
					BigFloat *bfPtr=tos.bigFloatPtr;
					tos.dataType=kTypeBool;
					tos.boolValue=*bfPtr==0;
					delete(bfPtr);
				}
				break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	// equivalent to '1 +'.
	Install(new Word("1+",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:		tos.intValue+=1;		break;
			case kTypeFloat:	tos.floatValue+=1;		break;
			case kTypeDouble:	tos.doubleValue+=1;		break;
			case kTypeBigInt:	*tos.bigIntPtr+=1;		break;
			case kTypeBigFloat:	*tos.bigFloatPtr+=1;	break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	// equivalent to '1 -'.
	Install(new Word("1-",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:		tos.intValue-=1;	break;
			case kTypeFloat:	tos.floatValue-=1;	break;
			case kTypeDouble:	tos.doubleValue-=1;	break;
			case kTypeBigInt:	*tos.bigIntPtr-=1;	break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("2/",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:		tos.intValue/=2;		break;
			case kTypeFloat:	tos.floatValue/=2.0f;	break;
			case kTypeDouble:	tos.doubleValue/=2.0;	break;
			case kTypeBigInt:	*tos.bigIntPtr/=2;		break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("rand-max",WORD_FUNC {
		inContext.DS.emplace_back(RAND_MAX);
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

	// 整数 --- 数値
	// n --- t 
	// s.t. t is an integer and t in [0,n).
	Install(new Word("rand-to",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
		}

		const int n=tos.intValue;
		if(n<0 || RAND_MAX<n) { return inContext.Error(E_TOS_POSITIVE_INT,n); }
		int t=(int)(rand()/((float)RAND_MAX+1)*n);
		inContext.DS.emplace_back(t);
		NEXT;
	}));

	Install(new Word("pi",WORD_FUNC {
		inContext.DS.emplace_back(M_PI);
		NEXT;
	}));

	Install(new Word(">int",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }

		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType==kTypeFloat) {
			if(tos.floatValue<INT_MIN || INT_MAX<tos.floatValue) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.floatValue;
		} else if(tos.dataType==kTypeDouble) {
			if(tos.doubleValue<INT_MIN || INT_MAX<tos.doubleValue) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.doubleValue;
		} else if(tos.dataType==kTypeLong) {
			if(tos.longValue<(long)INT_MIN || (long)INT_MAX<tos.longValue) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=(int)tos.longValue;
		} else if(tos.dataType==kTypeBigInt) {
			if(*tos.bigIntPtr<INT_MIN || INT_MAX<*tos.bigIntPtr) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW);
			}
			tos.intValue=static_cast<int>(*tos.bigIntPtr);
		} else if(tos.dataType==kTypeAddress) {
			tos.dataType=kTypeInt;
		} else if(tos.dataType!=kTypeInt) {
			return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		tos.dataType=kTypeInt;
		NEXT;
	}));

	Install(new Word(">long",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType==kTypeFloat) {
			if(tos.floatValue<LONG_MIN || LONG_MAX<tos.floatValue) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=(long)tos.floatValue;
		} else if(tos.dataType==kTypeDouble) {
			if(tos.doubleValue<LONG_MIN || LONG_MAX<tos.doubleValue) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=(long)tos.doubleValue;
		} else if(tos.dataType==kTypeInt) {
			tos.longValue=(long)tos.intValue;
		} else if(tos.dataType==kTypeBigInt) {
			if(*tos.bigIntPtr<LONG_MIN || LONG_MAX<*tos.bigIntPtr) {
				return inContext.Error(E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW);
			}
			tos.longValue=static_cast<long>(*tos.bigIntPtr);
		} else if(tos.dataType!=kTypeLong) {
			return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		tos.dataType=kTypeLong;
		NEXT;
	}));

	Install(new Word(">INT",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		BigInt bigInt;
		switch(tos.dataType) {
			case kTypeInt:	  bigInt=tos.intValue;		break;
			case kTypeLong:	  bigInt=tos.longValue;		break;
			case kTypeFloat:  bigInt=static_cast<BigInt>(tos.floatValue);	break;
			case kTypeDouble: bigInt=static_cast<BigInt>(tos.doubleValue);	break;
			case kTypeString: bigInt=BigInt(*tos.stringPtr); break;
			case kTypeBigInt:
				// do nothing
				break;
			case kTypeBigFloat: bigInt=static_cast<BigInt>(*tos.bigFloatPtr); break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER_OR_STRING,tos);
		}
		if(tos.dataType!=kTypeBigInt) {
			inContext.DS.emplace_back(bigInt);
		} else {
			inContext.DS.emplace_back(*tos.bigIntPtr);
		}
		NEXT;
	}));

	Install(new Word(">float",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:
				tos.floatValue=(float)tos.intValue;
				tos.dataType=kTypeFloat;
				break;
			case kTypeLong:
				tos.floatValue=(float)tos.longValue;
				tos.dataType=kTypeFloat;
				break;
			case kTypeBigInt: {
					if(*tos.bigIntPtr>kBigInt_FLT_MAX
					  || *tos.bigIntPtr<kBigInt_Minus_FLT_MAX) {
						return inContext.Error(
								E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigIntPtr;
					tos.floatValue=f;
					tos.dataType=kTypeFloat;
				}
				break;
			case kTypeFloat:
				// do nothing
				break;
			case kTypeDouble:
				if(tos.doubleValue>FLT_MAX || tos.doubleValue<-FLT_MAX) {
					return inContext.Error(E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
				}
				tos.floatValue=(float)tos.doubleValue;
				tos.dataType=kTypeFloat;
				break;
			case kTypeBigFloat: {
					if(*tos.bigFloatPtr>FLT_MAX || *tos.bigFloatPtr<-FLT_MAX) {
						return inContext.Error(
								E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
					}
					float f=tos.ToFloat(inContext);
					delete tos.bigFloatPtr;
					tos.floatValue=f;
					tos.dataType=kTypeFloat;
				}
				break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word(">double",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case kTypeInt:
				tos.doubleValue=(double)tos.intValue;
				tos.dataType=kTypeDouble;
				break;
			case kTypeLong:
				tos.doubleValue=(double)tos.longValue;
				tos.dataType=kTypeDouble;
				break;
			case kTypeBigInt: {
					if(*tos.bigIntPtr>kBigInt_DBL_MAX
					  || *tos.bigIntPtr<kBigInt_Minus_DBL_MAX) {
						return inContext.Error(
								E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigIntPtr;
					tos.doubleValue=t;
					tos.dataType=kTypeDouble;
				}
				break;
			case kTypeFloat:
				tos.doubleValue=(double)tos.floatValue;
				tos.dataType=kTypeDouble;
				break;
			case kTypeDouble:
				// do nothing
				break;
			case kTypeBigFloat: {
					if(*tos.bigFloatPtr>DBL_MAX || *tos.bigFloatPtr<-DBL_MAX) {
						return inContext.Error(
								E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
					}
					double t=tos.ToDouble(inContext);
					delete tos.bigFloatPtr;
					tos.doubleValue=t;
					tos.dataType=kTypeDouble;
				}
				break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word(">FLOAT",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tos=Pop(inContext.DS);
		BigFloat bigFloat;
		switch(tos.dataType) {
			case kTypeInt:	  bigFloat=tos.intValue;	break;
			case kTypeLong:	  bigFloat=tos.longValue;	break;
			case kTypeFloat:  bigFloat=tos.floatValue;	break;
			case kTypeDouble: bigFloat=tos.doubleValue;	break;
			case kTypeString: bigFloat=BigFloat(*tos.stringPtr); break;
			case kTypeBigInt: bigFloat=static_cast<BigFloat>(*tos.bigIntPtr); break;
			case kTypeBigFloat: /* do nothing */ 		break;
			default:
				return inContext.Error_InvalidType(E_TOS_NUMBER_OR_STRING,tos);
		}
		if(tos.dataType!=kTypeBigFloat) {
			inContext.DS.emplace_back(bigFloat);
		} else {
			inContext.DS.emplace_back(*tos.bigFloatPtr);
		}
		NEXT;
	}));

	Install(new Word("to-address",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=kTypeInt && tos.dataType!=kTypeAddress) {
			return inContext.Error_InvalidType(E_TOS_INT,tos);
		}
		tos.dataType=kTypeAddress;
		NEXT;
	}));
}

