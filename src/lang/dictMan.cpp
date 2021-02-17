#include <vector>
#include <unordered_map>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"
#include "word.h"
#include "context.h"

PP_API std::unordered_map<std::string,const Word*> Dict;

std::string gStdVocName("std");
static std::string gCurrentVocName(gStdVocName);

std::unordered_map<std::string,std::vector<Word*>*> gVocabulary;
std::unordered_map<std::string,std::vector<std::string>*> gNameToVocName;

static bool ambigous(Context& inContext) NOEXCEPT;

PP_API const std::string& GetCurrentVocName() {
    return gCurrentVocName;
}

PP_API void SetCurrentVocName(const std::string& inStr) {
	gCurrentVocName=inStr;
}

PP_API bool Install(Word *inWord,const bool inUseShortendForm) {
	const std::string& currentVocName=GetCurrentVocName();
	auto iter=gVocabulary.find(currentVocName);
	if(iter==gVocabulary.end()) {
		gVocabulary[currentVocName]=new std::vector<Word*>();
	}
	
	auto dictIter=Dict.find(inWord->longName);
	if(dictIter!=Dict.end() && dictIter->second->isForgetable==false) {
		return GlobalContext->Error(ErrorIdWithString::E_CAN_NOT_INSTALL_UNOVERWRITABLE,inWord->longName);
	}
	Dict[inWord->longName]=inWord;

	// name to voc. name
	if( inUseShortendForm ) {
		auto nameToVocNameIter=gNameToVocName.find(inWord->shortName);
		if(nameToVocNameIter==gNameToVocName.end()) {
			gNameToVocName[inWord->shortName]=new std::vector<std::string>();
		}
		std::vector<std::string> *v=gNameToVocName[inWord->shortName];
		bool found=false;
		const size_t n=v->size();
		for(size_t i=0; i<n; i++) {
			if(v->at(i)==currentVocName) {
				found=true;
				break;
			}
		}
		if( !found ) {
			v->push_back(currentVocName);
		}

		dictIter=Dict.find(inWord->shortName);
		if(dictIter!=Dict.end()) {
			if(v->size()==1 && v->at(0)==currentVocName
			  && dictIter->second->isForgetable) {
				Dict[inWord->shortName]=inWord;
			} else {
				const Word *shortendWord=dictIter->second;
				if(shortendWord->isForgetable==false) {
					GlobalContext->Error(ErrorIdWithString::E_CAN_NOT_MAKE_SHORTEN_NAME_UNOVERWRITABLE,
										shortendWord->shortName);
				} else {
					delete shortendWord;
					Word *ambigousWord=new Word(inWord->shortName.c_str(),ambigous);
					ambigousWord->shortName=std::string();
					Dict[inWord->shortName]=ambigousWord;
				}
			}
		} else {
			Dict[inWord->shortName]=inWord;
		}
	}
	inWord->vocabulary=currentVocName;

	return true;
}

PP_API void Uninstall(const Word *inTargetWord) {
	auto dictIter=Dict.find(inTargetWord->longName);
	if(dictIter==Dict.end()) {
		fprintf(stderr,
				"SYSTEM ERROR at Uninstall(): can not find the name (%s)..\n",
				inTargetWord->longName.c_str());
		exit(-1);
	}
	Dict.erase(dictIter);

	if(inTargetWord->shortName.length()>0) {
		dictIter=Dict.find(inTargetWord->shortName);
		if(dictIter==Dict.end()) {
			fprintf(stderr,
					"SYSTEM ERROR at Uninstall(): can not find the name (%s).\n",
					inTargetWord->shortName.c_str());
			exit(-1);
		}
		Dict.erase(dictIter);
	}
}

PP_API void Alias(const char *inNewName,const char *inSrcName) {
	auto dictIter=Dict.find(std::string(inSrcName));
	if(dictIter==Dict.end()) {
		fprintf(stderr,
			"SYSTEM ERROR at Alias(): can not find the name (%s)..\n",inSrcName);
		exit(-1);
	}
	Install(new Word(inNewName,Dict[inSrcName]->level,Dict[inSrcName]->code));
}

static bool ambigous(Context& inContext) NOEXCEPT {
	const Word *self=(*inContext.ip);
	inContext.Error(ErrorIdWithString::E_AMBIGOUS_WORD_NAME,self->shortName);

	auto iter=gNameToVocName.find(self->shortName);
	if(iter==gNameToVocName.end()) {
		inContext.Error(NoParamErrorID::E_SYSTEM_ERROR);
		exit(-1);
	}
	auto vector=iter->second;
	const size_t n=vector->size()-1;
	for(size_t i=0; i<n; i++) {
		fprintf(stderr,"%s, ",vector->at(i).c_str());
	}
	fprintf(stderr,"%s.\n",vector->at(vector->size()-1).c_str());
	fprintf(stderr,"please use 'packageName:wordName' "
				   "for example 'vec3:dot'.\n");

	NEXT;
}

/****
                    Word
IP---> Word* ----->+---------+
      (WordFunc*)  |WordFunc-+-->function
                   |         |
****/
PP_API bool Docol(Context& inContext) NOEXCEPT {
	inContext.IS.emplace_back(inContext.ip);
	const Word *word=(*inContext.ip);
	if(word->numOfLocalVar>0) {
		inContext.Env.push_back(LocalVarSlot(word->numOfLocalVar,TypedValue()));
	}
	inContext.ip=word->param;
	return true;
}

