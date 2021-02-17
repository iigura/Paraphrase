#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "externals.h"
#include "typedValue.h"

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

extern PP_API WordFunc G_DocolAdvice;

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
	std::unordered_map<int,int>  *addressOffsetToIndexMapper=NULL;

	LocalVarDict localVarDict;
	LVOP LVOpHint=LVOP::NOP;

	Word(const char *inShortName,bool (*inFunc)(Context& inContext) NOEXCEPT,
		 LVOP inLVOpHint=LVOP::NOP)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(WordLevel::Normal),type(WordType::Normal),code(inFunc),
	   LVOpHint(inLVOpHint)	{
		// empty
	}

	Word(const char *inShortName,WordLevel inLevel,
		 bool (*inFunc)(Context& inContext) NOEXCEPT,
		 LVOP inLVOpHint=LVOP::NOP)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(inLevel),type(WordType::Normal),code(inFunc),LVOpHint(inLVOpHint) {
		// empty
	}

	// for user defined word
	Word(const std::string *inShortName)
	  :shortName(*inShortName),longName(GetCurrentVocName()+":"+*inShortName),
	   level(WordLevel::Normal),type(WordType::Normal),code(G_DocolAdvice) {
		tmpParam=new std::vector<TypedValue>();
	}

	// for newWord
	Word(bool (*inFunc)(Context& inContext) NOEXCEPT)
	  :shortName(),longName(),
	   level(WordLevel::Normal),type(WordType::NoName),code(inFunc),
	   isForgetable(true) {
		tmpParam=new std::vector<TypedValue>();
	}

	// for comment and List
	Word(WordType inWordType)
	  :shortName(),longName(),
	   level(WordLevel::Normal),type(inWordType),isForgetable(true) {
		tmpParam=new std::vector<TypedValue>();
	}

	~Word() {
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

	bool RegisterLocalVar(const std::string& inVarName) {
		if(localVarDict.find(inVarName)!=localVarDict.end()) {
			return false;	// already registered
		}
		const int slotIndex=(int)localVarDict.size();
		localVarDict[inVarName]=slotIndex;
		return true;
	}

	int GetLocalVarSlotPos(const std::string& inVarName) {
		if(localVarDict.find(inVarName)==localVarDict.end()) { return -1; }
		return localVarDict[inVarName];
	}

	PP_API void Dump() const;
};

#define WORD_FUNC [](Context& inContext) NOEXCEPT ->bool
#define NEXT do { ++inContext.ip; return true; } while(0)

PP_API const Word *GetWordPtr(const std::string& inWordName);
PP_API const Word *GetWordPtr(Context& inContext,const TypedValue& inTV);
PP_API void CopyWordProperty(Word *outDestWord,const Word *inSrcWord);

