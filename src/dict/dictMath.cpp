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

static void doBoolConstant(Context& inContext,bool inBool) {
	if( inContext.IsInComment() ) { return; }
	if(inContext.ExecutionThreshold==Level::Interpret) {
		inContext.DS.emplace_back(inBool);
	} else if(inContext.newWord->type==WordType::List) {
		inContext.Compile(inBool);
	} else {
		inContext.Compile(std::string("_lit"));
		inContext.Compile(inBool);
	}
}

void InitDict_Math() {
	Install(new Word("true",WordLevel::Level2,WORD_FUNC {
		doBoolConstant(inContext,true);
		NEXT;
	}));

	Install(new Word("false",WordLevel::Level2,WORD_FUNC {
		doBoolConstant(inContext,false);
		NEXT;
	}));

	Install(new Word("+",WORD_FUNC { TwoOp(+); },LVOP::LVOpSupported2args| LVOP::ADD));
	Install(new Word("-",WORD_FUNC { TwoOp(-); },LVOP::LVOpSupported2args| LVOP::SUB));
	Install(new Word("*",WORD_FUNC { TwoOp(*); },LVOP::LVOpSupported2args| LVOP::MUL));
	Install(new Word("/",WORD_FUNC { TwoOp(/); },LVOP::LVOpSupported2args| LVOP::DIV));
	Install(new Word("%",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		CheckIntegerAndNonZero(tos);
		TypedValue& second=ReadTOS(inContext.DS);
		ModAssign(second,tos); 	// second %= tos;
		NEXT;
	}));


	Install(new Word("2*",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:			tos.intValue	   *= 2;	break;
			case DataType::Long:		tos.longValue	   *= 2L;	break;
			case DataType::BigInt:		*(tos.bigIntPtr)   *= 2;	break;
			case DataType::Float:		tos.floatValue	   *= 2.0f;	break;
			case DataType::Double:		tos.doubleValue	   *= 2.0;	break;
			case DataType::BigFloat:	*(tos.bigFloatPtr) *= 2;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args | LVOP::TWC));

	Install(new Word("@+",WORD_FUNC { RefTwoOp(inContext.DS,+); }));
	Install(new Word("@-",WORD_FUNC { RefTwoOp(inContext.DS,-); }));
	Install(new Word("@*",WORD_FUNC { RefTwoOp(inContext.DS,*); }));
	Install(new Word("@/",WORD_FUNC { RefTwoOp(inContext.DS,/); }));
	Install(new Word("@%",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
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
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int: 	tos.intValue=~tos.intValue;		break;
			case DataType::Long:	tos.longValue=~tos.longValue;	break;
			case DataType::BigInt:	*tos.bigIntPtr=~*tos.bigIntPtr;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
		}
		NEXT;
	}));

	Install(new Word("@~",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:
				inContext.DS.emplace_back(~tos.intValue);
				break;
			case DataType::Long:
				inContext.DS.emplace_back(~tos.longValue);
				break;
			case DataType::BigInt:
				inContext.DS.emplace_back(~*tos.bigIntPtr);	
				break;
			default: 
				return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		if(tos.dataType!=DataType::Bool) {
			return inContext.Error(InvalidTypeErrorID::TosBool,tos);
		}
		tos.boolValue = tos.boolValue != true;
		NEXT;
	}));
	Install(new Word("not-true?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS.emplace_back(!(tos.dataType==DataType::Bool
									&& tos.boolValue==true));
		NEXT;
	}));

	Install(new Word("sqrt",WORD_FUNC { OneArgFloatingFunc(sqrt); }));
	Install(new Word("@sqrt",WORD_FUNC { RefOneArgFloatingFunc(sqrt); }));

	Install(new Word("square",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int: 	 tos.intValue*=tos.intValue; 		  	   break;
			case DataType::Long:   	 tos.longValue*=tos.longValue; 		  	   break;
			case DataType::BigInt: 	 *(tos.bigIntPtr) *= *(tos.bigIntPtr); 	   break;
			case DataType::Float: 	 tos.floatValue*=tos.floatValue; 	  	   break;
			case DataType::Double: 	 tos.doubleValue*=tos.doubleValue; 		   break;
			case DataType::BigFloat: *(tos.bigFloatPtr) *= *(tos.bigFloatPtr); break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	}));

	Install(new Word("round",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:
			case DataType::Long:
			case DataType::BigInt:
				break;
			case DataType::Float:
				tos.floatValue = (float)(tos.floatValue>=0 ? (int)(tos.floatValue+0.5f)
														   : (int)(tos.floatValue-0.5f));
				break;
			case DataType::Double:
				tos.doubleValue = tos.doubleValue>=0 ? (int)(tos.doubleValue+0.5)
													 : (int)(tos.doubleValue-0.5);
				break;
			case DataType::BigFloat:
				*(tos.bigFloatPtr)=boost::multiprecision::round(*(tos.bigFloatPtr));
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:
				inContext.DS.emplace_back(abs(tos.intValue));
				break;
			case DataType::Long:
				inContext.DS.emplace_back(abs(tos.longValue));
				break;
			case DataType::BigInt:
				inContext.DS.emplace_back(abs(*(tos.bigIntPtr)));
				break;
			case DataType::Float:
				inContext.DS.emplace_back(abs(tos.floatValue));
				break;
			case DataType::Double:
				inContext.DS.emplace_back(abs(tos.doubleValue));
				break;
			case DataType::BigFloat:
				inContext.DS.emplace_back(abs(*(tos.bigIntPtr)));
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	}));

	Install(new Word("floor",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(floor,tos);
		NEXT;
	}));

	Install(new Word("ceil",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		FloorOrCeil(ceil,tos);
		NEXT;
	}));

	Install(new Word("deg-to-rad",WORD_FUNC { OneParamFunc(deg2rad); }));
	Install(new Word("rad-to-deg",WORD_FUNC { OneParamFunc(rad2deg); }));

	Install(new Word("pow",WORD_FUNC { 
		using namespace boost::multiprecision;

		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}

		TypedValue tos=Pop(inContext.DS);
		TypedValue& second=ReadTOS(inContext.DS);
		if(second.dataType==DataType::Double) {
			switch(tos.dataType) {
				case DataType::Int: /* double x int -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.intValue);
					break;
				case DataType::Long: /* double x long -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.longValue);
					break;
				case DataType::Float: /* double x float -> double */
					second.doubleValue=pow(second.doubleValue,(double)tos.floatValue);
					break;
				case DataType::Double: /* double x double -> dobule */
					second.doubleValue=pow(second.doubleValue,tos.doubleValue);
					break;
				case DataType::BigInt: { /* double x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),
									  BigFloat(*tos.bigIntPtr)); 
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				case DataType::BigFloat: { /* double x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.doubleValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::BigInt) {
			switch(tos.dataType) {
				case DataType::Int:	/* bigInt x int -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.intValue);
					break;
				case DataType::Long:	/* bigInt x long -> bigInt */
					*second.bigIntPtr=pow(*second.bigIntPtr,tos.longValue);
					break;
				case DataType::Float: {	/* bigInt x float -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.floatValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				case DataType::Double: {	/* bigInt x double -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),
									  BigFloat(tos.doubleValue));
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				case DataType::BigInt: /* bigInt x bigInt -> bigInt */
					return inContext.Error(InvalidTypeTosSecondErrorID::OutOfSupportTosSecond,tos,second);
				case DataType::BigFloat: { /* bigInt x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(*second.bigIntPtr),*tos.bigFloatPtr);
						delete(second.bigIntPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::BigFloat) {
			switch(tos.dataType) {
				case DataType::Int: /* bigFloat x int -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.intValue));
					break;
				case DataType::Long: /* bigFloat x long -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											 BigFloat(tos.longValue));
					break;
				case DataType::Float: /* bigFloat x float -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.floatValue));
					break;
				case DataType::Double: /* bigFloat x double -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(tos.doubleValue));
					break;
				case DataType::BigInt: /* bigFloat x bigInt -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,
											BigFloat(*tos.bigIntPtr));
					break;
				case DataType::BigFloat: /* bigFloat x bigFloat -> bigFloat */
					*second.bigFloatPtr=pow(*second.bigFloatPtr,*tos.bigFloatPtr);
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::Int) {
			switch(tos.dataType) {
				case DataType::Int:	/* int x int -> int */
					second.intValue=(int)pow(second.intValue,tos.intValue);
					break;
				case DataType::Long:	/* int x long -> long */
					second.longValue=(long)pow((long)second.intValue,tos.longValue);
					second.dataType=DataType::Long;
					break;
				case DataType::Float: /* int x float -> float */
					second.floatValue=(float)pow((float)second.intValue,
												 tos.floatValue);
					second.dataType=DataType::Float;
					break;
				case DataType::Double: /* int x double -> double */
					second.doubleValue=(double)pow((double)second.intValue,
												   tos.doubleValue);
					second.dataType=DataType::Double;
					break;
				case DataType::BigInt: /* int x bigInt -> OutOfSupport */
					return inContext.Error(InvalidTypeTosSecondErrorID::OutOfSupportTosSecond,tos,second);
				case DataType::BigFloat: { /* int x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.intValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;				
				default: goto onError;
			}
		} else if(second.dataType==DataType::Long) {
			switch(tos.dataType) {
				case DataType::Int:	/* long x int -> long */
					second.longValue=(long)pow(second.longValue,(long)tos.intValue);
					break;
				case DataType::Long:	/* long x long -> long */
					second.longValue=(long)pow(second.longValue,tos.longValue);
					break;
				case DataType::Float:	/* long x float -> float */
					second.floatValue=(float)pow((float)second.longValue,
												 tos.floatValue);
					second.dataType=DataType::Float;
					break;
				case DataType::Double:	/* long x double -> double */
					second.doubleValue=(double)pow((double)second.longValue,
												   tos.doubleValue);
					second.dataType=DataType::Double;
					break;
				case DataType::BigInt: /* long x bigInt -> OutOfSupport */
					return inContext.Error(InvalidTypeTosSecondErrorID::OutOfSupportTosSecond,tos,second);
				case DataType::BigFloat: { /* long x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.longValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				default: goto onError;
			}
		} else if(second.dataType==DataType::Float) {
			switch(tos.dataType) {
				case DataType::Int: /* float x int -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.intValue);
					break;
				case DataType::Long: /* float x long -> float*/
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.longValue);
					break;
				case DataType::Float: /* float x float -> float */
					second.floatValue=(float)pow((double)second.floatValue,
												 (double)tos.floatValue);
					break;
				case DataType::Double: /* float x double -> dobule */
					second.doubleValue=pow((double)second.floatValue,tos.doubleValue);
					second.dataType=DataType::Double;
					break;
				case DataType::BigInt: { /* float x bigInt -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),
									  BigFloat(*tos.bigIntPtr));
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				case DataType::BigFloat: { /* float x bigFloat -> bigFloat */
						BigFloat *bigFloat=new BigFloat();
						*bigFloat=pow(BigFloat(second.floatValue),*tos.bigFloatPtr);
						second.bigFloatPtr=bigFloat;
						second.dataType=DataType::BigFloat;
					}
					break;
				default: goto onError;
			}
		} else { 
onError: 
			return inContext.Error(InvalidTypeTosSecondErrorID::BothDataInvalid,
								   tos,second);
		} 
		NEXT;
	}));

	Install(new Word("max",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		PushFuncResult(inContext.DS,maxOp,second,tos);
		NEXT;
	}));

	Install(new Word("min",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::DsAtLeast2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		PushFuncResult(inContext.DS,minOp,second,tos);
		NEXT;
	}));

	// equivalent to '0 =='
	Install(new Word("0?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:
				tos.dataType=DataType::Bool;
				tos.boolValue=tos.intValue==0;
				break;
			case DataType::Long:
				tos.dataType=DataType::Bool;
				tos.boolValue=tos.longValue==0;
				break;
			case DataType::BigInt: {
					BigInt *biPtr=tos.bigIntPtr;
					tos.dataType=DataType::Bool;
					tos.boolValue=*biPtr==0;
					delete(biPtr);
				}
				break;
			case DataType::Float:
				tos.dataType=DataType::Bool;
				tos.boolValue=tos.floatValue==0;
				break;
			case DataType::Double:
				tos.dataType=DataType::Bool;
				tos.boolValue=tos.doubleValue==0;
				break;
			case DataType::BigFloat: {
					BigFloat *bfPtr=tos.bigFloatPtr;
					tos.dataType=DataType::Bool;
					tos.boolValue=*bfPtr==0;
					delete(bfPtr);
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosNumber,tos);
		}
		NEXT;
	}));

	Install(new Word("even?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::Int:
				tos.boolValue = (tos.intValue & 0x01)==0;
				tos.dataType=DataType::Bool;
				break;
			case DataType::Long:
				tos.boolValue = (tos.longValue & 0x01)==0;
				tos.dataType=DataType::Bool;
				break;
				case DataType::BigInt: {
					bool result=(*tos.bigIntPtr & 0x01)==0;
					delete tos.bigIntPtr;
					tos.dataType=DataType::Bool;
					tos.boolValue=result;
				}
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
		}
		NEXT;
	}));

	Install(new Word("@even?",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue& tos=ReadTOS(inContext.DS);
		bool result=false;
		switch(tos.dataType) {
			case DataType::Int: 	result = (tos.intValue & 0x01)==0; 		break;
			case DataType::Long:	result = (tos.longValue & 0x01)==0; 	break;
			case DataType::BigInt: 	result = (*tos.bigIntPtr & 0x01)==0;	break;
			default:
				return inContext.Error(InvalidTypeErrorID::TosIntOrLongOrBigint,tos);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }
		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
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
		if(inContext.DS.size()<1) { return inContext.Error(NoParamErrorID::DsIsEmpty); }

		TypedValue tos=Pop(inContext.DS);
		if(tos.dataType!=DataType::Int) {
			return inContext.Error(InvalidTypeErrorID::TosInt,tos);
		}

		const int n=tos.intValue;
		if(n<0 || RAND_MAX<n) {
			return inContext.Error(ErrorIdWithInt::TosPositiveInt,n);
		}
		int t=(int)(rand()/((float)RAND_MAX+1)*n);
		inContext.DS.emplace_back(t);
		NEXT;
	}));

	Install(new Word("pi",WORD_FUNC {
		inContext.DS.emplace_back(M_PI);
		NEXT;
	}));
}

