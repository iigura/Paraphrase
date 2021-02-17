#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

void InitDict_Optimize() {
	Install(new Word("_dup_i",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		inContext.DS.emplace_back(inContext.DS.back());
		inContext.DS.emplace_back(inContext.RS.back());
		NEXT;
	}));

	// equivalent to "+ >r"
	Install(new Word("+>r",WORD_FUNC {
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		AssignOpToSecond(inContext,second,+,tos);
		inContext.RS.emplace_back(second);
		NEXT;
	}));
	
	// equivalent to "@r> %".
	Install(new Word("_r>%",WORD_FUNC {
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& rsTos=inContext.RS.back();
		if(rsTos.dataType!=DataType::kTypeInt
		  && rsTos.dataType!=DataType::kTypeLong
		  && rsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}
		
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& dsTos=inContext.DS.back();
		if(dsTos.dataType!=DataType::kTypeInt
		  && dsTos.dataType!=DataType::kTypeLong
		  && dsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
		}
		ModAssign(dsTos,rsTos);
		NEXT;
	}));

	// equivalent to "dup i %".
	Install(new Word("_dup_@r>%",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		inContext.DS.emplace_back(inContext.DS.back()); // dup

		TypedValue& dsTos=ReadTOS(inContext.DS);
		if(dsTos.dataType!=DataType::kTypeInt
		  && dsTos.dataType!=DataType::kTypeLong
		  && dsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
		}

		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=DataType::kTypeInt
		  && rsTos.dataType!=DataType::kTypeLong
		  && rsTos.dataType!=DataType::kTypeBigInt) {
			return inContext.Error(InvalidTypeErrorID::E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}
		
		ModAssign(dsTos,rsTos);
		NEXT;
	}));

	// equivalent to "@r> +".
	Install(new Word("_r>+",WORD_FUNC {
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& rsTos=inContext.RS.back();
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& dsTos=inContext.DS.back();
		AssignOpToSecond(inContext,dsTos,+,rsTos);
		NEXT;
	}));

	// equivalent to "swap _r>+ swap".
	Install(new Word("_r>+_second",WORD_FUNC {
		const char *name="_r>+_second";
		if(inContext.RS.size()<1) {
			return inContext.Error(NoParamErrorID::E_RS_IS_EMPTY);
		}
		TypedValue& rsTos=inContext.RS.back();
		if(inContext.DS.size()<2) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_2);
		}
		TypedValue& dsSecond=inContext.DS[inContext.DS.size()-2];
		AssignOpToSecond(inContext,dsSecond,+,rsTos);
		NEXT;
	}));

	// equivalent to "_0 _setValue" ( "0 _setValue" )
	// X ---
	Install(new Word(">$0",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(0,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d0));	
	Install(new Word(">$1",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(1,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d1));	
	Install(new Word(">$2",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(2,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d2));	
	Install(new Word(">$3",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(3,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d3));	
	Install(new Word(">$4",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(4,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d4));	
	Install(new Word(">$5",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(5,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d5));	
	Install(new Word(">$6",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(6,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d6));	
	Install(new Word(">$7",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(7,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d7));	
	Install(new Word(">$8",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(8,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d8));	
	Install(new Word(">$9",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(9,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d9));	

	// equivalent to "_0 _getValue" ("0 _getValue")
	// -- X
	Install(new Word("$0>",WORD_FUNC {
		const int slotNo=0;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s0 | LVOP::dDS));
	Install(new Word("$1>",WORD_FUNC {
		const int slotNo=1;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s1 | LVOP::dDS));
	Install(new Word("$2>",WORD_FUNC {
		const int slotNo=2;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s2 | LVOP::dDS));
	Install(new Word("$3>",WORD_FUNC {
		const int slotNo=3;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s3 | LVOP::dDS));
	Install(new Word("$4>",WORD_FUNC {
		const int slotNo=4;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s4 | LVOP::dDS));
	Install(new Word("$5>",WORD_FUNC {
		const int slotNo=5;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s5 | LVOP::dDS));
	Install(new Word("$6>",WORD_FUNC {
		const int slotNo=6;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s6 | LVOP::dDS));
	Install(new Word("$7>",WORD_FUNC {
		const int slotNo=7;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s7 | LVOP::dDS));
	Install(new Word("$8>",WORD_FUNC {
		const int slotNo=8;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s8 | LVOP::dDS));
	Install(new Word("$9>",WORD_FUNC {
		const int slotNo=9;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(ErrorIdWithInt::E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s9 | LVOP::dDS));

	// a b c --- a b c a
	// equivalent to "2 pick" ("_2 pick")
	Install(new Word("2pick",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_3);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-3]);
		NEXT;
	}));
	Install(new Word("3pick",WORD_FUNC {
		if(inContext.DS.size()<4) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_4);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-4]);
		NEXT;
	}));
	Install(new Word("4pick",WORD_FUNC {
		if(inContext.DS.size()<5) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_5);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-5]);
		NEXT;
	}));
	Install(new Word("5pick",WORD_FUNC {
		if(inContext.DS.size()<6) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_6);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-6]);
		NEXT;
	}));
	Install(new Word("6pick",WORD_FUNC {
		if(inContext.DS.size()<7) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_7);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-7]);
		NEXT;
	}));
	Install(new Word("7pick",WORD_FUNC {
		if(inContext.DS.size()<8) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_8);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-8]);
		NEXT;
	}));
	Install(new Word("8pick",WORD_FUNC {
		if(inContext.DS.size()<9) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_9);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-9]);
		NEXT;
	}));
	Install(new Word("9pick",WORD_FUNC {
		if(inContext.DS.size()<10) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_10);
		}
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-10]);
		NEXT;
	}));
						
	// a b X --- X b
	// equivalent to "1 replace" ("_1 replace").
	Install(new Word("1replace",WORD_FUNC {
		if(inContext.DS.size()<3) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_3);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-2]=tos;
		NEXT;
	}));
	// a b c X --- X b c
	Install(new Word("2replace",WORD_FUNC {
		if(inContext.DS.size()<4) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_4);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-3]=tos;
		NEXT;
	}));
	Install(new Word("3replace",WORD_FUNC {
		if(inContext.DS.size()<5) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_5);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-4]=tos;
		NEXT;
	}));
	Install(new Word("4replace",WORD_FUNC {
		if(inContext.DS.size()<6) {
			return inContext.Error(NoParamErrorID::E_DS_AT_LEAST_6);
		}
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-5]=tos;
		NEXT;
	}));
				
	// equivalent to "_lit 1"
	Install(new Word("_0",WORD_FUNC { inContext.DS.emplace_back(0); NEXT; }));
	Install(new Word("_1",WORD_FUNC { inContext.DS.emplace_back(1); NEXT; }));
	Install(new Word("_2",WORD_FUNC { inContext.DS.emplace_back(2); NEXT; }));
	Install(new Word("_3",WORD_FUNC { inContext.DS.emplace_back(3); NEXT; }));
	Install(new Word("_4",WORD_FUNC { inContext.DS.emplace_back(4); NEXT; }));
	Install(new Word("_5",WORD_FUNC { inContext.DS.emplace_back(5); NEXT; }));
	Install(new Word("_6",WORD_FUNC { inContext.DS.emplace_back(6); NEXT; }));
	Install(new Word("_7",WORD_FUNC { inContext.DS.emplace_back(7); NEXT; }));
	Install(new Word("_8",WORD_FUNC { inContext.DS.emplace_back(8); NEXT; }));
	Install(new Word("_9",WORD_FUNC { inContext.DS.emplace_back(9); NEXT; }));

	// equivalent to "dup *"
	Install(new Word("_square",WORD_FUNC {
		// no needed stack check
		// if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:
				tos.intValue *= tos.intValue;
				break;
			case DataType::kTypeLong:
				tos.longValue *= tos.longValue;
				break;
			case DataType::kTypeBigInt:
				*(tos.bigIntPtr) *= *(tos.bigIntPtr);
				break;
			case DataType::kTypeFloat:
				tos.floatValue *= tos.floatValue;
				break;
			case DataType::kTypeDouble:
				tos.doubleValue	*= tos.doubleValue;
				break;
			case DataType::kTypeBigFloat:
				*(tos.bigFloatPtr) *= *(tos.bigFloatPtr);
				break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args|LVOP::PW2));

	Install(new Word("_inc",WORD_FUNC {
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		++tos.intValue;			break;
			case DataType::kTypeLong:		++tos.longValue;		break;
			case DataType::kTypeBigInt:		++*(tos.bigIntPtr);		break;
			case DataType::kTypeFloat:		++tos.floatValue;		break;
			case DataType::kTypeDouble:		++tos.doubleValue;		break;
			case DataType::kTypeBigFloat:	++*(tos.bigFloatPtr);	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));
	Install(new Word("_dec",WORD_FUNC {
		TypedValue& tos=ReadTOS(inContext.DS);
		switch(tos.dataType) {
			case DataType::kTypeInt:		--tos.intValue;			break;
			case DataType::kTypeLong:		--tos.longValue;		break;
			case DataType::kTypeBigInt:		--*(tos.bigIntPtr);		break;
			case DataType::kTypeFloat:		--tos.floatValue;		break;
			case DataType::kTypeDouble:		--tos.doubleValue;		break;
			case DataType::kTypeBigFloat:	--*(tos.bigFloatPtr);	break;
			default:
				return inContext.Error(InvalidTypeErrorID::E_TOS_NUMBER,tos);
		}
		NEXT;
	}));

	Install(new Word("_int*", WORD_FUNC     { IntTwoOp(*); }));
	Install(new Word("_long*",WORD_FUNC     { LongTwoOp(*); }));
	Install(new Word("_float*",WORD_FUNC    { FloatTwoOp(*); }));
	Install(new Word("_double*",WORD_FUNC   { DoubleTwoOp(*); }));
	Install(new Word("_bigInt*",WORD_FUNC   { BigIntTwoOp(*); }));
	Install(new Word("_bigFloat*",WORD_FUNC { BigFloatTwoOp(*); }));

	// equivalent to "dup 1 !="
	Install(new Word("_@1!=",WORD_FUNC {
		if(inContext.DS.size()<1) {
			return inContext.Error(NoParamErrorID::E_DS_IS_EMPTY);
		}
		TypedValue& tos=ReadTOS(inContext.DS);
		bool result=false;
		switch(tos.dataType) {
			case DataType::kTypeInt:		result=tos.intValue!=1;			break;
			case DataType::kTypeLong:		result=tos.longValue!=1;		break;
			case DataType::kTypeFloat:		result=tos.floatValue!=1.0f;	break;
			case DataType::kTypeDouble:		result=tos.doubleValue!=1.0;	break;
			case DataType::kTypeBigInt:		result=*(tos.bigIntPtr)!=1;		break;
			case DataType::kTypeBigFloat:	result=*(tos.bigFloatPtr)!=1.0;	break;
			default:
				;	// empty
		}
		inContext.DS.emplace_back(result);
		NEXT;
	}));
}

