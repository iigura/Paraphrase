#include <boost/format.hpp>

#include "word.h"
#include "typedValue.h"
#include "stack.h"
#include "context.h"

const BigInt gBI_DBL_MAX(DBL_MAX);
const BigInt gBI_FLT_MAX(FLT_MAX);

const BigFloat gBF_DBL_MAX(DBL_MAX);
const BigFloat gBF_FLT_MAX(FLT_MAX);

PP_API double TypedValue::ToDouble(Context& inContext) {
	switch(dataType) {
		case kTypeInt:		return (double)intValue;
		case kTypeLong:		return (double)longValue;
		case kTypeFloat:	return (double)floatValue;
		case kTypeDouble:	return doubleValue;
		case kTypeBigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_DBL_MAX) {
				return static_cast<double>(bigInt);
			} else {
				inContext.Error(E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
			}
		}
		case kTypeBigFloat: {
			const BigFloat& bigFloat=*bigFloatPtr;
			if(abs(bigFloat)<gBF_DBL_MAX) {
				return static_cast<double>(bigFloat);
			} else {
				inContext.Error(E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW);
			}
		}
		default:
			inContext.Error_InvalidType(E_TOS_CAN_NOT_CONVERT_TO_DOUBLE,*this);
	}
	return std::numeric_limits<double>::quiet_NaN();
}

PP_API float TypedValue::ToFloat(Context& inContext) {
	switch(dataType) {
		case kTypeInt:		return (float)intValue;
		case kTypeLong:		return (float)longValue;
		case kTypeFloat:	return floatValue;
		case kTypeDouble:	return (float)doubleValue;
		case kTypeBigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_FLT_MAX) {
				return static_cast<float>(*bigIntPtr);
			} else {
				inContext.Error(E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
			}
		}
		case kTypeBigFloat: {
			const BigFloat& bigFloat=*bigFloatPtr;
			if(abs(bigFloat)<=gBF_FLT_MAX) {
				return static_cast<float>(*bigFloatPtr);
			} else {
				inContext.Error(E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW);
			}
		}
		default:
			inContext.Error_InvalidType(E_TOS_CAN_NOT_CONVERT_TO_FLOAT,*this);
	}
	return std::numeric_limits<float>::quiet_NaN();
}

PP_API std::string TypedValue::GetTypeStr() const {
	switch(dataType) {
		case kTypeInvalid:		return std::string("(INVALID)");
		case kTypeEOC:			return std::string("EOC");
		case kTypeEmptySlot:	return std::string("(EMPRY-SLOT)");
		case kTypeDirectWord:	return std::string("(WORD-PTR)");
		case kTypeIP:			return std::string("(IPDATA)");
		case kTypeParamDest:	return std::string("PARAM-DEST");
		case kTypeNewWord:		return std::string("(NEW-WORD-PTR)");
		case kTypeAddress:		return std::string("(ADDRESS)");
		case kTypeThreshold:	return std::string("(Threshold)");

		case kTypeBool:		 	return std::string("bool");
		case kTypeInt:			return std::string("int");
		case kTypeLong:			return std::string("long");
		case kTypeBigInt:		return std::string("bigInt");
		case kTypeFloat:		return std::string("float");
		case kTypeDouble:		return std::string("double");
		case kTypeBigFloat:		return std::string("bigFloat");
		case kTypeString:		return std::string("string");
		case kTypeWord:			return std::string("word");
		case kTypeArray:		return std::string("array");
		case kTypeList:			return std::string("list");
		case kTypeSymbol:		return std::string("symbol");
		case kTypeFile:			return std::string("file");
		case kTypeEOF:			return std::string("eof");

		case kTypeMiscInt:		return std::string("(MISC-INT)");
	}
	fprintf(stderr,"UNKNOWN dataType=%d\n",dataType);
	return std::string("### SYSTEM_ERROR ###");
}

static void printIndent(int inIndent);
static std::string indentStr(int inIndent);

int TypedValue::GetLevel() const {
	return dataType==kTypeDirectWord ? (int)wordPtr->level : 0;
}

PP_API void TypedValue::PrintValue(int inIndent) const {
	printIndent(inIndent);
	std::string valueStr=GetValueString(inIndent);
	printf("%s",valueStr.c_str());
}

PP_API std::string TypedValue::GetValueString(int inIndent) const {
	std::string ret;
	switch(dataType) {
		case kTypeInvalid:	ret="(invalid-value)";	break;
		case kTypeEOC:		ret="(EOC)";			break;

		case kTypeIP:
			ret=(boost::format("(internal-data:IP %p)")%ipValue).str();
			break;
		case kTypeParamDest:
			ret=(boost::format("(PARAM-DEST %p)")%ipValue).str();
			break;

		case kTypeDirectWord:
		case kTypeNewWord:
		case kTypeWord:
			ret=(boost::format("%p[%s]")%wordPtr%wordPtr->longName).str();
			break;

		case kTypeBool:
		case kTypeEmptySlot:
			ret = boolValue ? "true" : "false";
			break;

		case kTypeAddress:
		case kTypeThreshold:
		case kTypeInt:
			ret=std::to_string(intValue);
			break;

		case kTypeLong:		ret=std::to_string(longValue);		break;
		case kTypeFloat:	ret=std::to_string(floatValue); 	break;
		case kTypeDouble:	ret=std::to_string(doubleValue);	break;

		case kTypeBigInt:	ret=bigIntPtr->str();	break;
		case kTypeBigFloat:	ret=bigFloatPtr->str();	break;

		case kTypeSymbol:
		case kTypeString:
			ret=std::string(*stringPtr);
			break;
			
		case kTypeArray:
			ret=(boost::format("array (size=%d data=%p)")
				 %arrayPtr->length%arrayPtr->data).str();
			if(inIndent>=0) {
				for(int i=0; i<arrayPtr->length; i++) {
					ret+=indentStr(inIndent);
					ret+=(boost::format("\t[%d]=")%i).str();
					ret+=arrayPtr->data[i].GetValueString(inIndent+1);
				}
			} else {
				ret+="...";
			}
			break;

		case kTypeList: {
				ret="( ";
				const size_t n=listPtr->size();
				for(size_t i=0; i<n; i++) {
					ret+=listPtr->at(i).GetValueString(inIndent);
					ret+=" ";
				}
				ret+=")";
			}
			break;

		case kTypeFile:
			ret=(boost::format("(file-pointer: %p)")%filePtr.get()).str();
			break;
		
		case kTypeEOF:		ret="(EOC)";			break;

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
		ret+="\t";
	}
	return ret;
}
	
PP_API void TypedValue::Dump() const {
	printf("dataType:%s ",GetTypeStr().c_str());
	printf("value:"); PrintValue(); printf("\n");
}

