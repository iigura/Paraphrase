#pragma once

// a += b
// AssignOp(a,+,b)
#define AssignOp(inTarget,OP,inRef) do { \
	if(inTarget.dataType==kTypeInt && inRef.dataType==kTypeInt) { \
		/* int x int -> int */ \
		inTarget.intValue=inTarget.intValue OP inRef.intValue; \
	} else if(inTarget.dataType==kTypeLong && inRef.dataType==kTypeLong) { \
		/* long x long -> long */ \
		inTarget.longValue=inTarget.longValue OP inRef.longValue; \
	} else if(inTarget.dataType==kTypeFloat && inRef.dataType==kTypeFloat) { \
		/* float x float -> float */ \
		inTarget.floatValue=inTarget.floatValue OP inRef.floatValue; \
	} else if(inTarget.dataType==kTypeDouble && inRef.dataType==kTypeDouble) { \
		/* double x double -> double */ \
		inTarget.doubleValue=inTarget.doubleValue OP inRef.doubleValue; \
	} else if(inTarget.dataType==kTypeBigInt && inRef.dataType==kTypeBigInt) { \
		/* bigInt x bigInt -> bigInt */ \
		*inTarget.bigIntPtr = *inTarget.bigIntPtr OP *inRef.bigIntPtr; \
	} else if(inTarget.dataType==kTypeBigFloat && inRef.dataType==kTypeBigFloat) { \
		/* bigFloat x bigFloat -> bigFloat */ \
		*inTarget.bigFloatPtr = *inTarget.bigFloatPtr OP *inRef.bigFloatPtr; \
	} else if(inTarget.dataType==kTypeInt) { \
		switch(inRef.dataType) { \
			case kTypeLong:		/* int x long -> long */ \
				inTarget.longValue=(long)inTarget.intValue OP inRef.longValue; \
				inTarget.dataType=kTypeLong; \
				break; \
			case kTypeFloat:	/* int x float -> float */ \
				inTarget.floatValue=(float)inTarget.intValue OP inRef.floatValue; \
				inTarget.dataType=kTypeFloat; \
				break; \
			case kTypeDouble:	/* int x double -> double */ \
				inTarget.doubleValue=(double)inTarget.intValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kTypeBigInt: {	/* int x BigInt -> BigInt */ \
					inTarget.bigIntPtr \
						=new BigInt(inTarget.intValue OP *inRef.bigIntPtr); \
					inTarget.dataType=kTypeBigInt; \
				} \
				break; \
			case kTypeBigFloat: { /* int x BigFloat -> BigFloat */ \
					inTarget.bigFloatPtr \
						=new BigFloat(inTarget.intValue OP *inRef.bigFloatPtr); \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if(inTarget.dataType==kTypeLong) { \
		switch(inRef.dataType) { \
			case kTypeInt:		/* long x int -> long */ \
				inTarget.longValue=inTarget.longValue OP (long)inRef.intValue; \
				break; \
			case kTypeFloat:	/* long x float -> float */ \
				inTarget.floatValue=(float)inTarget.longValue OP inRef.floatValue; \
				inTarget.dataType=kTypeFloat; \
				break; \
			case kTypeDouble:	/* long x double -> double */ \
				inTarget.doubleValue=(double)inTarget.longValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kTypeBigInt: {	/* long x bigInt -> bigInt */ \
					inTarget.bigIntPtr \
						=new BigInt(inTarget.longValue OP *inRef.bigIntPtr); \
					inTarget.dataType=kTypeBigInt; \
				} \
				break; \
			case kTypeBigFloat:	{ /* long x bigFloat -> bigFloat */ \
					inTarget.bigFloatPtr \
						=new BigFloat(inTarget.longValue OP *inRef.bigFloatPtr); \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if(inTarget.dataType==kTypeFloat) { \
		switch(inRef.dataType) { \
			case kTypeInt:		/* float x int -> float */ \
				inTarget.floatValue=inTarget.floatValue OP (float)inRef.intValue; \
				break; \
			case kTypeLong:		/* float x long -> float */ \
				inTarget.floatValue=inTarget.floatValue OP (float)inRef.longValue; \
				break; \
			case kTypeDouble:	/* float x double -> double */ \
				inTarget.doubleValue=(double)inTarget.floatValue OP inRef.doubleValue; \
				inTarget.dataType=kTypeDouble; \
				break; \
			case kTypeBigInt: {	/* float x bigInt -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(*inRef.bigIntPtr); \
					*bigFloat=inTarget.floatValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigFloat:	/* float x bigFloat -> bigFloat */ \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.floatValue OP *inRef.bigFloatPtr); \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			default: goto onError;	\
		} \
	} else if(inTarget.dataType==kTypeDouble) { \
		switch(inRef.dataType) { \
			case kTypeInt:		/* double x int -> double */ \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.intValue; \
				break; \
			case kTypeLong:		/* double x long -> double */ \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.longValue; \
				break; \
			case kTypeFloat:	/* double x float -> double */ \
				inTarget.doubleValue=inTarget.doubleValue OP (double)inRef.floatValue; \
				break; \
			case kTypeBigInt: {	/* double x bigInt -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(*inRef.bigIntPtr); \
					*bigFloat=inTarget.doubleValue OP *bigFloat; \
					inTarget.bigFloatPtr=bigFloat; \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigFloat:	/* double x bigFloat -> bigFloat */ \
				inTarget.bigFloatPtr \
					=new BigFloat(inTarget.doubleValue OP *inRef.bigFloatPtr); \
				inTarget.dataType=kTypeBigFloat; \
				break; \
			default: goto onError;	\
		} \
	} else if(inTarget.dataType==kTypeBigInt) { \
		switch(inRef.dataType) { \
			case kTypeInt:		/* bigInt x int -> bigInt */ \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inRef.intValue; \
				break; \
			case kTypeLong: 	/* bigInt x long -> bigInt */ \
				*inTarget.bigIntPtr = *inTarget.bigIntPtr OP inRef.longValue; \
				break; \
			case kTypeFloat: { /* bigInt x float -> bigFloat */ \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inRef.floatValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeDouble: { /* bigInt x double -> bigInt */ \
					BigFloat *bigFloat=new BigFloat(*inTarget.bigIntPtr); \
					*bigFloat=*bigFloat OP inRef.doubleValue; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			case kTypeBigFloat: { /* bigInt x bigFloat -> bigFlaot */ \
					BigFloat *bigFloat=new BigFloat(); \
					*bigFloat=BigFloat(*inTarget.bigIntPtr) OP *inRef.bigFloatPtr; \
					delete(inTarget.bigIntPtr); \
					inTarget.bigFloatPtr=bigFloat; \
					inTarget.dataType=kTypeBigFloat; \
				} \
				break; \
			default: goto onError;	\
		} \
	} else if(inTarget.dataType==kTypeBigFloat) { \
		switch(inRef.dataType) { \
			case kTypeInt: 		/* bigFloat x int -> bigFloat */ \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inRef.intValue); \
				break; \
			case kTypeLong: 	/* bigFloat x long -> bigFloat */ \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inRef.longValue); \
				break; \
			case kTypeFloat:	/* bigFloat x float -> bigFloat */ \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inRef.floatValue); \
				break; \
			case kTypeDouble:	/* bigFloat x double -> bigFloat */ \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(inRef.doubleValue); \
				break; \
			case kTypeBigInt:	/* bigFloat x bigInt -> bigFloat */ \
				*inTarget.bigFloatPtr \
					=*inTarget.bigFloatPtr OP BigFloat(*inRef.bigIntPtr); \
				break; \
			default: goto onError;	\
		} \
	} else if(#OP=="+" \
			 && inTarget.dataType==kTypeString && inRef.dataType==kTypeString) { \
		inTarget=TypedValue(*inTarget.stringPtr+*inRef.stringPtr); \
	} else { \
onError: \
		return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_FOR_OP, \
										   std::string(#OP),inTarget,inRef); \
	} \
} while(0)

#define TwoOp(OP) do { \
	if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); } \
	TypedValue tos=Pop(inContext.DS); \
	TypedValue& second=ReadTOS(inContext.DS); \
	AssignOp(second,OP,tos); \
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
	} else if(tos.dataType==kTypeDouble || second.dataType==kTypeDouble) { \
		double t=tos.ToDouble(inContext); \
		double s=second.ToDouble(inContext); \
		second.dataType=kTypeBool; \
		second.boolValue=s OP t; \
	} else if(tos.dataType==kTypeFloat || second.dataType==kTypeFloat) { \
		float t=tos.ToFloat(inContext); \
		float s=second.ToFloat(inContext); \
		second.dataType=kTypeBool; \
		second.boolValue=s OP t; \
	} else if(tos.dataType==kTypeString && second.dataType==kTypeString) { \
		second.dataType=kTypeBool; \
		second.boolValue=*second.stringPtr OP *tos.stringPtr; \
	} else if(tos.dataType==kTypeBigInt && second.dataType==kTypeBigInt) { \
		BigInt *s=second.bigIntPtr; \
		second.dataType=kTypeBool; \
		second.boolValue=(*s) OP (*tos.bigIntPtr); \
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
						second.boolValue=(double)second.intValue OP tos.doubleValue; \
						break; \
					case kTypeBigFloat: { \
							BigFloat bf(second.intValue); \
							second.dataType=kTypeBool; \
							second.boolValue=bf OP *tos.bigFloatPtr; \
						} \
						break; \
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
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
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
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
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
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
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
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
					default: \
						return inContext.Error(E_SYSTEM_ERROR); \
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
			default: \
				return inContext.Error(E_SYSTEM_ERROR); \
		} \
	} \
	NEXT; \
} while(0)

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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(E_SECOND_INT_OR_LONG_OR_BIGINT, \
											   second); \
	} \
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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
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
					return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT, \
													   tos); \
			} \
			break; \
		default: \
			return inContext.Error_InvalidType(E_SECOND_INT_OR_LONG_OR_BIGINT, \
											   second); \
	} \
	NEXT; \
} while(0)

#define BoolOp(OP) do { \
	if(inContext.DS.size()<2) { \
		return inContext.Error(E_DS_AT_LEAST_2); \
	} \
	TypedValue tos=Pop(inContext.DS); \
	if(tos.dataType!=kTypeBool) { \
		return inContext.Error_InvalidType(E_TOS_BOOL,tos); \
	} \
	TypedValue& second=ReadTOS(inContext.DS); \
	if(second.dataType!=kTypeBool) { \
		return inContext.Error_InvalidType(E_SECOND_BOOL,second); \
	} \
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
		default: \
			return inContext.Error_InvalidType(E_TOS_DOUBLABLE,tos); \
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
		return inContext.Error_InvalidType(E_INVALID_DATA_TYPE_TOS_SECOND, \
								   		   tos,second); \
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
			return inContext.Error_InvalidType(E_TOS_NUMBER,tos); \
	} \
	NEXT; \
} while(0)

// a %= b
// ModAssign(a,b);
#define ModAssign(inTarget,inRef) do { \
	if(inRef.dataType!=kTypeInt \
	  && inRef.dataType!=kTypeLong \
	  && inRef.dataType!=kTypeBigInt) { \
		return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT,inRef); \
	} \
	if(inTarget.dataType!=kTypeInt \
	  && inTarget.dataType!=kTypeLong \
	  && inTarget.dataType!=kTypeBigInt) { \
		return inContext.Error_InvalidType(E_SECOND_INT_OR_LONG_OR_BIGINT,inTarget); \
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
			return inContext.Error_InvalidType(E_TOS_NUMBER,ioTV); \
	} \
} while(0)
