#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "externals.h"
#include "typedValue.h"
#include "dictMan.h"
#include "optimizer.h"

struct Context;

enum class WordLevel {
	Invalid		=-1,
	Normal		= 0,
	Immediate	= 1,
	Level2		= 2,
};

enum class WordType {
	Invalid	=-1,
	Normal	= 0,
	NoName	= 1,
	List	= 2,
};

typedef std::unordered_map<std::string,int /* indexForSlot */> LocalVarDict;
typedef std::unordered_map<TypedValue,TypedValue,TypedValue::TvHash> Attr;

struct Word {
	WordFunc code=NULL;
	std::string longName;
	std::string shortName;
	std::string vocabulary;
	WordLevel level=WordLevel::Invalid;
	bool isForgetable=false;
	WordType type=WordType::Invalid;

	Attr *attr=NULL;

	int numOfLocalVar=0;

	const Word **param=NULL;
	CodeThread *tmpParam=NULL;
	std::shared_ptr<const Word> paramSrcWordHolder;	// for reference counter incrementation
	std::unordered_map<int,int>  *addressOffsetToIndexMapper=NULL;

	LocalVarDict localVarDict;
	LVOP LVOpHint=LVOP::NOP;

	PP_API Word(const char *inShortName,bool (*inFunc)(Context& inContext) NOEXCEPT,
		 LVOP inLVOpHint=LVOP::NOP)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(WordLevel::Normal),type(WordType::Normal),code(inFunc),
	   LVOpHint(inLVOpHint)	{
		// empty
	}

	PP_API Word(const char *inShortName,WordLevel inLevel,
		 bool (*inFunc)(Context& inContext) NOEXCEPT,
		 LVOP inLVOpHint=LVOP::NOP)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(inLevel),type(WordType::Normal),code(inFunc),LVOpHint(inLVOpHint) {
		// empty
	}

	// for user defined word
	PP_API Word(const std::string* inShortName);

	// for newWord
	PP_API Word(bool (*inFunc)(Context& inContext) NOEXCEPT)
	  :shortName(),longName(),
	   level(WordLevel::Normal),type(WordType::NoName),code(inFunc),
	   isForgetable(true) {
		tmpParam=new std::vector<TypedValue>();
	}

	// for comment and List
	PP_API Word(WordType inWordType)
	  :shortName(),longName(),
	   level(WordLevel::Normal),type(inWordType),isForgetable(true) {
		tmpParam=new std::vector<TypedValue>();
	}

	PP_API ~Word() {
		if( isForgetable ) {
			if(param!=NULL) { delete param; }
			if(addressOffsetToIndexMapper!=NULL) { delete addressOffsetToIndexMapper; }
		} else {
			fprintf(stderr,
					"SYSTEM ERROR: illegal destructor call. "
					"can not forget the word '%s'\n",
					longName.c_str());
			exit(-1);
		}
	}

	PP_API void CompileValue(const TypedValue& inTypedValue) {
		tmpParam->push_back(inTypedValue);
	}

	PP_API bool CompileValue(int inAddress,const TypedValue& inTypedValue,
							 bool inIsForceUpdate=false) {
		if(tmpParam->size()<=inAddress) {
			fprintf(stderr,"SYSTEM ERROR at Compile(addr,tv).\n");
			return false;
		}
		TypedValue& emptySlot=tmpParam->at(inAddress);
		if(inIsForceUpdate==false && emptySlot.dataType!=DataType::EmptySlot) {
			fprintf(stderr,"SYSTEM ERROR at Compile(addr,tv).\n");
			fprintf(stderr,"slot is not an empty slot.\n");
			fprintf(stderr,"current slot is:");
			emptySlot.Dump();
			return false;
		}
		tmpParam->at(inAddress)=inTypedValue;
		return true;
	}

	PP_API bool CompileWord(const char *inWordName) {
		const Word *wordPtr=GetWordPtr(inWordName);
		if(wordPtr==NULL) { return false; }
		CompileValue(wordPtr);
		return true;
	}

	PP_API int GetParamSize() {
		const size_t n=tmpParam->size();
		size_t ret=0;
		for(size_t i=0; i<n; i++) {
			TypedValue tv=tmpParam->at(i);
			switch(tv.dataType) {
				case DataType::Address:
				case DataType::DirectWord:
					ret++;
					break;
				default: ret += TvSize;
			}
		}
		return (int)ret;
	}

	PP_API void BuildParam();

	PP_API bool RegisterLocalVar(const std::string& inVarName) {
		if(localVarDict.find(inVarName)!=localVarDict.end()) {
			return false;	// already registered
		}
		const int slotIndex=(int)localVarDict.size();
		localVarDict[inVarName]=slotIndex;
		return true;
	}

	PP_API int GetLocalVarSlotPos(const std::string& inVarName) {
		if(localVarDict.find(inVarName)==localVarDict.end()) { return -1; }
		return localVarDict[inVarName];
	}

	PP_API void Dump() const;
};

#define WORD_FUNC [](Context& inContext) NOEXCEPT ->bool
#define NEXT do { ++inContext.ip; return true; } while(0)

PP_API bool Docol(Context& inContext) NOEXCEPT;
PP_API bool Semis(Context& inContext);

PP_API bool SetAttr(Context& inContext,
					TypedValue& ioWord,TypedValue& inKey,TypedValue& inValue);
PP_API bool HasAttr(const Word *inWord,const TypedValue& inKey);

PP_API const Word *GetWordPtr(Context& inContext,const TypedValue& inTV);
PP_API void CopyWordProperty(Word *outDestWord,const Word *inSrcWord);

PP_API void SetDeleteByForget();
PP_API void ClearDeleteByForget();
PP_API bool CheckDeleteByForget();

