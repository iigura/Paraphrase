#include <string>

#include "word.h"
#include "stack.h"
#include "context.h"

// for user defined word
PP_API Word::Word(const std::string* inShortName)
  :shortName(*inShortName), longName(GetCurrentVocName() + ":" + *inShortName),
   level(WordLevel::Normal), type(WordType::Normal), code(G_DocolAdvice) {
	tmpParam = new std::vector<TypedValue>();
}

PP_API void Word::BuildParam() {
	numOfLocalVar=(int)localVarDict.size();
	Optimize(tmpParam);
	ReplaceTailRecursionToJump(this,tmpParam);

	int paramSize=GetParamSize();

	const size_t n=tmpParam->size();
	int *addressTransferMap=new int[paramSize];
	size_t newIndex=0;
	for(int i=0; i<n; i++) {
		addressTransferMap[i]=(int)newIndex;
		TypedValue tv=tmpParam->at(i);
		switch(tv.dataType) {
			case DataType::Address:
			// case DataType::DirectWord:
			case DataType::Word:
				newIndex++; break;
			default:
				newIndex += sizeof(TypedValue)%sizeof(WordFunc*)==0
						  ? sizeof(TypedValue)/sizeof(WordFunc*)
						  : sizeof(TypedValue)/sizeof(WordFunc*)+1;
		}
	}

	const Word **wordPtrArray=new const Word*[paramSize+1];
	addressOffsetToIndexMapper=new std::unordered_map<int,int>();
	int dest=0;
	for(int i=0; i<n; i++) {
		addressOffsetToIndexMapper->insert({dest,i});
		TypedValue& tv=tmpParam->at(i);
		if(tv.dataType==DataType::Word /* DirectWord */) {
			wordPtrArray[dest]=tv.wordPtr;
			dest++;
		} else if(tv.dataType==DataType::Address) {
			wordPtrArray[dest]
				=(const Word*)(wordPtrArray+addressTransferMap[tv.intValue]);
			dest++;
		} else {
			new((TypedValue*)(wordPtrArray+dest)) TypedValue(tv);
			dest += TvSize;
		}
	}
	wordPtrArray[paramSize]=NULL;	// guard (end mark)

	param=wordPtrArray;
	isForgetable=true;
}

/****
                    Word
IP---> Word* ----->+---------+
      (WordFunc*)  |WordFunc-+-->function
                   |         |
****/
PP_API bool Docol(Context& inContext) NOEXCEPT {
	// printf("Docol - IN\n");
	inContext.IS.emplace_back(inContext.ip);
	const Word *word=(*inContext.ip);
	// printf("DOCOL: word=%s IN env size=%d\n",word->shortName.c_str(),(int)inContext.Env.size());
	if(word->numOfLocalVar>0) {
		// printf("DOCOL: Word=%s PUSH ENV (env size=%d)\n",word->shortName.c_str(),(int)inContext.Env.size());
		inContext.Env.push_back(
			LocalVarSlot(word->numOfLocalVar,
						 std::pair<TypedValue,bool>(TypedValue(),false)));
	}
	inContext.ip=word->param;
	// printf("Docol - OUT\n");
	return true;
}

PP_API bool Semis(Context& inContext) {
	if(inContext.IS.size()<1) { return inContext.Error(NoParamErrorID::IsBroken); }
	inContext.ip=inContext.IS.back();
	inContext.IS.pop_back();
	if((*inContext.ip)->numOfLocalVar>0) {
		assert(inContext.Env.size()>0);
		inContext.Env.pop_back();
	}
	return true;
}

// Attributes beginning with a '$' sign are used in the language system
// (language system reserved)
PP_API bool SetAttr(Context& inContext,
			 		TypedValue& ioWord,TypedValue& inKey,TypedValue& inValue) {
	const Word *wordPtr;
	if(ioWord.HasWordPtr(&wordPtr)==false) {
		return inContext.Error(InvalidTypeErrorID::ThirdWp,ioWord);
	}
	Word *word=(Word *)wordPtr;
	if(word->attr==NULL) { word->attr=new Attr(); }
	auto itr=word->attr->find(inKey);
	if(itr==word->attr->end()) {
		word->attr->insert({inKey,inValue});
	} else {
		itr->second=inValue;
	}
	return true;
}

PP_API bool HasAttr(const Word *inWord,const TypedValue& inKey) {
	bool ret;
	if(inWord->attr==NULL) {
		ret=false;
	} else {
		size_t numOfCounts=inWord->attr->count(inKey);
		ret = numOfCounts!=0;
	}
	return ret;
}

PP_API void CopyWordProperty(Word *outDestWord,const Word *inSrcWord) {
	outDestWord->param        =inSrcWord->param;
	outDestWord->tmpParam     =inSrcWord->tmpParam;
	outDestWord->numOfLocalVar=inSrcWord->numOfLocalVar;
	outDestWord->localVarDict =inSrcWord->localVarDict;
	outDestWord->LVOpHint     =inSrcWord->LVOpHint;
	if(inSrcWord->attr!=NULL) {
		outDestWord->attr=new Attr();
		*outDestWord->attr=(*inSrcWord->attr);
	}
	if(inSrcWord->addressOffsetToIndexMapper!=NULL) {
		outDestWord->addressOffsetToIndexMapper=new std::unordered_map<int,int>();
		*outDestWord->addressOffsetToIndexMapper
			=(*inSrcWord->addressOffsetToIndexMapper);
	}
}

PP_API void Word::Dump() const {
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

PP_API const Word *GetWordPtr(Context& inContext,const TypedValue& inTV) {
	const Word *ret;
	if( inTV.HasWordPtr(&ret) ) {
		return ret;
	} else if(inTV.dataType==DataType::String || inTV.dataType==DataType::Symbol) {
		const Word *ret=GetWordPtr(inTV.stringPtr->c_str());
		if(ret==NULL) {
			inContext.Error(ErrorIdWithString::CanNotFindTheWord,*inTV.stringPtr);
			return NULL;
		}
		return ret;
	} else {
		inContext.Error(InvalidTypeErrorID::TosSymbolOrStringOrWord,inTV);
		return NULL;
	}
}

static bool gDeleteByForget=false;

PP_API void SetDeleteByForget()   { gDeleteByForget=true;  }
PP_API void ClearDeleteByForget() { gDeleteByForget=false; }
PP_API bool CheckDeleteByForget() { return gDeleteByForget; }

