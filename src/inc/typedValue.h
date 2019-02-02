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
#include "outer.h"

typedef bool (*WordFunc)(Context& inContext) NOEXCEPT;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::multiprecision::cpp_dec_float_100 BigFloat;

enum DataType {
	kTypeInvalid,	// no values
	kTypeDirectWord,
	kTypeIP,
	kTypeNewWord,	// wordPtr value as NewWord.
	kTypeThreshold,	// intValue as ExecutionThreshold
	kTypeEmptySlot,	// use intValue as execution-threshld level backup.
	kTypeAddress,	// intValue as an address.
	kTypeParamDest,	// ipValue's vallue as a pointer to param[target]
	//kTypeEnvFrame,

	kTypeBool,
	kTypeInt,
	kTypeLong,
	kTypeBigInt,
	kTypeFloat,
	kTypeDouble,
	kTypeBigFloat,
	kTypeString,
	kTypeWord,
	kTypeArray,
	kTypeList,
	//kTypeMap,
	kTypeSymbol,
	kTypeEOC,	// no values
	kTypeFile,
	kTypeEOF,	// no values

	kTypeMiscInt,	// use intValue as misc. data.
};

// int value for kTypeMiscInt
enum ControlBlockType {
	kOPEN_C_STYLE_COMMENT			=-1000,
	kOPEN_CPP_STYLE_ONE_LINE_COMMENT=-1001,

	kSyntax_FOR_PLUS 	=-2000,
	kSyntax_FOR_MINUS	=-2001,
	kSyntax_WHILE	 	=-2010,
	kSyntax_WHILE_COND	=-2011,
	kSyntax_DO			=-2020,
	kSyntax_IF		 	=-2100,
	kSyntax_SWITCH	 	=-2110,
};

struct TypedValue {
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
		//std::unordered_map<TypedValue,TypedValue> mapValue;
		std::shared_ptr<File> filePtr;

		BigInt *bigIntPtr;
		BigFloat *bigFloatPtr;
	};

	TypedValue():dataType(kTypeInvalid) {
		// empty
	}

	TypedValue(DataType inDataType):dataType(inDataType) {
		assert(inDataType==kTypeEOC || inDataType==kTypeEOF);
	}

	inline TypedValue(const TypedValue& inSrc)
	  :dataType(inSrc.dataType) {
		switch(inSrc.dataType) {
			case kTypeInvalid:
			case kTypeEOC:
			case kTypeEOF:
				break;

			case kTypeDirectWord:
			case kTypeNewWord:
			case kTypeWord:
				wordPtr=inSrc.wordPtr;
				break;

			case kTypeBool:
				boolValue=inSrc.boolValue;
				break;

			case kTypeAddress:
			case kTypeThreshold:
			case kTypeEmptySlot:
			case kTypeMiscInt:
			case kTypeInt:		intValue=inSrc.intValue;		 break;

			case kTypeBigInt:
				bigIntPtr=new BigInt();
				*bigIntPtr=*inSrc.bigIntPtr;
				break;

			case kTypeBigFloat:
				bigFloatPtr=new BigFloat();
				*bigFloatPtr=*inSrc.bigFloatPtr;
				break;

			case kTypeLong:		longValue=inSrc.longValue;		break;
			case kTypeFloat:	floatValue=inSrc.floatValue;	break;
			case kTypeDouble:	doubleValue=inSrc.doubleValue;	break;

			case kTypeString:
			case kTypeSymbol:
				new(&stringPtr) std::shared_ptr<std::string>(inSrc.stringPtr);
				break;

			case kTypeArray:
				new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
				break;

			case kTypeList:
				new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
				break;

			case kTypeFile:
				new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
				break;

			case kTypeParamDest:
			case kTypeIP:	ipValue=inSrc.ipValue;	break;

			default:
				fprintf(stderr,"SYSTEM ERROR at TypedValue copy constructor.\n");
				exit(-1);
		}
	}

	TypedValue(const Word *inWordPtr)
	  :dataType(kTypeDirectWord),wordPtr(inWordPtr) {
		// empty
	}
	
	TypedValue(DataType inDataType,const Word *inWordPtr)
	  :dataType(inDataType),wordPtr(inWordPtr) {
		assert(inDataType==kTypeNewWord || inDataType==kTypeWord);
	}

	TypedValue(bool inBoolValue)
	  :dataType(kTypeBool),boolValue(inBoolValue) {
		// empty
	}

	TypedValue(DataType inDataType,int inIntValue)
	  :dataType(inDataType),intValue(inIntValue) {
		assert(inDataType==kTypeEmptySlot
			|| inDataType==kTypeThreshold
			|| inDataType==kTypeAddress
			|| inDataType==kTypeMiscInt);
	}

	TypedValue(int inIntValue):dataType(kTypeInt),intValue(inIntValue) {
		// empty
	}

	TypedValue(long inLongValue):dataType(kTypeLong),longValue(inLongValue) {
		// empty
	}

	TypedValue(const BigInt& inBigInt)
   	  :dataType(kTypeBigInt),bigIntPtr(new BigInt(inBigInt)) {
		// empty
	}

	TypedValue(float inFloatValue):dataType(kTypeFloat) {
		floatValue=inFloatValue;
	}

	TypedValue(double inDoubleValue):dataType(kTypeDouble) {
		doubleValue=inDoubleValue;
	}

	TypedValue(const BigFloat& inBigFloat)
	  :dataType(kTypeBigFloat),bigFloatPtr(new BigFloat(inBigFloat)) {
		// empty
	}

	TypedValue(const std::string& inStringValue)
	  :dataType(kTypeString),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(const std::string& inStringValue,DataType /* kTypeSymbol */)
	  :dataType(kTypeSymbol),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(Array<TypedValue> *inArray):dataType(kTypeArray) {
		new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inArray);
	}

	TypedValue(std::deque<TypedValue> *inList):dataType(kTypeList) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inList);
	}

	TypedValue(std::shared_ptr< std::deque<TypedValue> > inList)
	  :dataType(kTypeList) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >();
		listPtr=inList;
	}

	TypedValue(File *inFile):dataType(kTypeFile) {
		new(&filePtr) std::shared_ptr<File>(inFile);
	}

	TypedValue(const Word **inIPValue)
	  :dataType(kTypeIP),ipValue(inIPValue) {
		// empty
	}

	TypedValue(DataType inParamDestDataType,const Word **inParamDest)
	  :dataType(kTypeParamDest),ipValue(inParamDest) {
		assert(inParamDestDataType==kTypeParamDest);
	}

	~TypedValue() {
		switch(dataType) {
			case kTypeSymbol:
			case kTypeString:	stringPtr.reset();	break;

			case kTypeArray:	arrayPtr.reset();	break;
			case kTypeList:		listPtr.reset();	break;
			case kTypeFile:		filePtr.reset();	break;

			case kTypeBigInt:	delete(bigIntPtr);		break;
			case kTypeBigFloat:	delete(bigFloatPtr);	break;

			default:
				;	// empty
		}
	}

	inline TypedValue& operator=(const TypedValue& inSrc) {
		if(dataType!=inSrc.dataType) {
			switch(dataType) {
				case kTypeSymbol:
				case kTypeString:
					stringPtr.reset();
					break;

				case kTypeArray:	arrayPtr.reset();	break;
				case kTypeList: 	listPtr.reset();	break;
				case kTypeFile:		filePtr.reset();	break;

				case kTypeBigInt:	delete(bigIntPtr);		break;
				case kTypeBigFloat:	delete(bigFloatPtr);	break;

				default:
					;	// dummy
			}

			switch(inSrc.dataType) {
				case kTypeSymbol:
				case kTypeString:
					new(&stringPtr) std::shared_ptr<std::string>(
															inSrc.stringPtr);
					dataType=inSrc.dataType;
					return *this;

				case kTypeArray:
					new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(
															inSrc.arrayPtr);
					dataType=kTypeArray;
					return *this;

				case kTypeList:
					new(&listPtr)
						std::shared_ptr< std::deque<TypedValue> >(
															inSrc.listPtr);
					dataType=kTypeList;
					return *this;

				case kTypeFile:
					new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
					dataType=kTypeFile;
					return *this;

				case kTypeBigInt:
					bigIntPtr=new BigInt();
					*bigIntPtr=*inSrc.bigIntPtr;
					dataType=kTypeBigInt;
					return *this;

				case kTypeBigFloat:
					bigFloatPtr=new BigFloat();
					*bigFloatPtr=*inSrc.bigFloatPtr;
					dataType=kTypeBigFloat;
					return *this;

				default:
					;	// dummy
			}
		}

		switch(inSrc.dataType) {
			case kTypeInvalid:
			case kTypeEOC:
			case kTypeEOF:
				dataType=inSrc.dataType;
				break;

			case kTypeDirectWord:
			case kTypeNewWord:
			case kTypeWord:
				dataType=inSrc.dataType;
				wordPtr=inSrc.wordPtr;
				break;

			case kTypeBool:
				dataType=inSrc.dataType;
		 		boolValue=inSrc.boolValue;
				break;

			case kTypeAddress:
			case kTypeThreshold:
			case kTypeEmptySlot:
			case kTypeMiscInt:
			case kTypeInt:
				dataType=inSrc.dataType;
				intValue=inSrc.intValue;
				break;

			case kTypeLong:
				dataType=inSrc.dataType;
				longValue=inSrc.longValue;
				break;
			case kTypeFloat:
				dataType=inSrc.dataType;
				floatValue=inSrc.floatValue;
				break;
			case kTypeDouble:
				dataType=inSrc.dataType;
				doubleValue=inSrc.doubleValue;
				break;

			case kTypeBigInt:
				assert(dataType==kTypeBigInt && inSrc.dataType==kTypeBigInt);
				*bigIntPtr=*inSrc.bigIntPtr;
				break;

			case kTypeBigFloat:
				assert(dataType==kTypeBigFloat && inSrc.dataType==kTypeBigFloat);
				*bigFloatPtr=*inSrc.bigFloatPtr;
				break;

			case kTypeParamDest:
			case kTypeIP:
				dataType=inSrc.dataType;
				ipValue=inSrc.ipValue;
				break;

			case kTypeSymbol:
			case kTypeString:
				dataType=inSrc.dataType;
				stringPtr=inSrc.stringPtr;
				break;

			case kTypeList:
				dataType=inSrc.dataType;
				listPtr.reset();
				new(&listPtr)
					std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
				break;

			default:
				fprintf(stderr,"SYSTEM ERROR on TypedValue operator=\n");
				fprintf(stderr,"dest type is '%s'\n",GetTypeStr().c_str());
				fprintf(stderr,"src  type is '%s'\n",inSrc.GetTypeStr().c_str());
				exit(-1);
		}
		return *this;
	}

	inline bool IsInvalid() { return dataType==kTypeInvalid; }

	int GetLevel() const;

	PP_API double ToDouble(Context& inContext);
	PP_API float  ToFloat(Context& inContext);

	PP_API std::string GetTypeStr() const;

	PP_API void PrintValue(int inIndent=0) const;
	PP_API std::string GetValueString(int inIndent) const;

	PP_API void Dump() const;
};

inline bool operator==(const TypedValue& inTV1,const TypedValue& inTV2) {
	if(inTV1.dataType!=inTV2.dataType) { return false; }

	switch(inTV1.dataType) {
		case kTypeInvalid:
		case kTypeEOC:
		case kTypeEOF:
			return true;

		case kTypeParamDest:
		case kTypeIP:
			return inTV1.ipValue==inTV2.ipValue;

		case kTypeDirectWord:
		case kTypeNewWord:
			return inTV1.wordPtr==inTV2.wordPtr;

		case kTypeBool:
		case kTypeEmptySlot:
			return inTV1.boolValue==inTV2.boolValue;

		case kTypeAddress:
		case kTypeThreshold:
		case kTypeMiscInt:
		case kTypeInt: return inTV1.intValue==inTV2.intValue;

		case kTypeLong:		return inTV1.longValue==inTV2.longValue;
		case kTypeBigInt:	return *inTV1.bigIntPtr==*inTV2.bigIntPtr;
		case kTypeFloat: 	return inTV1.floatValue==inTV2.floatValue;
		case kTypeDouble:	return inTV1.doubleValue==inTV2.doubleValue;
		case kTypeBigFloat:	return *inTV1.bigFloatPtr==*inTV2.bigFloatPtr;

		case kTypeSymbol:
		case kTypeString:	return *inTV1.stringPtr.get()==*inTV2.stringPtr.get();

		case kTypeArray:
			if(!(inTV1.arrayPtr->length==inTV2.arrayPtr->length)) {
				return false;
			}
			for(int i=0; i<inTV1.arrayPtr->length; i++) {
				if(!(inTV1.arrayPtr->data[i]==inTV2.arrayPtr->data[i])) {
					return false;
				}
			}
			return true;

		case kTypeList:
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

		case kTypeFile:
			return inTV1.filePtr.get()==inTV2.filePtr.get();

		default:
			fprintf(stderr,"SYSTEM ERROR TypedValue Op==\n");
			exit(-1);
	}
}

typedef std::deque<TypedValue> List;

#define TvSize (sizeof(TypedValue)%sizeof(Word*)==0 \
					? sizeof(TypedValue)/sizeof(Word*) \
					: sizeof(TypedValue)/sizeof(Word*)+1)

#define MutexSize (sizeof(Mutex)%sizeof(Word*)==0 \
					? sizeof(Mutex)/sizeof(Word*) : sizeof(Mutex)/sizeof(Word*)+1)

