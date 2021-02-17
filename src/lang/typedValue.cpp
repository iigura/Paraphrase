#include <boost/format.hpp>

#include "word.h"
#include "typedValue.h"
#include "stack.h"
#include "context.h"

const BigInt gBI_DBL_MAX(DBL_MAX);
const BigInt gBI_FLT_MAX(FLT_MAX);

const BigFloat gBF_DBL_MAX(DBL_MAX);
const BigFloat gBF_FLT_MAX(FLT_MAX);

static std::string disAsmLVOP(int inLVOP);
static std::string controlBlockNameStr(const ControlBlockType inCBT);

PP_API double TypedValue::ToDouble(Context& inContext) {
	switch(dataType) {
		case DataType::kTypeInt:	return (double)intValue;
		case DataType::kTypeLong:	return (double)longValue;
		case DataType::kTypeFloat:	return (double)floatValue;
		case DataType::kTypeDouble:	return doubleValue;
		case DataType::kTypeBigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_DBL_MAX) {
				return static_cast<double>(bigInt);
			} else {
				inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
			}
		}
		case DataType::kTypeBigFloat: {
			const BigFloat& bigFloat=*bigFloatPtr;
			if(abs(bigFloat)<gBF_DBL_MAX) {
				return static_cast<double>(bigFloat);
			} else {
				inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
			}
		}
		default:
			inContext.Error(InvalidTypeErrorID::E_TOS_CAN_NOT_CONVERT_TO_DOUBLE,*this);
	}
	return std::numeric_limits<double>::quiet_NaN();
}

PP_API float TypedValue::ToFloat(Context& inContext) {
	switch(dataType) {
		case DataType::kTypeInt:	return (float)intValue;
		case DataType::kTypeLong:	return (float)longValue;
		case DataType::kTypeFloat:	return floatValue;
		case DataType::kTypeDouble:	return (float)doubleValue;
		case DataType::kTypeBigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_FLT_MAX) {
				return static_cast<float>(*bigIntPtr);
			} else {
				inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
			}
		}
		case DataType::kTypeBigFloat: {
			const BigFloat& bigFloat=*bigFloatPtr;
			if(abs(bigFloat)<=gBF_FLT_MAX) {
				return static_cast<float>(*bigFloatPtr);
			} else {
				inContext.Error(NoParamErrorID::E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
			}
		}
		default:
			inContext.Error(InvalidTypeErrorID::E_TOS_CAN_NOT_CONVERT_TO_FLOAT,*this);
	}
	return std::numeric_limits<float>::quiet_NaN();
}

PP_API TypedValue FullClone(TypedValue& inTV) {
	switch(inTV.dataType) {
		case DataType::kTypeString:
		case DataType::kTypeSymbol:
			return TypedValue(*inTV.stringPtr);
		case DataType::kTypeArray: {
				Array<TypedValue> *srcPtr=inTV.arrayPtr.get();
				Lock(srcPtr->mutex);
					const int n=srcPtr->length;
					TypedValue *dataBody=new TypedValue[n];
					for(int i=0; i<n; i++) {
						dataBody[i]=FullClone(srcPtr->data[i]);
					}
				Unlock(srcPtr->mutex);
				Array<TypedValue> *arrayPtr=new Array<TypedValue>(n,dataBody,true);
				return TypedValue(arrayPtr);
			}
		case DataType::kTypeList: {
				std::deque<TypedValue> *srcPtr=inTV.listPtr.get();
				std::deque<TypedValue> *destPtr=new std::deque<TypedValue>();
				const int n=(int)srcPtr->size();
				for(int i=0; i<n; i++) {
					destPtr->emplace_back(FullClone(srcPtr->at(i)));
				}
				return TypedValue(destPtr);
			}
		case DataType::kTypeKV: {
				KeyValue *srcPtr=inTV.kvPtr.get();
				KeyValue *dstPtr=new KeyValue();
				*dstPtr=*srcPtr;
				return TypedValue(dstPtr);
			}
		default:
			return TypedValue(inTV);
	}
}

PP_API std::string TypedValue::GetTypeStr() const {
	switch(dataType) {
		case DataType::kTypeInvalid:	return std::string("(INVALID)");
		case DataType::kTypeEOC:		return std::string("EOC");
		case DataType::kTypeEmptySlot:	return std::string("(EMPRY-SLOT)");
		case DataType::kTypeDirectWord:	return std::string("(WORD-PTR)");
		case DataType::kTypeIP:			return std::string("(IPDATA)");
		case DataType::kTypeParamDest:	return std::string("PARAM-DEST");
		case DataType::kTypeNewWord:	return std::string("(NEW-WORD-PTR)");
		case DataType::kTypeAddress:	return std::string("(ADDRESS)");
		case DataType::kTypeThreshold:	return std::string("(Threshold)");

		case DataType::kTypeBool:	 	return std::string("bool");
		case DataType::kTypeInt:		return std::string("int");
		case DataType::kTypeLong:		return std::string("long");
		case DataType::kTypeBigInt:		return std::string("bigInt");
		case DataType::kTypeFloat:		return std::string("float");
		case DataType::kTypeDouble:		return std::string("double");
		case DataType::kTypeBigFloat:	return std::string("bigFloat");
		case DataType::kTypeString:		return std::string("string");
		case DataType::kTypeMayBeAWord:	return std::string("mayBeAWord");
		case DataType::kTypeWord:		return std::string("word");

		case DataType::kTypeArray:		return std::string("array");
		case DataType::kTypeList:		return std::string("list");
		case DataType::kTypeKV:			return std::string("assoc");

		case DataType::kTypeSymbol:		return std::string("symbol");
		case DataType::kTypeFile:		return std::string("file");
		case DataType::kTypeEOF:		return std::string("eof");

		case DataType::kTypeMiscInt:	return std::string("(MISC-INT)");
		case DataType::kTypeLVOP:		return std::string("(LVOP)");
		case DataType::kTypeCB:			return std::string("(CBT)");

		case DataType::kTypeStdCode:	return std::string("stdCode");
		default: ;	// empty
	}
	fprintf(stderr,"UNKNOWN dataType=%d\n",(int)dataType);
	return std::string("### SYSTEM_ERROR ###");
}

static void printIndent(int inIndent);
static std::string indentStr(int inIndent);

int TypedValue::GetLevel() const {
	return dataType==DataType::kTypeDirectWord ? (int)wordPtr->level : 0;
}

PP_API void TypedValue::PrintValue(int inIndent) const {
	printIndent(inIndent);
	std::string valueStr=GetValueString(inIndent);
	printf("%s",valueStr.c_str());
}

// if inIndent<0 use short format.
static std::string getArrayValueString(const TypedValue& inTV,
									   const int inIndent) {
	std::string ret=(boost::format("array:(size=%d data=%p)")
					%inTV.arrayPtr->length%inTV.arrayPtr->data).str();
	if(inIndent>=0) {
		ret+="\n";
		for(int i=0; i<inTV.arrayPtr->length; i++) {
			ret+=indentStr(inIndent);
			ret+=(boost::format("[%d]=")%i).str();
			ret+=inTV.arrayPtr->data[i].GetValueString(inIndent+1);
			ret+="\n";
		}
	} else {
		ret+="...";
	}
	return ret;
}

static std::string getListValueString(const TypedValue& inTV,
									  const int inIndent) {
	const size_t n=inTV.listPtr->size();
	if(n==0) { return "( )"; }

	int childIndent=inIndent;
	std::string ret="( "+inTV.listPtr->at(0).GetValueString(childIndent);;
	std::string arraySpace = childIndent>=0 ? indentStr(childIndent) : " ";
	bool isNewLine=false;
	for(size_t i=1; i<n; i++) {
		if(inTV.listPtr->at(i).dataType==DataType::kTypeArray) {
			isNewLine=true;
			ret+=arraySpace;
		} else {
			ret+=" ";
		}
		ret+=inTV.listPtr->at(i).GetValueString(childIndent);
	}
	if( isNewLine ) {
		// ret+="\n";
		ret+=indentStr(inIndent)+")";
	} else {
		ret+=" )";
	}
	return ret;
}

static std::string getKvValueString(const TypedValue& inTV,
								    const int inIndent) {
	std::string ret="( ";
	for(auto kv : *inTV.kvPtr) {
		ret+="<"+kv.first.GetValueString(inIndent);
		ret+=","+kv.second.GetValueString(inIndent);
		ret+="> ";
	}
	ret+=")";
	return ret;
}

PP_API std::string TypedValue::GetEscapedValueString(int inIndent) const {
	if(dataType!=DataType::kTypeString) { return GetValueString(inIndent); }
	const char *s=stringPtr->c_str();
	std::string ret;
	ret.reserve(stringPtr->length()*2);
	if(inIndent>0) {
		for(int i=0; i<inIndent; i++) { ret+='\t'; }
	}
	for(int i=0; s[i]!='\0'; i++) {
		switch(s[i]) {
			case '\t': ret+="\\t"; break;
			case '\r': ret+="\\r"; break;
			case '\n': ret+="\\n"; break;
			default:
				ret+=s[i];
		}
	}
	return ret;
}

PP_API std::string TypedValue::GetValueString(int inIndent) const {
	std::string ret;
	switch(dataType) {
		case DataType::kTypeInvalid:	ret="(invalid-value)";	break;
		case DataType::kTypeEOC:		ret="(EOC)";			break;

		case DataType::kTypeIP:
			ret=(boost::format("(internal-data:IP %p)")%ipValue).str();
			break;
		case DataType::kTypeParamDest:
			ret=(boost::format("(PARAM-DEST %p)")%ipValue).str();
			break;

		case DataType::kTypeDirectWord:
		case DataType::kTypeNewWord:
		case DataType::kTypeWord:
			ret=(boost::format("%p[%s]")%wordPtr%wordPtr->longName).str();
			break;

		case DataType::kTypeBool:
		case DataType::kTypeEmptySlot:
			ret = boolValue ? "true" : "false";
			break;

		case DataType::kTypeAddress:
		case DataType::kTypeThreshold:
		case DataType::kTypeInt:
			ret=std::to_string(intValue);
			break;

		case DataType::kTypeLong:	ret=std::to_string(longValue);		break;
		case DataType::kTypeFloat:	ret=std::to_string(floatValue); 	break;
		case DataType::kTypeDouble:	ret=std::to_string(doubleValue);	break;

		case DataType::kTypeBigInt:		ret=bigIntPtr->str();	break;
		case DataType::kTypeBigFloat:	ret=bigFloatPtr->str();	break;

		case DataType::kTypeSymbol:
		case DataType::kTypeMayBeAWord:
		case DataType::kTypeString:
			ret=std::string(*stringPtr);
			break;
			
		case DataType::kTypeArray: 	ret=getArrayValueString(*this,inIndent);	break;
		case DataType::kTypeList:  	ret=getListValueString(*this,inIndent);		break;
		case DataType::kTypeKV:	 	ret=getKvValueString(*this,inIndent);		break;

		case DataType::kTypeFile:
			ret=(boost::format("(file-pointer: %p)")%filePtr.get()).str();
			break;
		
		case DataType::kTypeEOF:	ret="(EOF)";				break;
		
		case DataType::kTypeLVOP:	ret=disAsmLVOP(intValue);	break;

		case DataType::kTypeCB:
			ret=controlBlockNameStr((ControlBlockType)intValue);
			break;

		case DataType::kTypeStdCode: {
				std::stringstream strm;
				strm << (void*)stdCodePtr;
				ret=strm.str();
			}
			break;

		default:
			fprintf(stderr,"SYSTEM ERROR at TypedValue::GetValueString().\n");
			exit(-1);
	}
	return ret;
}
static void printIndent(int inIndent) {
	for(int i=0; i<inIndent; i++) {
		putchar('\t');
	}
}
static std::string indentStr(int inIndent) {
	std::string ret;
	for(int i=0; i<inIndent; i++) {
		ret+="    ";	// 4-tab
	}
	return ret;
}

static std::string disAsmLVOP(int inLVOP) {
	LVOP lvop=static_cast<LVOP>(inLVOP);
	LVOP op=lvop & LVOP::opMask;
	std::string ret=GetOpStr(op);
	switch(op) {
		case LVOP::NOP:
			// nothing
			break;
		case LVOP::ADD:
		case LVOP::SUB:
		case LVOP::MUL:
		case LVOP::DIV:
		case LVOP::MOD: 
			ret+=" "+GetSrc1Str(lvop&LVOP::src1Mask)
				+","+GetSrc2Str(lvop&LVOP::src2Mask)
				+","+GetDestStr(lvop&LVOP::destMask);
			break;
		case LVOP::INC:
		case LVOP::DEC:
		case LVOP::TWC:
		case LVOP::HLF:
		case LVOP::PW2:
			ret+=" "+GetSrc1Str(lvop&LVOP::src1Mask)
				+",,"+GetDestStr(lvop&LVOP::destMask);
			break;
		case LVOP::TLV:
		case LVOP::RTL:
			ret+=" "+GetFromStackStr(lvop&LVOP::src1Mask)
			    +",,"+GetDestStr(lvop&LVOP::destMask);
			break;
		case LVOP::TST:
			ret+=" "+GetSrc1Str(lvop&LVOP::src1Mask)
			    +",,"+GetToStackStr(lvop&LVOP::destMask);
			break;

		case LVOP::AddPush:
		case LVOP::SubPush:
		case LVOP::MulPush:
		case LVOP::DivPush:
		case LVOP::ModPush:
			ret+=" "+GetSrc1Str(lvop&LVOP::src1Mask)
				+","+GetSrc2Str(lvop&LVOP::src2Mask)
				+","+GetToStackStr(lvop&LVOP::destMask);
			break;

		case LVOP::IncPush:
		case LVOP::DecPush:
		case LVOP::TwcPush:
		case LVOP::HlfPush:
		case LVOP::Pw2Push:
			ret+=" "+GetSrc1Str(lvop&LVOP::src1Mask)
				+",,"+GetToStackStr(lvop&LVOP::destMask);
			break;

		case LVOP::PopAdd:
		case LVOP::PopSub:
		case LVOP::PopMul:
		case LVOP::PopDiv:
		case LVOP::PopMod:
			ret+=" "+GetFromStackStr(lvop&LVOP::src1Mask)
				+","+GetSrc2Str(lvop&LVOP::src2Mask)
				+","+GetDestStr(lvop&LVOP::destMask);
			break;

		default:
			fprintf(stderr,"SYSTEM ERROR in disAsmLVOP(). LVOP=%X\n",inLVOP);
			exit(-1);
	}
	return ret;
}

static std::string controlBlockNameStr(const ControlBlockType inCBT) {
	std::string ret;
	switch(inCBT) {
		case ControlBlockType::kGROUP_MASK:
			ret="(GROUP_MSK)";		
			break;

		case ControlBlockType::kOPEN_COMMENT_GROUP:
			ret="(COMMENT_GROUP)";
			break;
		case ControlBlockType::kOPEN_C_STYLE_COMMENT:
			ret="(OPEN_C_COMMENT)";	
			break;
		case ControlBlockType::kOPEN_CPP_STYLE_ONE_LINE_COMMENT:
			ret="(OPEN_CPP_COMMENT)";
			break;

		case ControlBlockType::kOPEN_IF_GROUP:
			ret="(IF_GROUP)";
			break;
		case ControlBlockType::kSyntax_IF:
			ret="(if)";
			break;

		case ControlBlockType::kOPEN_LEAVABLE_LOOP_GROUP:
			ret="(LEAVABLE_LOOP_GROUP)";
			break;
		case ControlBlockType::kSyntax_FOR_PLUS:	
			ret="(for+)";
			break;
		case ControlBlockType::kSyntax_FOR_MINUS:
			ret="(for-)";
			break;
		case ControlBlockType::kSyntax_WHILE:
			ret="(while)";
			break;
		case ControlBlockType::kSyntax_LOOP:
			ret="(loop)";
			break;

		case ControlBlockType::kOPEN_SWITCH_GROUP:
			ret="(SWITCH_GROUP)";
			break;
		case ControlBlockType::kSyntax_SWITCH:
			ret="(switch)";
			break;

		default:
			fprintf(stderr,"SYSTEM ERROR at controlBlockNameStr().\n");
			exit(-1);
	}
	return ret;
}

PP_API void TypedValue::Dump() const {
	printf("dataType:%s ",GetTypeStr().c_str());
	printf("value:"); PrintValue(); printf("\n");
}

