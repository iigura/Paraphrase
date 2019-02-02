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
	
	// equivalent to "@r> %".
	Install(new Word("_r>%",WORD_FUNC {
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=inContext.RS.back();
		if(rsTos.dataType!=kTypeInt
		  && rsTos.dataType!=kTypeLong
		  && rsTos.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
		}
		
		if(inContext.DS.size()<1) { return inContext.Error(E_DS_IS_EMPTY); }
		TypedValue& dsTos=inContext.DS.back();
		if(dsTos.dataType!=kTypeInt
		  && dsTos.dataType!=kTypeLong
		  && dsTos.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
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
			return inContext.Error_InvalidType(E_TOS_INT_OR_LONG_OR_BIGINT,dsTos);
		}

		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=ReadTOS(inContext.RS);
		if(rsTos.dataType!=kTypeInt
		  && rsTos.dataType!=kTypeLong
		  && rsTos.dataType!=kTypeBigInt) {
			return inContext.Error_InvalidType(E_RS_TOS_INT_OR_LONG_OR_BIGINT,rsTos);
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
		AssignOp(dsTos,+,rsTos);
		NEXT;
	}));

	// equivalent to "swap _r>+ swap".
	Install(new Word("_r>+_second",WORD_FUNC {
		const char *name="_r>+_second";
		if(inContext.RS.size()<1) { return inContext.Error(E_RS_IS_EMPTY); }
		TypedValue& rsTos=inContext.RS.back();
		if(inContext.DS.size()<2) { return inContext.Error(E_DS_AT_LEAST_2); }
		TypedValue& dsSecond=inContext.DS[inContext.DS.size()-2];
		AssignOp(dsSecond,+,rsTos);
		NEXT;
	}));

	// equivalent to "_lit 1"
	Install(new Word("_1",WORD_FUNC {
		inContext.DS.emplace_back(1);
		NEXT;
	}));
	
	// equivalent to "_lit 2"
	Install(new Word("_2",WORD_FUNC {
		inContext.DS.emplace_back(2);
		NEXT;
	}));
}

