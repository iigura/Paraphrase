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
struct ChanMan;

#include "thread.h"
#include "paraphrase.h"
#include "array.h"
#include "file.h"
#include "lvop.h"

typedef bool (*WordFunc)(Context& inContext) NOEXCEPT;
typedef boost::multiprecision::cpp_int BigInt;

enum class Level;	// see context.h

const int kType_HeavyMask=0x4000;
enum class DataType {
	Invalid	=999,	// no values
	MayBeAWord =kType_HeavyMask | 998,	// for args, use stringPtr.
	DirectWord	=900,
	IP			=910,
	NewWord		=920,	// wordPtr value as NewWord.
	Threshold	=930,	// intValue as ExecutionThreshold
	EmptySlot	=940,	// use intValue as execution-threshld level backup.
	Address		=950,	// intValue as an address.
	ParamDest	=960,	// ipValue's vallue as a pointer to param[target]
	Lambda		=kType_HeavyMask | 970,
	//kTypeEnvFrame,

	Bool	=10,
	Int	 	=0,
	Long	=1,
	BigInt	=kType_HeavyMask | 2,
	Float	=3,
	Double	=4,

	String=kType_HeavyMask | 20,
	Symbol=kType_HeavyMask | 21,

	Word=30,

	Array=kType_HeavyMask | 41,
	List =kType_HeavyMask | 42,
	KV   =kType_HeavyMask | 43,

	EoC =50,	// no values

	File=kType_HeavyMask | 60,
	EoF	=61,	// no values

	Context=kType_HeavyMask | 70,
	Channel=kType_HeavyMask | 75,

	StdCode=80,	// use stdCodePtr

	UserData=kType_HeavyMask | 99,

	MiscInt=800,	// use intValue as misc. data.
	LVOP   =810,	// use intValue as LVOP.
	CB 	   =820,	// use intValue as CB (Control Block) info.
};

inline int GetMathOpType(DataType inType1,DataType inType2) {
	return (((int)inType1)<<16) | ((int)inType2);
}

#define MathOpCombi(s,t) ((((int)s)<<16) | ((int)t))

enum class TypeCombinationForMathOP {
	IntInt 		=MathOpCombi(DataType::Int,DataType::Int),
	IntLong		=MathOpCombi(DataType::Int,DataType::Long),
	IntBigInt	=MathOpCombi(DataType::Int,DataType::BigInt),
	IntFloat	=MathOpCombi(DataType::Int,DataType::Float),
	IntDouble	=MathOpCombi(DataType::Int,DataType::Double),

	LongInt	 	 =MathOpCombi(DataType::Long,DataType::Int),
	LongLong	 =MathOpCombi(DataType::Long,DataType::Long),
	LongBigInt	 =MathOpCombi(DataType::Long,DataType::BigInt),
	LongFloat	 =MathOpCombi(DataType::Long,DataType::Float),
	LongDouble	 =MathOpCombi(DataType::Long,DataType::Double),

	BigIntInt   	=MathOpCombi(DataType::BigInt,DataType::Int),
   	BigIntLong  	=MathOpCombi(DataType::BigInt,DataType::Long),
	BigIntBigInt	=MathOpCombi(DataType::BigInt,DataType::BigInt),
	BigIntFloat 	=MathOpCombi(DataType::BigInt,DataType::Float),
	BigIntDouble	=MathOpCombi(DataType::BigInt,DataType::Double),

	FloatInt   		=MathOpCombi(DataType::Float,DataType::Int),
	FloatLong  		=MathOpCombi(DataType::Float,DataType::Long),
   	FloatBigInt  	=MathOpCombi(DataType::Float,DataType::BigInt),
	FloatFloat		=MathOpCombi(DataType::Float,DataType::Float),
	FloatDouble		=MathOpCombi(DataType::Float,DataType::Double),

	DoubleInt  		=MathOpCombi(DataType::Double,DataType::Int),
	DoubleLong  	=MathOpCombi(DataType::Double,DataType::Long),
	DoubleBigInt  	=MathOpCombi(DataType::Double,DataType::BigInt),
	DoubleFloat		=MathOpCombi(DataType::Double,DataType::Float),
	DoubleDouble	=MathOpCombi(DataType::Double,DataType::Double),

	kInvalidMathOpTypeThreshold=BigIntBigInt+1,
};

// int value for DataType::MiscInt
enum class ControlBlockType {
	kGROUP_MASK=0xFF00,

	kOPEN_COMMENT_GROUP=0x0100,
	OpenCStyleComment		  =kOPEN_COMMENT_GROUP | 0x01,
	OpenCppStyleOneLineComment=kOPEN_COMMENT_GROUP | 0x02,

	kOPEN_IF_GROUP=0x0200,
	SyntaxIf=kOPEN_IF_GROUP | 0x01,

	kOPEN_LEAVABLE_LOOP_GROUP=0x0400,
	SyntaxForPlus 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x01,
	SyntaxForMinus	=kOPEN_LEAVABLE_LOOP_GROUP | 0x02,
	SyntaxWhile	 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x11,
	SyntaxLoop	 	=kOPEN_LEAVABLE_LOOP_GROUP | 0x12,

	kOPEN_SWITCH_GROUP=0x0800,
	SyntaxSwitch=kOPEN_SWITCH_GROUP | 0x01,
	SyntaxCond	=kOPEN_SWITCH_GROUP | 0x02,

	kOPEN_HERE_DOCUMENT_GROUP=0x1000,
	OpenHereDocumentRaw   =kOPEN_HERE_DOCUMENT_GROUP | 0x01,
	OpenHereDocumentDedent=kOPEN_HERE_DOCUMENT_GROUP | 0x02,

	kOPEN_ARG_GROUP=0x2000,
	SyntaxArgBegin=kOPEN_ARG_GROUP | 0x01,

	kOPEN_LAMBDA_GROUP=0x4000,
	NewSimpleLambda						=kOPEN_LAMBDA_GROUP | 0x01,

	kTHREAD=0x8000 | kOPEN_LAMBDA_GROUP,
	kTHREAD_INFO_MASK=0xFF,
	kThreadMask_Open_ThreadOutputAsInput	=0x01,	// >[  , >[[
	kThreadMask_Open_TosChanAsInput			=0x02,	// >>[ , >>[[
	kThreadMask_Open_TosBroadcast			=0x04,	// [-> , [[->
	kThreadMask_Open_PushTosListElem		=0x08,	// [=> , [[=>
	kThreadMask_Close_ThreadOutputAsInput	=0x10,	// ]>  , ]]>
	kThreadMask_Close_PushOutputChan		=0x20,	// ]>> , ]]>>
	kThreadMask_Close_PushContext			=0x40,	// ]=  , ]]=
	//  [ ... ]  kTHREAD_INFO
	// >[ ... ]  kTHREAD_INFO | kThreadMask_Open_ThreadOutputAsInput
	// >[ ... ]> kTHREAD_INFO | kThreadMask_Open_ThreadOutputAsInput | kThreadMask_Close_ThreadOutputAsInput
	
	OpenSimpleThread 	    = kTHREAD,	// [ , [[
	OpenConnectedThread     = kTHREAD | kThreadMask_Open_ThreadOutputAsInput, // >[ , >[[
	OpenTosChanConnectThread= kTHREAD | kThreadMask_Open_TosChanAsInput // >>[ , >>[[
};

struct UserData {
	void *dataPtr;
	std::string memo;

	UserData():dataPtr(NULL),memo("") { /* empty */ }
	UserData(void *inDataPtr,std::string inMemo)
	  :dataPtr(inDataPtr),memo(inMemo) { /* empty */ }
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

		std::shared_ptr<const Word> lambdaPtr;
		std::shared_ptr<std::string> stringPtr;
		std::shared_ptr< Array<TypedValue> > arrayPtr;
		std::shared_ptr< std::deque<TypedValue> > listPtr;
		std::shared_ptr< std::unordered_map<TypedValue,TypedValue,TvHash> > kvPtr;
		std::shared_ptr<File> filePtr;
		std::shared_ptr<Context> contextPtr;
		std::shared_ptr<ChanMan> channelPtr;

		BigInt *bigIntPtr;

		WordFunc stdCodePtr;

		std::shared_ptr<UserData> userDataPtr;
	};

#ifdef _MSVC_LANG
	#pragma warning(push)
	#pragma warning(disable:26495)
#endif
	TypedValue():dataType(DataType::Invalid) { /* empty */ }

	TypedValue(DataType inDataType):dataType(inDataType) {
		assert(inDataType==DataType::EoC || inDataType==DataType::EoF);
	}

	inline TypedValue(const TypedValue& inSrc)
	  :dataType(inSrc.dataType) {
		if(((int)inSrc.dataType & kType_HeavyMask)==0) {
			// simple memcpy for primitive data.
			// note: double is a most largest primitive.
			doubleValue=inSrc.doubleValue;
		} else {
			switch(inSrc.dataType) {
				case DataType::Lambda:
					new(&lambdaPtr) std::shared_ptr<const Word>(inSrc.lambdaPtr);
					break;

				case DataType::BigInt:
					bigIntPtr=new BigInt();
					*bigIntPtr=*inSrc.bigIntPtr;
					break;

				case DataType::String:
				case DataType::Symbol:
				case DataType::MayBeAWord:
					new(&stringPtr) std::shared_ptr<std::string>(inSrc.stringPtr);
					break;

				case DataType::Array:
					new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
					break;

				case DataType::List:
					new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
					break;

				case DataType::KV:
					new(&kvPtr)
						std::shared_ptr<
							std::unordered_map<TypedValue,TypedValue,TvHash>
						>(inSrc.kvPtr);
					break;

				case DataType::File:
					new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
					break;

				case DataType::UserData:
					new(&userDataPtr) std::shared_ptr<UserData>(inSrc.userDataPtr);
					break;

				case DataType::Context:
					new(&contextPtr) std::shared_ptr<Context>(inSrc.contextPtr);
					break;

				case DataType::Channel:
					new(&channelPtr) std::shared_ptr<ChanMan>(inSrc.channelPtr);
					break;
					
				default:
					fprintf(stderr,"SYSTEM ERROR at TypedValue copy constructor.\n");
					exit(-1);
			}
		}
	}

	TypedValue(const Word *inWordPtr)
	  :dataType(DataType::DirectWord),wordPtr(inWordPtr) {
		// empty
	}

	TypedValue(DataType inDataType,const Word *inWordPtr)
	  :dataType(inDataType),wordPtr(inWordPtr) {
		assert(inDataType==DataType::NewWord || inDataType==DataType::Word);
	}

	TypedValue(std::shared_ptr<const Word> inLambdaPtr):dataType(DataType::Lambda) {
		new(&lambdaPtr) std::shared_ptr<const Word>(inLambdaPtr);
	}

	TypedValue(bool inBoolValue)
	  :dataType(DataType::Bool),boolValue(inBoolValue) {
		// empty
	}

	TypedValue(DataType inDataType,int inIntValue)
	  :dataType(inDataType),intValue(inIntValue) {
		assert(inDataType==DataType::EmptySlot
			|| inDataType==DataType::Address);
	}

	TypedValue(Level inLevel)
	  :dataType(DataType::Threshold),intValue((int)inLevel) {
		// empty
	}

	TypedValue(DataType inDataType,ControlBlockType inCBT)
	  :dataType(DataType::MiscInt),intValue((int)inCBT) {
		assert(inDataType==DataType::MiscInt);
	}

	TypedValue(DataType inDataType,Level inLevel)
	  :dataType(DataType::EmptySlot),intValue((int)inLevel) {
		assert(inDataType==DataType::EmptySlot);
	}

	TypedValue(LVOP inLVOP)
	  :dataType(DataType::LVOP),intValue(static_cast<int>(inLVOP)) {
		// empty
	}

	TypedValue(int inIntValue):dataType(DataType::Int),intValue(inIntValue) {
		// empty
	}

	TypedValue(long inLongValue):dataType(DataType::Long),longValue(inLongValue) {
		// empty
	}

	TypedValue(const BigInt& inBigInt)
   	  :dataType(DataType::BigInt),bigIntPtr(new BigInt(inBigInt)) {
		// empty
	}

	TypedValue(float inFloatValue):dataType(DataType::Float) {
		floatValue=inFloatValue;
	}

	TypedValue(double inDoubleValue):dataType(DataType::Double) {
		doubleValue=inDoubleValue;
	}

	TypedValue(const std::string& inStringValue)
	  :dataType(DataType::String),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(const std::string& inStringValue,DataType /* DataType::Symbol */)
	  :dataType(DataType::Symbol),stringPtr(new std::string(inStringValue)) {
		// empty
	}

	TypedValue(Array<TypedValue> *inArray):dataType(DataType::Array) {
		new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inArray);
	}

	TypedValue(std::deque<TypedValue> *inList):dataType(DataType::List) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >(inList);
	}

	TypedValue(std::shared_ptr< std::deque<TypedValue> > inList)
	  :dataType(DataType::List) {
		new(&listPtr) std::shared_ptr< std::deque<TypedValue> >();
		listPtr=inList;
	}

	TypedValue(std::unordered_map<TypedValue,TypedValue,TvHash> *inKV)
	  :dataType(DataType::KV) {
		new(&kvPtr) std::shared_ptr<
				std::unordered_map<TypedValue,TypedValue,TvHash> >(inKV);
	}

	TypedValue(File *inFile):dataType(DataType::File) {
		new(&filePtr) std::shared_ptr<File>(inFile);
	}

	PP_API TypedValue(Context *inContextPtr);
	PP_API TypedValue(ChanMan *inChannel);
	TypedValue(std::shared_ptr<ChanMan> inChannel):dataType(DataType::Channel) {
		new(&channelPtr) std::shared_ptr<ChanMan>(inChannel);
	}

	TypedValue(const Word **inIPValue):dataType(DataType::IP),ipValue(inIPValue) {
		// empty
	}

	TypedValue(ControlBlockType inCBT)
	  :dataType(DataType::CB),intValue(static_cast<int>(inCBT)) {
		// empty
	}

	TypedValue(DataType inParamDestDataType,const Word **inParamDest)
	  :dataType(DataType::ParamDest),ipValue(inParamDest) {
		assert(inParamDestDataType==DataType::ParamDest);
	}

	TypedValue(WordFunc inStdCodePtr)
	  :dataType(DataType::StdCode),stdCodePtr(inStdCodePtr) {
		// empty
	}

	TypedValue(void *inUserDataPtr,std::string inMemo)
	  :dataType(DataType::UserData),userDataPtr(new UserData(inUserDataPtr,inMemo)) {
		// empty
	}
#ifdef _MSVC_VER
	#pragma warning(pop)
#endif

	~TypedValue() {
		if((((int)dataType) & kType_HeavyMask)==0) { return; }
		switch(dataType) {
			case DataType::Lambda:	lambdaPtr.reset();	break;

			case DataType::Symbol:
			case DataType::String:
			case DataType::MayBeAWord:
				stringPtr.reset();	break;

			case DataType::Array:	arrayPtr.reset();	break;
			case DataType::List:	listPtr.reset();	break;
			case DataType::KV:		kvPtr.reset();		break;
			case DataType::File:	filePtr.reset();	break;
			case DataType::Context:	contextPtr.reset(); break;
			case DataType::Channel:	channelPtr.reset();	break;

			case DataType::BigInt:		delete(bigIntPtr);		break;

			case DataType::UserData:	userDataPtr.reset();	break;

			default:
				;	// empty
		}
	}

	inline TypedValue& operator=(const TypedValue& inSrc) {
		if(dataType!=inSrc.dataType) {
			switch(dataType) {
				case DataType::Lambda:	lambdaPtr.reset();	break;

				case DataType::Symbol:
				case DataType::String:
				case DataType::MayBeAWord:
					stringPtr.reset();
					break;

				case DataType::Array:	arrayPtr.reset();	break;
				case DataType::List: 	listPtr.reset();	break;
				case DataType::KV:		kvPtr.reset();		break;
				case DataType::File:	filePtr.reset();	break;
				case DataType::Context:	contextPtr.reset();	break;
				case DataType::Channel: channelPtr.reset(); break;

				case DataType::BigInt:		delete(bigIntPtr);		break;

				case DataType::UserData:	userDataPtr.reset();	break;

				default:
					;	// dummy
			}

			switch(inSrc.dataType) {
				case DataType::Lambda:
					new(&lambdaPtr) std::shared_ptr<const Word>(inSrc.lambdaPtr);
					dataType=inSrc.dataType;
					return *this;

				case DataType::Symbol:
				case DataType::String:
				case DataType::MayBeAWord:
					new(&stringPtr) std::shared_ptr<std::string>(inSrc.stringPtr);
					dataType=inSrc.dataType;
					return *this;

				case DataType::Array:
					new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
					dataType=DataType::Array;
					return *this;

				case DataType::List:
					new(&listPtr)
						std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
					dataType=DataType::List;
					return *this;

				case DataType::KV:
					new(&kvPtr)
						std::shared_ptr<
							std::unordered_map<TypedValue,TypedValue,TvHash>
						>(inSrc.kvPtr);
					dataType=DataType::KV;
					return *this;

				case DataType::File:
					new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
					dataType=DataType::File;
					return *this;

				case DataType::Context:
					new(&contextPtr) std::shared_ptr<Context>(inSrc.contextPtr);
					dataType=DataType::Context;
					return *this;

				case DataType::Channel:
					new(&channelPtr) std::shared_ptr<ChanMan>(inSrc.channelPtr);
					dataType=DataType::Channel;
					return *this;

				case DataType::BigInt:
					bigIntPtr=new BigInt();
					*bigIntPtr=*inSrc.bigIntPtr;
					dataType=DataType::BigInt;
					return *this;

				case DataType::StdCode:
					dataType=DataType::StdCode;
					stdCodePtr=inSrc.stdCodePtr;
					return *this;

				case DataType::UserData:
					new(&userDataPtr) std::shared_ptr<UserData>(inSrc.userDataPtr);
					dataType=DataType::UserData;
					return *this;

				default:
					;	// dummy
			}
		}

		switch(inSrc.dataType) {
			case DataType::Invalid:
			case DataType::EoC:
			case DataType::EoF:
				dataType=inSrc.dataType;
				break;

			case DataType::DirectWord:
			case DataType::NewWord:
			case DataType::Word:
				dataType=inSrc.dataType;
				wordPtr=inSrc.wordPtr;
				break;

			case DataType::Lambda:
				dataType=inSrc.dataType;
				lambdaPtr=inSrc.lambdaPtr;
				break;

			case DataType::Bool:
				dataType=inSrc.dataType;
		 		boolValue=inSrc.boolValue;
				break;

			case DataType::Address:
			case DataType::Threshold:
			case DataType::EmptySlot:
			case DataType::MiscInt:
			case DataType::LVOP:
			case DataType::CB:
			case DataType::Int:
				dataType=inSrc.dataType;
				intValue=inSrc.intValue;
				break;

			case DataType::Long:
				dataType=inSrc.dataType;
				longValue=inSrc.longValue;
				break;
			case DataType::Float:
				dataType=inSrc.dataType;
				floatValue=inSrc.floatValue;
				break;
			case DataType::Double:
				dataType=inSrc.dataType;
				doubleValue=inSrc.doubleValue;
				break;

			case DataType::BigInt:
				assert(dataType==DataType::BigInt
					   && inSrc.dataType==DataType::BigInt);
				*bigIntPtr=*inSrc.bigIntPtr;
				break;

			case DataType::ParamDest:
			case DataType::IP:
				dataType=inSrc.dataType;
				ipValue=inSrc.ipValue;
				break;

			case DataType::Symbol:
			case DataType::String:
			case DataType::MayBeAWord:
				dataType=inSrc.dataType;
				stringPtr=inSrc.stringPtr;
				break;

			case DataType::Array:
				dataType=inSrc.dataType;
				arrayPtr.reset();
				new(&arrayPtr) std::shared_ptr< Array<TypedValue> >(inSrc.arrayPtr);
				break;

			case DataType::List:
				dataType=inSrc.dataType;
				listPtr.reset();
				new(&listPtr)
					std::shared_ptr< std::deque<TypedValue> >(inSrc.listPtr);
				break;

			case DataType::KV:
				dataType=inSrc.dataType;
				kvPtr.reset();
				new(&kvPtr)
					std::shared_ptr<
						std::unordered_map<TypedValue,TypedValue,TvHash>
					>(inSrc.kvPtr);
				break;

			case DataType::File:
				dataType=inSrc.dataType;
				new(&filePtr) std::shared_ptr<File>(inSrc.filePtr);
				break;

			case DataType::StdCode:
				dataType=inSrc.dataType;
				stdCodePtr=inSrc.stdCodePtr;
				break;

			case DataType::UserData:
				dataType=inSrc.dataType;
				userDataPtr.reset();
				new(&userDataPtr) std::shared_ptr<UserData>(inSrc.userDataPtr);
				break;

			case DataType::Context:
				dataType=inSrc.dataType;
				contextPtr.reset();
				new(&contextPtr) std::shared_ptr<Context>(inSrc.contextPtr);
				break;

			case DataType::Channel:
				dataType=inSrc.dataType;
				new(&channelPtr) std::shared_ptr<ChanMan>(inSrc.channelPtr);
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
			case DataType::Invalid:
			case DataType::Int:
			case DataType::Long:
			case DataType::Float:
			case DataType::Double:
			case DataType::CB:
			case DataType::StdCode:
				intValue=inValue;
				break;
			case DataType::BigInt:
				delete(bigIntPtr);
				intValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::Lambda:	lambdaPtr.reset();	break;

					case DataType::Symbol:
					case DataType::String:
					case DataType::MayBeAWord:
						stringPtr.reset();
						break;

					case DataType::Array:	arrayPtr.reset();	break;
					case DataType::List: 	listPtr.reset();	break;
					case DataType::KV:		kvPtr.reset();		break;
					case DataType::File:	filePtr.reset();	break;
					case DataType::Context:	contextPtr.reset();	break;
					case DataType::Channel: channelPtr.reset();	break;
					case DataType::UserData:userDataPtr.reset();break;

					default:
						;	// dummy
				}
				intValue=inValue;
		}
		dataType=DataType::Int;
	}

	inline void Set(float inValue) {
		switch(dataType) {
			case DataType::Invalid:
			case DataType::Int:
			case DataType::Long:
			case DataType::Float:
			case DataType::Double:
			case DataType::CB:
			case DataType::StdCode:
				floatValue=inValue;
				break;
			case DataType::BigInt:
				delete(bigIntPtr);
				floatValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::Lambda:	lambdaPtr.reset();	break;

					case DataType::Symbol:
					case DataType::String:
					case DataType::MayBeAWord:
						stringPtr.reset();
						break;

					case DataType::Array:	arrayPtr.reset();	break;
					case DataType::List: 	listPtr.reset();	break;
					case DataType::KV:		kvPtr.reset();		break;
					case DataType::File:	filePtr.reset();	break;
					case DataType::Context:	contextPtr.reset();	break;
					case DataType::Channel:	channelPtr.reset();	break;
					case DataType::UserData:userDataPtr.reset();break;

					default:
						;	// dummy
				}
				floatValue=inValue;
		}
		dataType=DataType::Float;
	}

	inline void Set(double inValue) {
		switch(dataType) {
			case DataType::Invalid:
			case DataType::Int:
			case DataType::Long:
			case DataType::Float:
			case DataType::Double:
			case DataType::CB:
			case DataType::StdCode:
				doubleValue=inValue;
				break;
			case DataType::BigInt:
				delete(bigIntPtr);
				doubleValue=inValue;
				break;
			default:
				switch(dataType) {
					case DataType::Lambda:	lambdaPtr.reset();	break;

					case DataType::Symbol:
					case DataType::String:
					case DataType::MayBeAWord:
						stringPtr.reset();
						break;

					case DataType::Array:	arrayPtr.reset();	break;
					case DataType::List: 	listPtr.reset();	break;
					case DataType::KV:		kvPtr.reset();		break;
					case DataType::File:	filePtr.reset();	break;
					case DataType::Context:	contextPtr.reset();	break;
					case DataType::Channel: channelPtr.reset();	break;
					case DataType::UserData:userDataPtr.reset();break;

					default:
						;	// dummy
				}
				doubleValue=inValue;
		}
		dataType=DataType::Double;
	}

	inline size_t Hash() const {
		switch(dataType) {
			case DataType::Invalid:
			case DataType::BigInt:
			case DataType::EoC:
			case DataType::EoF:
				return std::hash<int>()((int)dataType);

			case DataType::Word:
			case DataType::DirectWord:
			case DataType::NewWord:
				return std::hash<const Word*>()(wordPtr);

			case DataType::Lambda:
				return std::hash<const Word*>()(lambdaPtr.get());

			case DataType::IP:
			case DataType::ParamDest:
				return std::hash<const Word**>()(ipValue);

			case DataType::Bool:
				return std::hash<bool>()(boolValue);

			case DataType::Int:
			case DataType::Threshold:
			case DataType::EmptySlot:
			case DataType::Address:
			case DataType::MiscInt:
			case DataType::LVOP:
			case DataType::CB:
				return std::hash<int>()(intValue);

			case DataType::Long:
				return std::hash<long>()(longValue);

			case DataType::Float:
				return std::hash<float>()(floatValue);

			case DataType::Double:
				return std::hash<double>()(doubleValue);

			case DataType::String:
			case DataType::Symbol:
			case DataType::MayBeAWord:
				return std::hash<std::string>()(*stringPtr);

			case DataType::StdCode:
				return std::hash<WordFunc>()(stdCodePtr);

			case DataType::Array: {
					int length=arrayPtr->length;
					size_t hash=std::hash<int>()(length);
					for(int i=0; i<length; i++) {
						hash^=arrayPtr->data[i].Hash();
					}
					return hash;
				}

			case DataType::List: {
					int length=(int)listPtr->size();
					size_t hash=std::hash<int>()(length);
					for(int i=0; i<length; i++) {
						hash^=listPtr->at(i).Hash();
					}
					return hash;
				}

			case DataType::KV: {
					size_t hash=std::hash<int>()((int)kvPtr->size());
					for(auto kv : *kvPtr) {
						hash^=kv.first.Hash();
						hash^=kv.second.Hash();
					}
					return hash;
				}

			case DataType::File: return std::hash<File*>()(filePtr.get());
			case DataType::Context: return std::hash<Context*>()(contextPtr.get());
			case DataType::Channel: return std::hash<ChanMan*>()(channelPtr.get());
			case DataType::UserData: return std::hash<UserData*>()(userDataPtr.get());

			default:
				fprintf(stderr, "SYSTEM ERROR: TypedValue::Hash().");
				exit(-1);
		}
	}

	inline bool IsInvalid() const { return dataType==DataType::Invalid; }
	inline bool IsValid()   const { return dataType!=DataType::Invalid; }

	inline bool HasWordPtr(const Word **outWordPtr) const {
		if(dataType==DataType::Word
		   || dataType==DataType::DirectWord
		   || dataType==DataType::NewWord
		   || dataType==DataType::Lambda) {
			if(outWordPtr!=NULL) {
				*outWordPtr = dataType==DataType::Lambda ? lambdaPtr.get()
														 : wordPtr;
			}
			return true;
		} else {
			if(outWordPtr!=NULL) { *outWordPtr=NULL; }
			return false;
		}
	}

	inline bool HasStringPtr() const {
		return dataType==DataType::String || dataType==DataType::Symbol;
	}

	inline bool IsNumber() const {
		return dataType==DataType::Int
			|| dataType==DataType::Long
			|| dataType==DataType::Float
			|| dataType==DataType::Double
			|| dataType==DataType::BigInt;
	}

	inline bool IsZero() const {
		return (dataType==DataType::Int      && intValue==0)
			|| (dataType==DataType::Long     && longValue==0)
			|| (dataType==DataType::Float    && floatValue==0) 
			|| (dataType==DataType::Double   && doubleValue==0)
			|| (dataType==DataType::BigInt   && *bigIntPtr==0);
	}

	inline bool IsInteger() const {
		return dataType==DataType::Int
			|| dataType==DataType::Long
			|| dataType==DataType::BigInt;
	}

	inline bool IsCollection() const {
		return dataType==DataType::Array
			|| dataType==DataType::List
			|| dataType==DataType::KV;
	}

	PP_API int GetLevel() const;

	PP_API double ToDouble(Context& inContext);
	PP_API float  ToFloat(Context& inContext);

	PP_API std::string GetTypeStr() const;

	PP_API void PrintValue(int inIndent=0,bool inTypePostfix=false) const;
	PP_API std::string GetEscapedValueString(int inIndent=-1) const;
	PP_API std::string GetValueString(int inIndent=-1,
									  bool inDetail=true,
									  bool inTypePostfix=false) const;

	PP_API void Dump() const;
};

inline bool operator==(const TypedValue& inTV1,const TypedValue& inTV2) {
	if(inTV1.dataType!=inTV2.dataType) { return false; }

	switch(inTV1.dataType) {
		case DataType::Invalid:
		case DataType::EoC:
		case DataType::EoF:
			return true;

		case DataType::ParamDest:
		case DataType::IP:
			return inTV1.ipValue==inTV2.ipValue;

		case DataType::DirectWord:
		case DataType::NewWord:
			return inTV1.wordPtr==inTV2.wordPtr;

		case DataType::Lambda: 		return inTV1.lambdaPtr==inTV2.lambdaPtr;

		case DataType::Bool:
		case DataType::EmptySlot:
			return inTV1.boolValue==inTV2.boolValue;

		case DataType::Address:
		case DataType::Threshold:
		case DataType::MiscInt:
		case DataType::LVOP:
		case DataType::CB:
		case DataType::Int:
			return inTV1.intValue==inTV2.intValue;

		case DataType::Long:		return inTV1.longValue   ==inTV2.longValue;
		case DataType::BigInt:		return *inTV1.bigIntPtr  ==*inTV2.bigIntPtr;
		case DataType::Float: 		return inTV1.floatValue  ==inTV2.floatValue;
		case DataType::Double:		return inTV1.doubleValue ==inTV2.doubleValue;

		case DataType::Symbol:
		case DataType::String:
		case DataType::MayBeAWord:
			return *inTV1.stringPtr.get()==*inTV2.stringPtr.get();

		case DataType::Array:
			if(!(inTV1.arrayPtr->length==inTV2.arrayPtr->length)) {
				return false;
			}
			for(int i=0; i<inTV1.arrayPtr->length; i++) {
				if(!(inTV1.arrayPtr->data[i]==inTV2.arrayPtr->data[i])) {
					return false;
				}
			}
			return true;

		case DataType::List:
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

		case DataType::KV:
			return (*inTV1.kvPtr.get())==(*inTV2.kvPtr.get());

		case DataType::File: return inTV1.filePtr.get()==inTV2.filePtr.get();
		case DataType::Context: return inTV1.contextPtr.get()==inTV2.contextPtr.get();
		case DataType::Channel: return inTV1.channelPtr.get()==inTV2.channelPtr.get();
		case DataType::StdCode: return inTV1.stdCodePtr==inTV2.stdCodePtr;

		case DataType::UserData:
			return inTV1.userDataPtr.get()->dataPtr==inTV2.userDataPtr.get()->dataPtr;

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
PP_API bool IsValidDataTypeValue(DataType inDataType);

PP_API bool IsSameValue(const TypedValue& inTV1,const TypedValue& inTV2);

inline bool IsThreadInfo(TypedValue& inTV) {
	return inTV.dataType==DataType::CB
		   && (inTV.intValue & (int)ControlBlockType::kTHREAD)!=0;
}
