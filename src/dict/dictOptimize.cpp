#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"
#include "mathMacro.h"

void InitDict_Optimize() {
	Install(new Word("_dup_i",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		inContext.DS.emplace_back(inContext.DS.back());
		inContext.DS.emplace_back(inContext.RS.back());
		NEXT;
	}));

	// equivalent to "+ >r"
	Install(new Word("+>r",WORD_FUNC {
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue tos=Pop(inContext.DS);
		TypedValue second=Pop(inContext.DS);
		AssignOp(inContext,second,+,tos);
		inContext.RS.emplace_back(second);
		NEXT;
	}));
	
	// equivalent to "@r> %".
	Install(new Word("_r>%",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=inContext.RS.back();
		if(rsTos.dataType!=kTypeInt
		  && rsTos.dataType!=kTypeLong
		  && rsTos.dataType!=kTypeBigInt) {
			return inContext.Error(E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}
		
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& dsTos=inContext.DS.back();
		if(dsTos.dataType!=kTypeInt
		  && dsTos.dataType!=kTypeLong
		  && dsTos.dataType!=kTypeBigInt) {
			return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
		}
		ModAssign(dsTos,rsTos);
		NEXT;
	}));

	// equivalent to "dup i %".
	Install(new Word("_dup_@r>%",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		inContext.DS.emplace_back(inContext.DS.back()); // dup

		TypedValue& dsTos=ReadTOS(inContext.DS);
		if(dsTos.dataType!=kTypeInt
		  && dsTos.dataType!=kTypeLong
		  && dsTos.dataType!=kTypeBigInt) {
			return inContext.Error(E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
		}

		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=kTypeInt
		  && rsTos.dataType!=kTypeLong
		  && rsTos.dataType!=kTypeBigInt) {
			return inContext.Error(E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}
		
		ModAssign(dsTos,rsTos);
		NEXT;
	}));

	// equivalent to "@r> +".
	Install(new Word("_r>+",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=inContext.RS.back();
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& dsTos=inContext.DS.back();
		AssignOp(inContext,dsTos,+,rsTos);
		NEXT;
	}));

	// equivalent to "swap _r>+ swap".
	Install(new Word("_r>+_second",WORD_FUNC {
		const char *name="_r>+_second";
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=inContext.RS.back();
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue& dsSecond=inContext.DS[inContext.DS.size()-2];
		AssignOp(inContext,dsSecond,+,rsTos);
		NEXT;
	}));

	// equivalent to "_0 _setValue" ( "0 _setValue" )
	// X ---
	Install(new Word(">$0",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(0,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d0));	
	Install(new Word(">$1",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(1,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d1));	
	Install(new Word(">$2",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(2,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d2));	
	Install(new Word(">$3",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(3,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d3));	
	Install(new Word(">$4",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(4,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d4));	
	Install(new Word(">$5",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(5,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d5));	
	Install(new Word(">$6",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(6,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d6));	
	Install(new Word(">$7",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(7,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d7));	
	Install(new Word(">$8",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(8,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d8));	
	Install(new Word(">$9",WORD_FUNC {
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue tv=Pop(inContext.DS);
		inContext.SetCurrentLocalVar(9,tv);
		NEXT;
	},LVOP::TLV | LVOP::sDS | LVOP::d9));	

	// equivalent to "_0 _getValue" ("0 _getValue")
	// -- X
	Install(new Word("$0>",WORD_FUNC {
		const int slotNo=0;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s0 | LVOP::dDS));
	Install(new Word("$1>",WORD_FUNC {
		const int slotNo=1;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s1 | LVOP::dDS));
	Install(new Word("$2>",WORD_FUNC {
		const int slotNo=2;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s2 | LVOP::dDS));
	Install(new Word("$3>",WORD_FUNC {
		const int slotNo=3;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s3 | LVOP::dDS));
	Install(new Word("$4>",WORD_FUNC {
		const int slotNo=4;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s4 | LVOP::dDS));
	Install(new Word("$5>",WORD_FUNC {
		const int slotNo=5;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s5 | LVOP::dDS));
	Install(new Word("$6>",WORD_FUNC {
		const int slotNo=6;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s6 | LVOP::dDS));
	Install(new Word("$7>",WORD_FUNC {
		const int slotNo=7;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s7 | LVOP::dDS));
	Install(new Word("$8>",WORD_FUNC {
		const int slotNo=8;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s8 | LVOP::dDS));
	Install(new Word("$9>",WORD_FUNC {
		const int slotNo=9;
		if(inContext.GetLocalVarValue(slotNo,inContext.DS)==false) {
			return inContext.Error(E_INVALID_LOCAL_SLOT_INDEX,slotNo);
		}
		NEXT;
	},LVOP::TST | LVOP::s9 | LVOP::dDS));

	// --- X
	// equivalent to "2 pick" ("_2 pick")
	Install(new Word("2pick",WORD_FUNC {
		if(inContext.DS.size()<3) { return inContext.Error(E_DS_AT_LEAST_3); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-3]);
		NEXT;
	}));
	Install(new Word("3pick",WORD_FUNC {
		if(inContext.DS.size()<4) { return inContext.Error(E_DS_AT_LEAST_4); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-4]);
		NEXT;
	}));
	Install(new Word("4pick",WORD_FUNC {
		if(inContext.DS.size()<5) { return inContext.Error(E_DS_AT_LEAST_5); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-5]);
		NEXT;
	}));
	Install(new Word("5pick",WORD_FUNC {
		if(inContext.DS.size()<6) { return inContext.Error(E_DS_AT_LEAST_6); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-6]);
		NEXT;
	}));
	Install(new Word("6pick",WORD_FUNC {
		if(inContext.DS.size()<7) { return inContext.Error(E_DS_AT_LEAST_7); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-7]);
		NEXT;
	}));
	Install(new Word("7pick",WORD_FUNC {
		if(inContext.DS.size()<8) { return inContext.Error(E_DS_AT_LEAST_8); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-8]);
		NEXT;
	}));
	Install(new Word("8pick",WORD_FUNC {
		if(inContext.DS.size()<9) { return inContext.Error(E_DS_AT_LEAST_9); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-9]);
		NEXT;
	}));
	Install(new Word("9pick",WORD_FUNC {
		if(inContext.DS.size()<10) { return inContext.Error(E_DS_AT_LEAST_10); }
		const int size=(int)inContext.DS.size();
		inContext.DS.emplace_back(inContext.DS[size-10]);
		NEXT;
	}));
						
	// X ---
	// equivalent to "1 replace" ("_1 replace").
	Install(new Word("1replace",WORD_FUNC {
		if(inContext.DS.size()<3) { return inContext.Error(E_DS_AT_LEAST_3); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-2]=tos;
		NEXT;
	}));
	Install(new Word("2replace",WORD_FUNC {
		if(inContext.DS.size()<4) { return inContext.Error(E_DS_AT_LEAST_4); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-3]=tos;
		NEXT;
	}));
	Install(new Word("3replace",WORD_FUNC {
		if(inContext.DS.size()<5) { return inContext.Error(E_DS_AT_LEAST_5); }
		TypedValue tos=Pop(inContext.DS);
		inContext.DS[inContext.DS.size()-4]=tos;
		NEXT;
	}));
	Install(new Word("4replace",WORD_FUNC {
		if(inContext.DS.size()<6) { return inContext.Error(E_DS_AT_LEAST_6); }
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
			case kTypeInt:		tos.intValue	   *= tos.intValue;			break;
			case kTypeLong:		tos.longValue	   *= tos.longValue;		break;
			case kTypeBigInt:	*(tos.bigIntPtr)   *= *(tos.bigIntPtr);		break;
			case kTypeFloat:	tos.floatValue	   *= tos.floatValue;		break;
			case kTypeDouble:	tos.doubleValue	   *= tos.doubleValue;		break;
			case kTypeBigFloat:	*(tos.bigFloatPtr) *= *(tos.bigFloatPtr);	break;
			default: 			return inContext.Error(E_TOS_NUMBER,tos);
		}
		NEXT;
	},LVOP::LVOpSupported1args|LVOP::PW2));
}

