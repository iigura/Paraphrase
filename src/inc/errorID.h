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

	IsBroken,	// IS (=IP stack) is broken

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
	SsInvalidThetaOnSecond,

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
	TosChannel,
	TosContext,
	TosNumberOrString,
	TosWp, // TosWpOrStdCode, TosAddress,
	TosParamDest,
	TosNewWord, TosWpOrNw,
	TosWpOrStringOrKV,
	TosWpOrStringOrSymbolOrKV,
	TosStdCode,
	TosContextOrContainer,
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
	SecondContext,
	SecondCB,

	ThirdBool,
	ThirdList,
	ThirdString,
	ThirdArrayOrListOrKV,
	ThirdWp,
	ThirdUserData,

	RsTosIntOrLongOrBigint,
	RsTosNumber,
	RsTosWp,
	// RsTosAddress,
	RsTosThreshold,

	RsSecondIntOrLongOrBigint,
	RsSecondWp,

	SsMiscInt,
	SsTosShouldBeLoop,
	SsThirdWp,

	TosCanNotConvertToDouble,
	TosCanNotConvertToFloat,

	FirstElementString,

	LvNumber,

	ElementShouldBeTuple,

	ArrayElemContext,
	ListElemContext,
	KvElemContext,

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
	InvalidFormat,

	CanNotFindTheWord, CanNotFindTheLocalVar,
	CanNotInstallUnoverWritable, CanNotMakeShortenNameUnoverWritable,

	AmbigousWordName,

	AlreadyDefined,
	AlreadyRegisteredLocalVar, NoSuchLocalVar,

	CanNotOpenFile, CanNotCreateFile, CanNotCloseTheFile, CanNotReadFile,
	NoSuchFileExist,

	CanNotFindTheEntryPoint,

	NotEnoughParamValues,

	StringShouldBeOneChar,

	CanNotCreateDir,
	DirAlreadyExists,
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

