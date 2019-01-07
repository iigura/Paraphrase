#include "word.h"

PP_API const Word *GetWordPtr(const std::string& inWordName) {
    auto iter=Dict.find(inWordName);
	return iter==Dict.end() ? NULL : iter->second;
}

