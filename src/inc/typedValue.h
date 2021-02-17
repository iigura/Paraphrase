#pragma once

#include <float.h>

#include <memory>
#include <string>
#include <deque>
#include <unordered_map>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <assert.h>

struct Word;
struct Context;

#include "thread.h"
#include "paraphrase.h"
#include "array.h"
#include "file.h"
#include "lvop.h"

typedef bool (*WordFunc)(Context& inContext) NOEXCEPT;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::multiprecision::cpp_dec_float_100 BigFloat;

enum class Level;	// see context.h

const int kType_HeavyMask=0x4000;
enum class DataType {
	kTypeInvalid	=9999,	// no values
	kTypeMayBeAWord =kType_HeavyMask | 9998,	// for args, use stringPtr.
	kTypeDirectWord	=9000,
	kTypeIP			=9100,
	kTypeNewWord	=9200,	// wordPtr value as NewWord.
	kTypeThreshold	=9300,	// intValue as ExecutionThreshold
	kTypeEmptySlot	=9400,	// use intValue as execution-threshld level backup.
	kTypeAddress	=9500,	// intValue as an address.
	kTypeParamDest	=9600,	// ipValue's vallue as a pointer to param[target]
	//kTypeEnvFrame,

	kTypeBool	 =100,
	kTypeInt	 =0,
	kTypeLong	 =1,
	kTypeBigInt	 =kType_HeavyMask | 2,
	kTypeFloat	 =3,
	kTypeDouble	 =4,
	kTypeBigFloat=kType_HeavyMask | 5,

	kTypeString=kType_HeavyMask | 200,
	kTypeWord=300,

	kTypeArray=kType_HeavyMask | 410,
	kTypeList =kType_HeavyMask | 420,
	kTypeKV   =kType_HeavyMask | 430,

	kTypeSymbol=kType_HeavyMask | 600,
	kTypeEOC=700,	// no values
	kTypeFile	=kType_HeavyMask | 800,
	kTypeEOF	=801,	// no values

	kTypeStdCode=999,	// use stdCodePtr

	kTypeMiscInt=900,	// use intValue as misc. data.
	kTypeLVOP=1000,		// use intValue as LVOP.
	kTypeCB =2000,		// use intValue as CB (Control Block) info.
};

enum class DebugCommandType {
	kDCTypeSystemError,
	kDCTypeInvalid,
	kDCTypeContinue,
	kDCTypeStepOver,
	kDCTypeStepIn,
	kDCTypeQuit,
};

inline int GetMathOpType(DataType inType1,DataType inType2) {
	return (((int)inType1)<<16) | ((int)inType2);
}

#define MathOpCombi(s,t) ((((int)s)<<16) | ((int)t))

enum class TypeCombinationForMathOP {
	kIntInt 	=MathOpCombi(DataType::kTypeInt,DataType::kTypeInt),
	kIntLong	=MathOpCombi(DataType::kTypeInt,DataType::kTypeLong),
	kIntBigInt	=MathOpCombi(DataType::kTypeInt,DataType::kTypeBigInt),
	kIntFloat	=MathOpCombi(DataType::kTypeInt,DataType::kTypeFloat),
	kIntDouble	=MathOpCombi(DataType::kTypeInt,DataType::kTypeDouble),
   	kIntBigFloat=MathOpCombi(DataType::kTypeInt,DataType::kTypeBigFloat),

	kLongInt	 =MathOpCombi(DataType::kTypeLong,DataType::kTypeInt),
	kLongLong	 =MathOpCombi(DataType::kTypeLong,DataType::kTypeLong),
	kLongBigInt	 =MathOpCombi(DataType::kTypeLong,DataType::kTypeBigInt),
	kLongFloat	 =MathOpCombi(DataType::kTypeLong,DataType::kTypeFloat),
	kLongDouble	 =MathOpCombi(DataType::kTypeLong,DataType::kTypeDouble),
	kLongBigFloat=MathOpCombi(DataType::kTypeLong,DataType::kTypeBigFloat),

	kBigIntInt   	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeInt),
   	kBigIntLong  	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeLong),
	kBigIntBigInt	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeBigInt),
	kBigIntFloat 	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeFloat),
	kBigIntDouble	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeDouble),
	kBigIntBigFloat	=MathOpCombi(DataType::kTypeBigInt,DataType::kTypeBigFloat),

	kFloatInt   	=MathOpCombi(DataType::kTypeFloat,DataType::kTypeInt),
	kFloatLong  	=MathOpCombi(DataType::kTypeFloat,DataType::kTypeLong),
   	kFloatBigInt  	=MathOpCombi(DataType::kTypeFloat,DataType::kTypeBigInt),
	kFloatFloat		=MathOpCombi(DataType::kTypeFloat,DataType::kTypeFloat),
	kFloatDouble	=MathOpCombi(DataType::kTypeFloat,DataType::kTypeDouble),
	kFloatBigFloat	=MathOpCombi(DataType::kTypeFloat,DataType::kTypeBigFloat),

	kDoubleInt  	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeInt),
	kDoubleLong  	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeLong),
	kDoubleBigInt  	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeBigInt),
	kDoubleFloat	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeFloat),
	kDoubleDouble	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeDouble),
	kDoubleBigFloat	=MathOpCombi(DataType::kTypeDouble,DataType::kTypeBigFloat),

	kBigFloatInt  	 =MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeInt),
	kBigFloatLong  	 =MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeLong),
	kBigFloatBigInt  =MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeBigInt),
	kBigFloatFloat	 =MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeFloat),
	kBigFloatDouble	 =MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeDouble),
	kBigFloatBigFloat=MathOpCombi(DataType::kTypeBigFloat,DataType::kTypeBigFloat),

	kInvalidMathOpTypeThreshold=kBigFloatBigFloat+1,
};

// int value for kTypeMiscInt
enum class ControlBlockType {
	kGROUP_MASK=0xFF00,

	kOPEN_COMMENT_GROUP=0x0100,
	kOPEN_C_STYLE_COMMENT			=kOPEN_COMMENT_GROUP | 0x01,
	kOPEN_CPP_STYLE_ONE_LINE_COMMENT=kOPEN_COMMENT_GROUP | 0x02,

	kOPEN_IF_GROUP=0x0200,
	kSyntax_IF=kOPEN_IF_GROUP | 0x01,

	kOPEN_LEAVABLE_LOOP_GROUP=0x0400,
	kSyntax_FOR_PLUS 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x01,
	kSyntax_FOR_MINUS	=kOPEN_LEAVABLE_LOOP_GROUP | 0x02,
	kSyntax_WHILE	 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x11,
	kSyntax_LOOP	 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x12,

	kOPEN_SWITCH_GROUP=0x0800,
	kSyntax_SWITCH=kOPEN_SWITCH_GROUP | 0x01,

	kOPEN_HERE_DOCUMENT_GROUP=0x1000,
	kOPEN_HERE_DOCUMENT_RAW   =kOPEN_HERE_DOCUMENT_GROUP | 0x01,
	kOPEN_HERE_DOCUMENT_DEDENT=kOPEN_HERE_DOCUMENT_GROUP | 0x02,
};

struct TypedValue {
	class TvHash {
		public:
			std::size_t operator() (const TypedValue& inTV) const;
	};

	DataType dataType;
	union {
		const Word *wordPtr;
		const Word **ipValue;
		bool   boolValue;
		int    intValue;
		long   longValue;
		float  floatValue;
		double doubleValue;

		std::shared_ptr<std::string> stringPtr;
		std::shared_ptr< Array<TypedValue> > arrayPtr;
		std::shared_ptr< std::deque<TypedValue> > listPtr;
		std::shared_ptr< std::unordered_map<TypedValue,TypedValue,TvHash> > kvPtr;
		std::shared_ptr<File> filePtr;

		BigInt *bigIntPtr;
		BigFloat *bigFloatPtr;

		WordFunc stdCodePtr;
	};

#ifdef _MSVC_LANG
	#pragma warning(push)
	#pragma warning(disable:26495)
#endif
	TypedValue():dataType(DataType::kTypeInvalid) {
		// empty
	}

	TypedValue(DataType inDataType):dataType(inDataType) {
		assert(inDataType==DataType::kTypeEOC || inDataType==DataType::kTypeEOF);
	}

	inline TypedValue(const TypedValue& inSrc)
	  :dataType(inSrc.dataType) {
//printf("COPY-CONSTRUCTOR\n");
		if(((int)inSrc.dataType & kType_HeavyMask)==0) {
//printf("COPY-CONSTRUCTOR-LightWeight\n");
			doubleValue=inSrc.doubleValue;
		} else {
			switch(inSrc.dataType) {
				case DataType::kTypeBigInt:
					bigIntPtr=new BigInt();
					*bigIntPtr=*inSrc.bigIntPtr;
					break;

				case DataType::kTypeBigFloat:
					bigFloatPtr=new BigFloat();
					*bigFloatPtr=*inSrc.bigFloatPtr;
					break;

				case DataType::kTypeString:
				case DataType::kTypeSymbol:
				case DataType::kTypeMayBeAWord:
					new(&stringPtr) std::shared_ptr<std::string>(inSrc.stringPtr);
					break;

				case DataType::kTypeArray:
					new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
					break;

				case DataType::kTypeList:
					new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
					break;

				case DataType::kTypeKV:
					new(&kvPtr)
						std::shared_ptr<
							std::unordered_map<TypedValue,TypedValue,TvHash>
						>(inSrc.kvPtr);
					break;

				case DataType::kTypeFile:
					new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
					break;

				default:
					fprintf(stderr,"SYSTEM ERROR at TypedValue copy constructor.\n");
					exit(-1);
			}
		}
	}

	TypedValue(const Word *inWordPtr)
	  :dataType(DataType::kTypeDirectWord),wordPtr(inWordPtr) {
		// empty
	}
	
	TypedValue(DataType inDataType,const Word *inWordPtr)
	  :dataType(inDataType),wordPtr(inWordPtr) {
		assert(inDataType==DataType::kTypeNewWord || inDataType==DataType::kTypeWord);
	}

	TypedValue(bool inBoolValue)
	  :dataType(DataType::kTypeBool),boolValue(inBoolValue) {
		// empty
	}

	TypedValue(DataType inDataType,int inIntValue)
	  :dataType(inDataType),intValue(inIntValue) {
		assert(inDataType==DataType::kTypeEmptySlot
			|| inDataType==DataType::kTypeAddress);
	}

	TypedValue(enum Level inLevel)
	  :dataType(DataType::kTypeThreshold),intValue((int)inLevel) {
		// empty
	}

	TypedValue(DataType inDataType,enum ControlBlockType inCBT)
	  :dataType(DataType::kTypeMiscInt),intValue((int)inCBT) {
		assert(inDataType==DataType::kTypeMiscInt);
	}

	TypedValue(DataType inDataType,enum Level inLevel)
	  :dataType(DataType::kTypeEmptySlot),intValue((int)inLevel) {
		assert(inDataType==DataType::kTypeEmptySlot);
	}

	TypedValue(LVOP inLVOP)
	  :dataType(DataType::kTypeLVOP),intValue(static_cast<int>(inLVOP)) {
		// empty
	}

	TypedValue(int inIntValue):dataType(DataType::kTypeInt),intValue(inIntValue) {
		// empty
	}

	TypedValue(DebugCommandType inDCT)
	  :dataType(DataType::kTypeInt),intValue((int)inDCT) {
		// empty
	}

	TypedValue(long inLongValue):dataType(DataType::kTypeLong),longValue(inLongValue) {
		// empty
	}

	TypedValue(const BigInt& inBigInt)
   	  :dataType(DataType::kTypeBigInt),bigIntPtr(new BigInt(inBigInt)) {
		// empty
	}

	TypedValue(float inFloatValue):dataType(DataType::kTypeFloat) {
		floatValue=inFloatValue;
	}

	TypedValue(double inDoubleValue):dataType(DataType::kTypeDouble) {
		doubleValue=inDoubleValue;
	}

	TypedValue(const BigFloat& inBigFloat)
	  :dataType(DataType::kTypeBigFloat),bigFloatPtr(new BigFloat(inBigFloat)) {
		// empty
	}

	TypedValue(const std::string& inStringValue)
	  :dataType(DataType::kTypeString),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(const std::string& inStringValue,DataType /* kTypeSymbol */)
	  :dataType(DataType::kTypeSymbol),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(Array<TypedValue> *inArray):dataType(DataType::kTypeArray) {
		new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inArray);
	}

	TypedValue(std::deque<TypedValue> *inList):dataType(DataType::kTypeList) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inList);
	}

	TypedValue(std::shared_ptr< std::deque<TypedValue> > inList)
	  :dataType(DataType::kTypeList) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >();
		listPtr=inList;
	}

	TypedValue(std::unordered_map<TypedValue,TypedValue,TvHash> *inKV)
	  :dataType(DataType::kTypeKV) {
		new(&kvPtr) std::shared_ptr<
				std::unordered_map<TypedValue,TypedValue,TvHash> >(inKV);
	}

	TypedValue(File *inFile):dataType(DataType::kTypeFile) {
		new(&filePtr) std::shared_ptr<File>(inFile);
	}

	TypedValue(const Word **inIPValue)
	  :dataType(DataType::kTypeIP),ipValue(inIPValue) {
		// empty
	}

	TypedValue(ControlBlockType inCBT)
	  :dataType(DataType::kTypeCB),intValue(static_cast<int>(inCBT)) {
		// empty
	}

	TypedValue(DataType inParamDestDataType,const Word **inParamDest)
	  :dataType(DataType::kTypeParamDest),ipValue(inParamDest) {
		assert(inParamDestDataType==DataType::kTypeParamDest);
	}

	TypedValue(WordFunc inStdCodePtr)
	  :dataType(DataType::kTypeStdCode),stdCodePtr(inStdCodePtr) {
		// empty
	}
#ifdef _MSVC_VER
	#pragma warning(pop)
#endif

	~TypedValue() {
		if((((int)dataType) & kType_HeavyMask)==0) { return; }
		switch(dataType) {
			case DataType::kTypeSymbol:
			case DataType::kTypeString:
			case DataType::kTypeMayBeAWord:
								stringPtr.reset();	break;

			case DataType::kTypeArray:	arrayPtr.reset();	break;
			case DataType::kTypeList:	listPtr.reset();	break;
			case DataType::kTypeKV:		kvPtr.reset();		break;
			case DataType::kTypeFile:	filePtr.reset();	break;

			case DataType::kTypeBigInt:		delete(bigIntPtr);		break;
			case DataType::kTypeBigFloat:	delete(bigFloatPtr);	break;

			default:
				;	// empty
		}
	}

	inline TypedValue& operator=(const TypedValue& inSrc) {
		if(dataType!=inSrc.dataType) {
			switch(dataType) {
				case DataType::kTypeSymbol:
				case DataType::kTypeString:
				case DataType::kTypeMayBeAWord:
					stringPtr.reset();
					break;

				case DataType::kTypeArray:	arrayPtr.reset();	break;
				case DataType::kTypeList: 	listPtr.reset();	break;
				case DataType::kTypeKV:		kvPtr.reset();		break;
				case DataType::kTypeFile:	filePtr.reset();	break;

				case DataType::kTypeBigInt:		delete(bigIntPtr);		break;
				case DataType::kTypeBigFloat:	delete(bigFloatPtr);	break;

				default:
					;	// dummy
			}

			switch(inSrc.dataType) {
				case DataType::kTypeSymbol:
				case DataType::kTypeString:
				case DataType::kTypeMayBeAWord:
					new(&stringPtr) std::shared_ptr<std::string>(inSrc.stringPtr);
					dataType=inSrc.dataType;
					return *this;

				case DataType::kTypeArray:
					new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
					dataType=DataType::kTypeArray;
					return *this;

				case DataType::kTypeList:
					new(&listPtr)
						std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
					dataType=DataType::kTypeList;
					return *this;

				case DataType::kTypeKV:
					new(&kvPtr)
						std::shared_ptr<
							std::unordered_map<TypedValue,TypedValue,TvHash>
						>(inSrc.kvPtr);
					dataType=DataType::kTypeKV;
					return *this;

				case DataType::kTypeFile:
					new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
					dataType=DataType::kTypeFile;
					return *this;

				case DataType::kTypeBigInt:
					bigIntPtr=new BigInt();
					*bigIntPtr=*inSrc.bigIntPtr;
					dataType=DataType::kTypeBigInt;
					return *this;

				case DataType::kTypeBigFloat:
					bigFloatPtr=new BigFloat();
					*bigFloatPtr=*inSrc.bigFloatPtr;
					dataType=DataType::kTypeBigFloat;
					return *this;

				case DataType::kTypeStdCode:
					dataType=DataType::kTypeStdCode;
					stdCodePtr=inSrc.stdCodePtr;
					return *this;

				default:
					;	// dummy
			}
		}

		switch(inSrc.dataType) {
			case DataType::kTypeInvalid:
			case DataType::kTypeEOC:
			case DataType::kTypeEOF:
				dataType=inSrc.dataType;
				break;

			case DataType::kTypeDirectWord:
			case DataType::kTypeNewWord:
			case DataType::kTypeWord:
				dataType=inSrc.dataType;
				wordPtr=inSrc.wordPtr;
				break;

			case DataType::kTypeBool:
				dataType=inSrc.dataType;
		 		boolValue=inSrc.boolValue;
				break;

			case DataType::kTypeAddress:
			case DataType::kTypeThreshold:
			case DataType::kTypeEmptySlot:
			case DataType::kTypeMiscInt:
			case DataType::kTypeLVOP:
			case DataType::kTypeCB:
			case DataType::kTypeInt:
				dataType=inSrc.dataType;
				intValue=inSrc.intValue;
				break;

			case DataType::kTypeLong:
				dataType=inSrc.dataType;
				longValue=inSrc.longValue;
				break;
			case DataType::kTypeFloat:
				dataType=inSrc.dataType;
				floatValue=inSrc.floatValue;
				break;
			case DataType::kTypeDouble:
				dataType=inSrc.dataType;
				doubleValue=inSrc.doubleValue;
				break;

			case DataType::kTypeBigInt:
				assert(dataType==DataType::kTypeBigInt
					   && inSrc.dataType==DataType::kTypeBigInt);
				*bigIntPtr=*inSrc.bigIntPtr;
				break;

			case DataType::kTypeBigFloat:
				assert(dataType==DataType::kTypeBigFloat
					   && inSrc.dataType==DataType::kTypeBigFloat);
				*bigFloatPtr=*inSrc.bigFloatPtr;
				break;

			case DataType::kTypeParamDest:
			case DataType::kTypeIP:
				dataType=inSrc.dataType;
				ipValue=inSrc.ipValue;
				break;

			case DataType::kTypeSymbol:
			case DataType::kTypeString:
			case DataType::kTypeMayBeAWord:
				dataType=inSrc.dataType;
				stringPtr=inSrc.stringPtr;
				break;

			case DataType::kTypeArray:
				dataType=inSrc.dataType;
				arrayPtr.reset();
				new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
				break;

			case DataType::kTypeList:
				dataType=inSrc.dataType;
				listPtr.reset();
				new(&listPtr)
					std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
				break;

			case DataType::kTypeKV:
				dataType=inSrc.dataType;
				kvPtr.reset();
				new(&kvPtr)
					std::shared_ptr<
						std::unordered_map<TypedValue,TypedValue,TvHash>
					>(inSrc.kvPtr);
				break;

			case DataType::kTypeFile:
				dataType=inSrc.dataType;
				new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
				break;

			case DataType::kTypeStdCode:
				dataType=inSrc.dataType;
				stdCodePtr=inSrc.stdCodePtr;
				break;

			default:
				fprintf(stderr,"SYSTEM ERROR on TypedValue operator=\n");
				fprintf(stderr,"dest type is '%s'\n",GetTypeStr().c_str());
				fprintf(stderr,"src  type is '%s'\n",inSrc.GetTypeStr().c_str());
				exit(-1);
		}
		return *this;
	}

	inline void Set(int inValue) {
		switch(dataType) {
			case DataType::kTypeInvalid:
			case DataType::kTypeInt:
			case DataType::kTypeLong:
			case DataType::kTypeFloat:
			case DataType::kTypeDouble:
			case DataType::kTypeCB:
			case DataType::kTypeStdCode:
				intValue=inValue;
				break;
			case DataType::kTypeBigInt:
				delete(bigIntPtr);
				intValue=inValue;
				break;
			case DataType::kTypeBigFloat:
				delete(bigFloatPtr);
				intValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::kTypeSymbol:
					case DataType::kTypeString:
					case DataType::kTypeMayBeAWord:
						stringPtr.reset();
						break;

					case DataType::kTypeArray:	arrayPtr.reset();	break;
					case DataType::kTypeList: 	listPtr.reset();	break;
					case DataType::kTypeKV:		kvPtr.reset();		break;
					case DataType::kTypeFile:	filePtr.reset();	break;

					default:
						;	// dummy
				}
				intValue=inValue;
		}
		dataType=DataType::kTypeInt;
	}

	inline void Set(float inValue) {
		switch(dataType) {
			case DataType::kTypeInvalid:
			case DataType::kTypeInt:
			case DataType::kTypeLong:
			case DataType::kTypeFloat:
			case DataType::kTypeDouble:
			case DataType::kTypeCB:
			case DataType::kTypeStdCode:
				floatValue=inValue;
				break;
			case DataType::kTypeBigInt:
				delete(bigIntPtr);
				floatValue=inValue;
				break;
			case DataType::kTypeBigFloat:
				delete(bigFloatPtr);
				floatValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::kTypeSymbol:
					case DataType::kTypeString:
					case DataType::kTypeMayBeAWord:
						stringPtr.reset();
						break;

					case DataType::kTypeArray:	arrayPtr.reset();	break;
					case DataType::kTypeList: 	listPtr.reset();	break;
					case DataType::kTypeKV:		kvPtr.reset();		break;
					case DataType::kTypeFile:	filePtr.reset();	break;

					default:
						;	// dummy
				}
				floatValue=inValue;
		}
		dataType=DataType::kTypeFloat;
	}

	inline void Set(double inValue) {
		switch(dataType) {
			case DataType::kTypeInvalid:
			case DataType::kTypeInt:
			case DataType::kTypeLong:
			case DataType::kTypeFloat:
			case DataType::kTypeDouble:
			case DataType::kTypeCB:
			case DataType::kTypeStdCode:
				doubleValue=inValue;
				break;
			case DataType::kTypeBigInt:
				delete(bigIntPtr);
				doubleValue=inValue;
				break;
			case DataType::kTypeBigFloat:
				delete(bigFloatPtr);
				doubleValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::kTypeSymbol:
					case DataType::kTypeString:
					case DataType::kTypeMayBeAWord:
						stringPtr.reset();
						break;

					case DataType::kTypeArray:	arrayPtr.reset();	break;
					case DataType::kTypeList: 	listPtr.reset();	break;
					case DataType::kTypeKV:		kvPtr.reset();		break;
					case DataType::kTypeFile:	filePtr.reset();	break;

					default:
						;	// dummy
				}
				doubleValue=inValue;
		}
		dataType=DataType::kTypeDouble;
	}

	inline size_t Hash() const {
		switch(dataType) {
			case DataType::kTypeInvalid:
			case DataType::kTypeBigInt:
			case DataType::kTypeBigFloat:
			case DataType::kTypeEOC:
			case DataType::kTypeEOF:
				return std::hash<int>()((int)dataType);

			case DataType::kTypeWord:
			case DataType::kTypeDirectWord:
			case DataType::kTypeNewWord:
				return std::hash<const Word*>()(wordPtr);

			case DataType::kTypeIP:
			case DataType::kTypeParamDest:
				return std::hash<const Word**>()(ipValue);

			case DataType::kTypeBool:
				return std::hash<bool>()(boolValue);

			case DataType::kTypeInt:
			case DataType::kTypeThreshold:
			case DataType::kTypeEmptySlot:
			case DataType::kTypeAddress:
			case DataType::kTypeMiscInt:
			case DataType::kTypeLVOP:
			case DataType::kTypeCB:
				return std::hash<int>()(intValue);

			case DataType::kTypeLong:
				return std::hash<long>()(longValue);

			case DataType::kTypeFloat:
				return std::hash<float>()(floatValue);

			case DataType::kTypeDouble:
				return std::hash<double>()(doubleValue);

			case DataType::kTypeString:
			case DataType::kTypeSymbol:
			case DataType::kTypeMayBeAWord:
				return std::hash<std::string>()(*stringPtr);

			case DataType::kTypeStdCode:
				return std::hash<WordFunc>()(stdCodePtr);

			case DataType::kTypeArray: {
					int length=arrayPtr->length;
					size_t hash=std::hash<int>()(length);
					for(int i=0; i<length; i++) {
						hash^=arrayPtr->data[i].Hash();
					}
					return hash;
				}

			case DataType::kTypeList: {
					int length=(int)listPtr->size();
					size_t hash=std::hash<int>()(length);
					for(int i=0; i<length; i++) {
						hash^=listPtr->at(i).Hash();
					}
					return hash;
				}

			case DataType::kTypeKV: {
					size_t hash=std::hash<int>()((int)kvPtr->size());
					for(auto kv : *kvPtr) {
						hash^=kv.first.Hash();
						hash^=kv.second.Hash();
					}
					return hash;
				}

			case DataType::kTypeFile:
				return std::hash<File*>()(filePtr.get());
			default:
				fprintf(stderr, "SYSTEM ERROR: TypedValue::Hash().");
				exit(-1);
		}
	}

	inline bool IsInvalid() const { return dataType==DataType::kTypeInvalid; }

	inline bool HasWordPtr() const {
		return dataType==DataType::kTypeWord
			|| dataType==DataType::kTypeDirectWord
			|| dataType==DataType::kTypeNewWord;
	}

	inline bool IsNumber() const {
		return dataType==DataType::kTypeInt
			|| dataType==DataType::kTypeLong
			|| dataType==DataType::kTypeFloat
			|| dataType==DataType::kTypeDouble
			|| dataType==DataType::kTypeBigInt
			|| dataType==DataType::kTypeBigFloat;
	}

	int GetLevel() const;

	PP_API double ToDouble(Context& inContext);
	PP_API float  ToFloat(Context& inContext);

	PP_API std::string GetTypeStr() const;

	PP_API void PrintValue(int inIndent=0) const;
	PP_API std::string GetEscapedValueString(int inIndent=-1) const;
	PP_API std::string GetValueString(int inIndent=-1) const;

	PP_API void Dump() const;
};

inline bool operator==(const TypedValue& inTV1,const TypedValue& inTV2) {
	if(inTV1.dataType!=inTV2.dataType) { return false; }

	switch(inTV1.dataType) {
		case DataType::kTypeInvalid:
		case DataType::kTypeEOC:
		case DataType::kTypeEOF:
			return true;

		case DataType::kTypeParamDest:
		case DataType::kTypeIP:
			return inTV1.ipValue==inTV2.ipValue;

		case DataType::kTypeDirectWord:
		case DataType::kTypeNewWord:
			return inTV1.wordPtr==inTV2.wordPtr;

		case DataType::kTypeBool:
		case DataType::kTypeEmptySlot:
			return inTV1.boolValue==inTV2.boolValue;

		case DataType::kTypeAddress:
		case DataType::kTypeThreshold:
		case DataType::kTypeMiscInt:
		case DataType::kTypeLVOP:
		case DataType::kTypeCB:
		case DataType::kTypeInt: return inTV1.intValue==inTV2.intValue;

		case DataType::kTypeLong:		return inTV1.longValue   ==inTV2.longValue;
		case DataType::kTypeBigInt:		return *inTV1.bigIntPtr  ==*inTV2.bigIntPtr;
		case DataType::kTypeFloat: 		return inTV1.floatValue  ==inTV2.floatValue;
		case DataType::kTypeDouble:		return inTV1.doubleValue ==inTV2.doubleValue;
		case DataType::kTypeBigFloat:	return *inTV1.bigFloatPtr==*inTV2.bigFloatPtr;

		case DataType::kTypeSymbol:
		case DataType::kTypeString:
		case DataType::kTypeMayBeAWord:
							return *inTV1.stringPtr.get()==*inTV2.stringPtr.get();

		case DataType::kTypeArray:
			if(!(inTV1.arrayPtr->length==inTV2.arrayPtr->length)) {
				return false;
			}
			for(int i=0; i<inTV1.arrayPtr->length; i++) {
				if(!(inTV1.arrayPtr->data[i]==inTV2.arrayPtr->data[i])) {
					return false;
				}
			}
			return true;

		case DataType::kTypeList:
			if(inTV1.listPtr->size()!=inTV2.listPtr->size()) {
				return false;
			}
			{
				const size_t n=inTV1.listPtr->size();
				for(size_t i=0; i<n; i++) {
					if(!(inTV1.listPtr->at(i)==inTV2.listPtr->at(i))) {
						return false;
					}
				}
			}
			return true;

		case DataType::kTypeKV:
			return (*inTV1.kvPtr.get())==(*inTV2.kvPtr.get());

		case DataType::kTypeFile:
			return inTV1.filePtr.get()==inTV2.filePtr.get();

		case DataType::kTypeStdCode:
			return inTV1.stdCodePtr==inTV2.stdCodePtr;

		default:
			fprintf(stderr,"SYSTEM ERROR TypedValue Op==\n");
			exit(-1);
	}
}

inline std::size_t TypedValue::TvHash::operator() (const TypedValue& inTV) const {
	return inTV.Hash();
}

typedef std::deque<TypedValue> List;
typedef std::unordered_map<TypedValue,TypedValue,TypedValue::TvHash> KeyValue;

#define TvSize (sizeof(TypedValue)%sizeof(Word*)==0 \
					? sizeof(TypedValue)/sizeof(Word*) \
					: sizeof(TypedValue)/sizeof(Word*)+1)

#define MutexSize (sizeof(Mutex)%sizeof(Word*)==0 \
					? sizeof(Mutex)/sizeof(Word*) : sizeof(Mutex)/sizeof(Word*)+1)

PP_API TypedValue FullClone(TypedValue& inTV);

