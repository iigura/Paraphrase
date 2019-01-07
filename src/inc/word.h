#pragma once

#include <string>
#include <vector>

#include "externals.h"
#include "typedValue.h"

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

typedef std::vector<TypedValue> CodeThread;

struct Word {
	WordFunc code=NULL;
	std::string longName;
	std::string shortName;
	WordLevel level=WordLevel::Invalid;
	bool isForgetable=false;
	WordType type=WordType::Invalid;

	const Word **param=NULL;
	CodeThread *tmpParam=NULL;

	Word(const char *inShortName,bool (*inFunc)(Context& inContext) NOEXCEPT)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(WordLevel::Normal),type(WordType::Normal),code(inFunc) {
		// empty
	}

	Word(const char *inShortName,WordLevel inLevel,
		 bool (*inFunc)(Context& inContext) NOEXCEPT)
	  :shortName(inShortName),longName(GetCurrentVocName()+":"+inShortName),
	   level(inLevel),type(WordType::Normal),code(inFunc) {
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
};

#define WORD_FUNC [](Context& inContext) NOEXCEPT ->bool
#define NEXT do { ++inContext.ip; return true; } while(0)

PP_API const Word *GetWordPtr(const std::string& inWordName);

