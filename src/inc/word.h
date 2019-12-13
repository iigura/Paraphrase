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

struct Word {
	WordFunc code=NULL;
	std::string longName;
	std::string shortName;
	WordLevel level=WordLevel::Invalid;
	bool isForgetable=false;
	WordType type=WordType::Invalid;
	//bool useLocalVar=false;
	int numOfLocalVar=0;

	const Word **param=NULL;
	CodeThread *tmpParam=NULL;

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
	   level(WordLevel::Normal),type(WordType::Normal),code(Docol) {
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
			// empty
		} else {
			fprintf(stderr,
					"SYSTEM ERROR: can not forget the word '%s'\n",
					longName.c_str());
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

	void Dump() const {
		printf("num of local vars: %d\n",numOfLocalVar);
		std::vector<std::string> lvInfo(numOfLocalVar);
		for(auto itr=localVarDict.begin(); itr!=localVarDict.end(); itr++) {
			lvInfo[itr->second]=itr->first;
   		}
		for(int i=0; i<numOfLocalVar; i++) {
			printf("  slot #%02d: local var name: %s\n",i,lvInfo[i].c_str());
		}
		printf("the word '%s' is:\n",longName.c_str());
		const size_t n=tmpParam->size();
		for(size_t i=0; i<n; i++) {
			printf("[%zu] ",i);
			tmpParam->at(i).Dump();
		}
	}
};

#define WORD_FUNC [](Context& inContext) NOEXCEPT ->bool
#define NEXT do { ++inContext.ip; return true; } while(0)

PP_API const Word *GetWordPtr(const std::string& inWordName);

