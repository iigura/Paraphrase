#include <boost/format.hpp>

#include "word.h"
#include "chan.h"
#include "typedValue.h"
#include "stack.h"
#include "context.h"

const BigInt gBI_DBL_MAX(DBL_MAX);
const BigInt gBI_FLT_MAX(FLT_MAX);

static std::string disAsmLVOP(int inLVOP);
static std::string controlBlockNameStr(const ControlBlockType inCBT);

PP_API TypedValue::TypedValue(Context *inContextPtr):dataType(DataType::Context) {
	new(&contextPtr) std::shared_ptr<Context>(inContextPtr);
}

PP_API TypedValue::TypedValue(ChanMan *inChannel):dataType(DataType::Channel) {
	new(&channelPtr) std::shared_ptr<ChanMan>(inChannel);
}

PP_API double TypedValue::ToDouble(Context& inContext) {
	switch(dataType) {
		case DataType::Int:		return (double)intValue;
		case DataType::Long:	return (double)longValue;
		case DataType::Float:	return (double)floatValue;
		case DataType::Double:	return doubleValue;
		case DataType::BigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_DBL_MAX) {
				return static_cast<double>(bigInt);
			} else {
				inContext.Error(NoParamErrorID::CanNotConvertToDoubleDueToOverflow);
			}
		}
		default:
			inContext.Error(InvalidTypeErrorID::TosCanNotConvertToDouble,*this);
	}
	return std::numeric_limits<double>::quiet_NaN();
}

PP_API float TypedValue::ToFloat(Context& inContext) {
	switch(dataType) {
		case DataType::Int:	return (float)intValue;
		case DataType::Long:	return (float)longValue;
		case DataType::Float:	return floatValue;
		case DataType::Double:	return (float)doubleValue;
		case DataType::BigInt: {
			const BigInt& bigInt=*bigIntPtr;
			if(abs(bigInt)<=gBI_FLT_MAX) {
				return static_cast<float>(*bigIntPtr);
			} else {
				inContext.Error(NoParamErrorID::CanNotConvertToFloatDueToOverflow);
			}
		}
		default:
			inContext.Error(InvalidTypeErrorID::TosCanNotConvertToFloat,*this);
	}
	return std::numeric_limits<float>::quiet_NaN();
}

PP_API TypedValue FullClone(TypedValue& inTV) {
	switch(inTV.dataType) {
		case DataType::Array: {
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
		case DataType::List: {
				std::deque<TypedValue> *srcPtr=inTV.listPtr.get();
				std::deque<TypedValue> *destPtr=new std::deque<TypedValue>();
				const int n=(int)srcPtr->size();
				for(int i=0; i<n; i++) {
					destPtr->emplace_back(FullClone(srcPtr->at(i)));
				}
				return TypedValue(destPtr);
			}
		case DataType::KV: {
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
		case DataType::Invalid:		return std::string("(INVALID)");
		case DataType::EoC:			return std::string("EoC");
		case DataType::EmptySlot:	return std::string("(EMPRY-SLOT)");
		// case DataType::DirectWord:	return std::string("(WORD-PTR)");
		case DataType::IP:			return std::string("(IPDATA)");
		case DataType::ParamDest:	return std::string("PARAM-DEST");
		case DataType::NewWord:		return std::string("(NEW-WORD-PTR)");
		case DataType::Address:		return std::string("(ADDRESS)");
		case DataType::Threshold:	return std::string("(Threshold)");
		case DataType::Lambda:		return std::string("(LAMBDA)");

		case DataType::Bool:	 	return std::string("bool");
		case DataType::Int:			return std::string("int");
		case DataType::Long:		return std::string("long");
		case DataType::BigInt:		return std::string("bigInt");
		case DataType::Float:		return std::string("float");
		case DataType::Double:		return std::string("double");
		case DataType::String:		return std::string("string");
		case DataType::MayBeAWord:	return std::string("mayBeAWord");
		case DataType::Word:		return std::string("word");

		case DataType::Array:		return std::string("array");
		case DataType::List:		return std::string("list");
		case DataType::KV:			return std::string("assoc");

		case DataType::Symbol:		return std::string("symbol");
		case DataType::File:		return std::string("file");
		case DataType::EoF:			return std::string("eof");
		case DataType::Context:		return std::string("context");
		case DataType::Channel:		return std::string("channel");

		case DataType::MiscInt:	return std::string("(MISC-INT)");
		case DataType::LVOP:		return std::string("(LVOP)");
		case DataType::CB:			return std::string("(CBT)");

		case DataType::StdCode:	return std::string("stdCode");

		case DataType::UserData: return std::string("userData");

		default: ;	// empty
	}
	fprintf(stderr,"UNKNOWN dataType=%d\n",(int)dataType);
	return std::string("### SYSTEM_ERROR ###");
}

static void printIndent(int inIndent);
static std::string indentStr(int inIndent);

#if 0
PP_API int TypedValue::GetLevel() const {
	return dataType==DataType::DirectWord ? (int)wordPtr->level : 0;
}
#endif

PP_API void TypedValue::PrintValue(int inIndent,bool inTypePostfix) const {
	printIndent(inIndent);
	std::string valueStr=GetValueString(inIndent,true,inTypePostfix);
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
									  const int inIndent,
									  bool inTypePostfix) {
	const size_t n=inTV.listPtr->size();
	if(n==0) { return "( )"; }

	int childIndent=inIndent;
	std::string ret="(";
	std::string arraySpace = childIndent>=0 ? indentStr(childIndent) : " ";
	bool isNewLine=false;
	for(size_t i=0; i<n; i++) {
		if(inTV.listPtr->at(i).dataType==DataType::Array) {
			isNewLine=true;
			ret+=" "+arraySpace;
		} else {
			ret+=" ";
		}
		TypedValue element=inTV.listPtr->at(i);
		if(element.dataType==DataType::String) {
			ret+="\""+element.GetValueString(childIndent)+"\"";
		} else {
			ret+=element.GetValueString(childIndent,false,inTypePostfix);
		}
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
								    const int inIndent,bool inDetail) {
	std::string ret="( ";
	if( inDetail ) {
		for(auto kv : *inTV.kvPtr) {
			ret+="<"+kv.first.GetValueString(inIndent);
			ret+=","+kv.second.GetValueString(inIndent);
			ret+="> ";
		}
	} else {
		ret+="num of "+std::to_string(inTV.kvPtr->size())+" pairs";;
	}
	ret+=")";
	return ret;
}

PP_API std::string TypedValue::GetEscapedValueString(int inIndent) const {
	if(dataType!=DataType::String) { return GetValueString(inIndent); }
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

PP_API std::string TypedValue::GetValueString(int inIndent,
											  bool inDetail,
											  bool inTypePostfix) const {
	std::string ret;
	switch(dataType) {
		case DataType::Invalid:	ret="(invalid-value)";	break;
		case DataType::EoC:		ret="(EoC)";			break;

		case DataType::IP:
			ret=(boost::format("(internal-data:IP %p)")%ipValue).str();
			break;
		case DataType::ParamDest:
			ret=(boost::format("(PARAM-DEST %p)")%ipValue).str();
			break;

		// case DataType::DirectWord:
		case DataType::NewWord:
		case DataType::Word:
			ret=(boost::format("%p[%s]")%wordPtr%wordPtr->longName).str();
			break;

		case DataType::Lambda:
			ret=(boost::format("%p[%s]")%lambdaPtr.get()%lambdaPtr.get()->longName).str();
			break;

		case DataType::Bool:
		case DataType::EmptySlot:
			ret = boolValue ? "true" : "false";
			break;

		case DataType::Address:
		case DataType::Threshold:
		case DataType::Int:
			ret=std::to_string(intValue);
			break;

		case DataType::Long:
			ret=std::to_string(longValue);
			if( inTypePostfix ) { ret+="L"; }
			break;
		case DataType::Float:
			ret=std::to_string(floatValue);
			if( inTypePostfix ) { ret+="f"; }
			break;
		case DataType::Double:	ret=std::to_string(doubleValue);	break;

		case DataType::BigInt:
			ret=bigIntPtr->str();
			if( inTypePostfix ) { ret+="LL"; }
			break;

		case DataType::Symbol:
		case DataType::MayBeAWord:
		case DataType::String:
			ret=std::string(*stringPtr);
			break;
			
		case DataType::Array: 	ret=getArrayValueString(*this,inIndent);		break;
		case DataType::List:
			ret=getListValueString(*this,inIndent,inTypePostfix);
			break;
		case DataType::KV:	 	ret=getKvValueString(*this,inIndent,inDetail);	break;

		case DataType::File:
			ret=(boost::format("(file-pointer: %p)")%filePtr.get()).str();
			break;
		
		case DataType::EoF:		ret="(EoF)";		break;

		case DataType::Context:
			ret=(boost::format("(context-pointer: %p)")%contextPtr.get()).str();
			break;

		case DataType::Channel:
			ret=(boost::format("(channel-pointer: %p ")%channelPtr.get()).str();
			ret+=std::string("name='")+channelPtr->name+"')";
			break;
		
		case DataType::LVOP:	ret=disAsmLVOP(intValue);	break;

		case DataType::CB: ret=controlBlockNameStr((ControlBlockType)intValue); break;

		case DataType::StdCode: {
				std::stringstream strm;
				strm << (void*)stdCodePtr;
				ret=strm.str();
			}
			break;

		case DataType::UserData:
			ret=(boost::format("(%p ")%userDataPtr.get()).str();
			ret+=std::string("memo='")+userDataPtr.get()->memo+"')";
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
		case ControlBlockType::OpenCStyleComment:
			ret="(OpenCComment)";	
			break;
		case ControlBlockType::OpenCppStyleOneLineComment:
			ret="(OpenCppComment)";
			break;

		case ControlBlockType::kOPEN_IF_GROUP:
			ret="(IF_GROUP)";
			break;
		case ControlBlockType::SyntaxIf:
			ret="(if)";
			break;

		case ControlBlockType::kOPEN_LEAVABLE_LOOP_GROUP:
			ret="(LEAVABLE_LOOP_GROUP)";
			break;
		case ControlBlockType::SyntaxForPlus:	ret="(for+)"; 	break;
		case ControlBlockType::SyntaxForMinus: 	ret="(for-)"; 	break;
		case ControlBlockType::SyntaxWhile: 	ret="(while)"; 	break;
		case ControlBlockType::SyntaxLoop: 		ret="(loop)"; 	break;

		case ControlBlockType::kOPEN_SWITCH_GROUP:
			ret="(SWITCH_GROUP)";
			break;
		case ControlBlockType::SyntaxSwitch: ret="(switch)"; break;

		default: {
				if(((int)inCBT & (int)ControlBlockType::kTHREAD)!=0) {
					ret="(thread-info:";
					if(((int)inCBT & (int)ControlBlockType::kThreadMask_Open_ThreadOutputAsInput)!=0) {
						ret+=">[ ";
					} else if(((int)inCBT & (int)ControlBlockType::kThreadMask_Open_TosChanAsInput)!=0) {
						ret+=">>[ ";
					} else {
						ret+="[ ";
					}
					if(((int)inCBT & (int)ControlBlockType::kThreadMask_Close_ThreadOutputAsInput)!=0) {
						ret+=" ]>)";
					} else {
						ret+=" ])";
					}
				} else {
					fprintf(stderr,"SYSTEM ERROR at controlBlockNameStr().\n");
					exit(-1);
				}
			}
	}
	return ret;
}

PP_API void TypedValue::Dump() const {
	printf("dataType:%s ",GetTypeStr().c_str());
	printf("value:"); PrintValue(); printf("\n");
}

PP_API bool IsValidDataTypeValue(DataType inDataType) {
	switch(inDataType) {
		case DataType::Invalid:
		case DataType::EoC:
		case DataType::EmptySlot:
		// case DataType::DirectWord:
		case DataType::IP:
		case DataType::ParamDest:
		case DataType::NewWord:
		case DataType::Address:
		case DataType::Threshold:

		case DataType::Bool:
		case DataType::Int:
		case DataType::Long:
		case DataType::BigInt:
		case DataType::Float:
		case DataType::Double:
		case DataType::String:
		case DataType::MayBeAWord:
		case DataType::Word:

		case DataType::Array:
		case DataType::List:
		case DataType::KV:

		case DataType::Symbol:
		case DataType::File:
		case DataType::EoF:

		case DataType::MiscInt:
		case DataType::LVOP:
		case DataType::CB:

		case DataType::StdCode:

		case DataType::UserData:
			return true;
		default:
			;	// empty
	}
	return false;
}

PP_API bool IsSameValue(const TypedValue& inTV1,const TypedValue& inTV2) {
	if(inTV1==inTV2) { return true; }
	switch(inTV1.dataType) {
		case DataType::Int:
			switch(inTV2.dataType) {
				case DataType::Int: 	return inTV1.intValue==inTV2.intValue;
				case DataType::Long:	return inTV1.intValue==inTV2.longValue;
				case DataType::BigInt:	return inTV1.intValue==*inTV2.bigIntPtr;
				case DataType::Float:	return inTV1.intValue==inTV2.floatValue;
				case DataType::Double:	return inTV1.intValue==inTV2.doubleValue;
				default:				return false;
			}
			break;
		case DataType::Long:
			switch(inTV2.dataType) {
				case DataType::Int:		return inTV1.longValue==inTV2.intValue;
				case DataType::Long:	return inTV1.longValue==inTV2.longValue;
				case DataType::BigInt:	return inTV1.longValue==*inTV2.bigIntPtr;
				case DataType::Float:	return inTV1.longValue==inTV2.floatValue;
				case DataType::Double:	return inTV1.longValue==inTV2.doubleValue;
				default:				return false;
			}
			break;
		case DataType::BigInt:
			switch(inTV2.dataType) {
				case DataType::Int:		return *inTV1.bigIntPtr==inTV2.intValue;
				case DataType::Long:	return *inTV1.bigIntPtr==inTV2.longValue;
				case DataType::BigInt:	return *inTV1.bigIntPtr==*inTV2.bigIntPtr;
				case DataType::Float:	return *inTV1.bigIntPtr==inTV2.floatValue;
				case DataType::Double:	return *inTV1.bigIntPtr==inTV2.doubleValue;
				default:				return false;
			}
			break;
		case DataType::Float:
			switch(inTV2.dataType) {
				case DataType::Int:		return inTV1.floatValue==inTV2.intValue;
				case DataType::Long:	return inTV1.floatValue==inTV2.longValue;
				case DataType::BigInt:	return inTV1.floatValue==*inTV2.bigIntPtr;
				case DataType::Float:	return inTV1.floatValue==inTV2.floatValue;
				case DataType::Double:	return inTV1.floatValue==inTV2.doubleValue;
				default:				return false;
			}
			break;
		case DataType::Double:
			switch(inTV2.dataType) {
				case DataType::Int:		return inTV1.doubleValue==inTV2.intValue;
				case DataType::Long:	return inTV1.doubleValue==inTV2.longValue;
				case DataType::BigInt:	return inTV1.doubleValue==*inTV2.bigIntPtr;
				case DataType::Float:	return inTV1.doubleValue==inTV2.floatValue;
				case DataType::Double:	return inTV1.doubleValue==inTV2.doubleValue;
				default:				return false;
			}
			break;
		case DataType::List: {
				if(inTV2.dataType!=DataType::List) { return false; }
				if(inTV1.listPtr->size()!=inTV2.listPtr->size()) { return false; }
				int n=(int)inTV1.listPtr->size();
				for(int i=0; i<n; i++) {
					if(IsSameValue(inTV1.listPtr->at(i),inTV2.listPtr->at(i))==false) {
						return false;
					}
				}
				return true;
			}
			break;
		case DataType::Array: {
				if(inTV2.dataType!=DataType::Array) { return false; }
				if(inTV1.arrayPtr->length!=inTV2.arrayPtr->length) { return false; }
				int n=inTV1.arrayPtr->length;
				for(int i=0; i<n; i++) {
					if(IsSameValue(inTV1.arrayPtr->data[i],
								   inTV2.arrayPtr->data[i])==false) {
						return false;
					}
				}
				return true;
			}
			break;
		case DataType::KV: {
				if(inTV2.dataType!=DataType::KV) { return false; }
				if(inTV1.kvPtr->size()!=inTV2.kvPtr->size()) { return false; }
				for(auto const &pair : *inTV1.kvPtr) {
					auto itr=inTV2.kvPtr->find(pair.first);
					if(itr==inTV2.kvPtr->end()) { return false; }
					if(!(pair.first==itr->first)) { return false; }
				}
				return true;
			}
			break;
		default:
			return false;
	}
}
