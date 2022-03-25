#include "word.h"
#include "stack.h"
#include "context.h"

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

PP_API const Word *GetWordPtr(const std::string& inWordName) {
    auto iter=Dict.find(inWordName);
	return iter==Dict.end() ? NULL : iter->second;
}

PP_API const Word *GetWordPtr(Context& inContext,const TypedValue& inTV) {
	const Word *ret;
	if( inTV.HasWordPtr(&ret) ) {
		return ret;
	} else if(inTV.dataType==DataType::String || inTV.dataType==DataType::Symbol) {
		const Word *ret=GetWordPtr(*inTV.stringPtr);
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

