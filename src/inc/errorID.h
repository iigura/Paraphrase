#pragma once

enum class PPLang { EN, JP, };

enum class NoParamErrorID {
	SystemError,

	IllegalLVOP,

	ShouldBeExecutedInDefinition,
	ShouldBeExecuteOnDebugger,
	InvalidUse,

	SorryNotSupportThisOperation,
	OpenDoubleQuote,
	OpenSingleQuote,
	InvalidWordName,

	ThreadMayBeBroken,

	LocalVarUseOnlyWordDef,
	// NoLocalVarFrame,

	DsIsEmpty,  DsAtLeast2, DsAtLeast3, DsAtLeast4, DsAtLeast5,
	DsAtLeast6, DsAtLeast7, DsAtLeast8, DsAtLeast9, DsAtLeast10,

	RsIsEmpty,  RsAtLeast2, RsAtLeast3,

	RsBroken, RsBrokenTosShouldBeInt, RsBrokenSecondShouldBeInt, // RsNotEnough,

	IpsBroken,	// IP Stack is broken

	SsBroken,

	NoLastDefinedWord,

	CStyleCommentMismatch,

	HereDocumentMismatch,

	CanNotConvertToInt,
	CanNotConvertToIntDueToOverflow,
	CanNotConvertToLongDueToOverflow,
	CanNotConvertToFloatDueToOverflow,
	CanNotConvertToDoubleDueToOverflow,
	CanNotConvertToSymbol,

	ChanIsAlreadyClosed,
	NoChanFromParent, NoChanToParent, NoChanFromChild, NoChanFromPipe,

	TosArrayNoElement,
	TosListNoElement, TosListAtLeastCores, TosLambdaList, TosSymbolList,

	TosShouldBeNonZero,

	// NoLoopBlock,

	ListMismatch,
	FormatDataMismatch,

	InvalidEnumList,

	FileIsAlreadyClosed, CanNotCloseFile, CanNotFlushFile,

	RegexCollate,   RegexCType,      RegexEscape,   RegexBackRef, 	  RegexBrack,
	RegexParen,     RegexBrace,      RegexBadBrace, RegexRange,   	  RegexSpace,
	RegexBadRepeat, RegexComplexity, RegexStack,    RegexUnknownError,

	// WpAtTosShouldPushABool, WpAtSecondShouldPushABool,

	ShouldBeCompileMode,

	SyntaxMismatchIf, SyntaxMismatchFor, // SyntaxMismatchWhile, SyntaxMismatchLoop,
	SyntaxMismatchSwitchCond, SyntaxMismatchLeavableLoopGroup,

	SyntaxOpenIf, SyntaxOpenFor, SyntaxOpenSwitch,
	// SyntaxOpenWhile, SyntaxOpenDo,

	SsMiscIntInvalid,

	InvalidValue,

	NoWordedDocol,
};

enum class InvalidTypeErrorID {
	TosInt, TosIntOrLongOrBigint,
	TosDoublable, TosNumber,
	// TosIntOrLong, TosSymbolOrList, TosArray, TosBoolOrWp,
	TosBool,
	TosString, TosStringOrWord,
	TosSymbol, TosSymbolOrWord,
	TosSymbolOrString,
	TosSymbolOrStringOrWord,
	TosArrayOrList, TosArrayOrListOrStringOrSymbol,
	TosList,
	TosFile,
	TosNumberOrString,
	TosWp, // TosWpOrStdCode, TosAddress,
	TosParamDest,
	TosNewWord, TosWpOrNw,
	TosWpOrStringOrKV,
	TosWpOrStringOrSymbolOrKV,
	TosStdCode,
	TosCanNotConvertToBool,
	TosCanNotConvertToString,
	
	SecondInt, SecondIntOrLongOrBigint,
	SecondBool,
	SecondString, SecondStringOrSymbol,
	// SecondArray, SecondBoolOrWp,
	SecondList,
	SecondKV,
	SecondArrayOrList, SecondArrayOrListOrKV,
	SecondFile,
	SecondThreshold,
	SecondWp,

	ThirdBool,
	ThirdList,
	ThirdString,
	ThirdArrayOrListOrKV,
	ThirdWp,

	RsTosIntOrLongOrBigint,
	RsTosNumber,
	RsTosWp,
	// RsTosAddress,
	RsTosThreshold,

	RsSecondIntOrLongOrBigint,
	RsSecondWp,

	SsMiscInt,
	SsTosShouldBeLoop,

	TosCanNotConvertToDouble,
	TosCanNotConvertToFloat,

	FirstElementString,

	LvNumber,

	ElementShouldBeTuple,
	CompiledLiteralShouldBeList,
	CompiledLiteralShouldBeSymbolOrString,
	CompiledLiteralShouldBeSymbolOrStringOrList,
};

enum class InvalidValueErrorID {
	TosNoSuchAKey,
};

enum class InvalidTypeTosSecondErrorID {
	BothDataInvalid,
	OutOfSupportTosSecond,
};

enum class InvalidTypeStrTvTvErrorID {
	ForOp,
};

enum class ErrorIdWithString {
	InvalidToken,

	CanNotFindTheWord, CanNotFindTheLocalVar,
	CanNotInstallUnoverWritable, CanNotMakeShortenNameUnoverWritable,

	AmbigousWordName,

	AlreadyDefined,
	AlreadyRegisteredLocalVar, NoSuchLocalVar,

	CanNotOpenFile, CanNotCreateFile, CanNotCloseTheFile, CanNotReadFile,

	CanNotFindTheEntryPoint,

	NotEnoughParamValues,

	StringShouldBeOneChar,
};

enum class ErrorIdWithInt {
	TosPositiveInt, SecondPositiveInt,
	TosNonNegative, SecondNonNegative,

	InvalidLocalSlotIndex,
};

enum class ErrorIdWith2int {
	// DsNotEnoughData,

	DepthIndexOutOfRange, StringIndexOutOfRange, 	ArrayIndexOutOfRange,
	ListIndexOutOfRange,  ListStartIndexOutOfRange, ListLengthIndexOutOfRange,
};

