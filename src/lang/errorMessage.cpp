#include <stdio.h>

#include "typedValue.h"
#include "paraphrase.h"
#include "externals.h"
#include "word.h"
#include "stack.h"
#include "context.h"
#include "errorID.h"

static bool gErrorMessageWithTrace=false;

PP_API void SetTraceOn()  { gErrorMessageWithTrace=true;  }
PP_API void SetTraceOff() { gErrorMessageWithTrace=false; }
PP_API bool IsTraceOn() { return gErrorMessageWithTrace; }
PP_API void ShowTrace(const Context& inContext) {
	printf("--- trace info --- (caller)\n");
	for(int i=0; i<inContext.IS.size(); i++) {
		const Word **p=inContext.IS[i];
		printf("%s\n",(*p)->longName.c_str());
	}
	printf("------------------ (callee)\n");
}

typedef std::unordered_map<NoParamErrorID,
			std::unordered_map<PPLang,std::string> > NoParamErrorMessageDict;

typedef std::unordered_map<InvalidTypeErrorID,
			std::unordered_map<PPLang,std::string> > InvalidTypeErrorMessageDict;

typedef std::unordered_map<InvalidValueErrorID,
			std::unordered_map<PPLang,std::string> > InvalidValueErrorMessageDict;

typedef std::unordered_map<InvalidTypeTosSecondErrorID,
			std::unordered_map<PPLang,std::string> >
				InvalidTypeTosSecondErrorMessageDict;

typedef std::unordered_map<InvalidTypeStrTvTvErrorID,
			std::unordered_map<PPLang,std::string> >
				InvalidTypeStrTvTvErrorMessageDict;

typedef std::unordered_map<ErrorIdWithString,
			std::unordered_map<PPLang,std::string> > ErrorWithStringMessageDict;

typedef std::unordered_map<ErrorIdWithInt,
			std::unordered_map<PPLang,std::string> > ErrorWithIntMessageDict;

typedef std::unordered_map<ErrorIdWith2int,
			std::unordered_map<PPLang,std::string> > ErrorWith2IntMessageDict;

static NoParamErrorMessageDict gNoParamErrorMessageDict {
	{ NoParamErrorID::SystemError, {
		{ PPLang::EN,	"SYSTEM ERROR." },
		{ PPLang::JP,	"システムエラー。" },
	  }
	},

	{ NoParamErrorID::IllegalLVOP, {
		{ PPLang::EN,	"Illegal LVOP." },
		{ PPLang::JP,	"不正な LVOP です" },
	  }
	},

	{ NoParamErrorID::ShouldBeExecutedInDefinition, {
		{ PPLang::EN,	"should be executed in definition "
						"(forgot ':' or {{ ... }} ?)." },
		{ PPLang::JP,	"ワード定義中に実行する必要があります"
						"（ : や {{ ... }} を忘れましたか？)" },
	  }
	},
	{ NoParamErrorID::ShouldBeExecuteOnDebugger, {
		{ PPLang::EN,	"should be execute on the debugger." },
		{ PPLang::JP,	"このワードはデバッガ上にて実行する必要があります。" },
	  }
	},
	{ NoParamErrorID::InvalidUse, {
		{ PPLang::EN,	"The usage of the Word is incorrect (see word reference)." },
		{ PPLang::JP,	"ワードの使用方法が間違っています"
						"（ワードリファレンスを参照して下さい）。" },
	  }
	},

	{ NoParamErrorID::SorryNotSupportThisOperation, {
		{ PPLang::EN,	"sorry, can not support this operation.." },
		{ PPLang::JP,	"この操作は現段階ではまだサポートされていません。" },
	  }
	},
	{ NoParamErrorID::OpenDoubleQuote, {
		{ PPLang::EN,	"open string about double-quote." },
		{ PPLang::JP, "ダブルクォーテーションが閉じていません。"},
	  }
	},
	{ NoParamErrorID::OpenSingleQuote, {
		{ PPLang::EN,	"open string about single-quote." },
		{ PPLang::JP, "シングルクォーテーションが閉じていません。"},
	  }
	},
	{ NoParamErrorID::InvalidWordName, {
		{ PPLang::EN,	"invalid word name (might not it start with the '$' sign?)" },
		{ PPLang::JP,	"有効なワード名ではありません"
						"（ワード名が '$' 記号で始まっていませんか？）" },
	  }
	},
	{ NoParamErrorID::ThreadMayBeBroken, {
		{ PPLang::EN,	"thread may be broken." },
		{ PPLang::JP,	"スレッドの情報が異常です。" },
	  }
	},

	{ NoParamErrorID::LocalVarUseOnlyWordDef, {
		{ PPLang::EN,	"local variables can only use in word definition." },
		{ PPLang::JP,	"ローカル変数はワード定義中にのみ使用できます。" },
	  }
	},

	//{ NoParamErrorID::NoLocalVarFrame, {
	//	{ PPLang::EN,	"no local variable frames." },
	//	{ PPLang::JP,	"ローカル変数用のフレームが存在しません。" },
	//  }
	//},

	{ NoParamErrorID::DsIsEmpty, {
		{ PPLang::EN,	"DS is empty." },
		{ PPLang::JP, "DS が空です。"},
	  }
	},
	{ NoParamErrorID::DsAtLeast2, {
		{ PPLang::EN,	"DS should have two elements at least."},
		{ PPLang::JP,	"DS に 2 つの値が格納されていません。" },
	  }
	},
	{ NoParamErrorID::DsAtLeast3, {
		{ PPLang::EN,	"DS should have three elements at least."},
		{ PPLang::JP,	"DS に 3 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast4, {
		{ PPLang::EN,	"DS should have four elements at least."},
		{ PPLang::JP,	"DS に 4 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast5, {
		{ PPLang::EN,	"DS should have five elements at least."},
		{ PPLang::JP,	"DS に 5 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast6, {
		{ PPLang::EN,	"DS should have six elements at least."},
		{ PPLang::JP,	"DS に 6 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast7, {
		{ PPLang::EN,	"DS should have seven elements at least."},
		{ PPLang::JP,	"DS に 7 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast8, {
		{ PPLang::EN,	"DS should have eight elements at least."},
		{ PPLang::JP,	"DS に 8 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast9, {
		{ PPLang::EN,	"DS should have nine elements at least."},
		{ PPLang::JP,	"DS に 9 つの値が格納されていません。"	 },
	  }
	},
	{ NoParamErrorID::DsAtLeast10, {
		{ PPLang::EN,	"DS should have ten elements at least."},
		{ PPLang::JP,	"DS に 10 個の値が格納されていません。"	 },
	  }
	},

	{ NoParamErrorID::RsIsEmpty, {
		{ PPLang::EN,	"RS is empty."  },
		{ PPLang::JP,	"RS が空です。" },
	  }
	},
	{ NoParamErrorID::RsAtLeast2, {
		{ PPLang::EN,	"RS should have two elements at least."},
		{ PPLang::JP,	"RS に 2 つの値が格納されていません。" },
	  }
	},
	{ NoParamErrorID::RsAtLeast3, {
		{ PPLang::EN,	"RS should have three elements at least."},
		{ PPLang::JP,	"RS に 3 つの値が格納されていません。" },
	  }
	},
	{ NoParamErrorID::RsBroken, {
		{ PPLang::EN,	"RS may be broken."			},
		{ PPLang::JP,	"RS が壊れているようです。" },
	  }
	},
	{ NoParamErrorID::RsBrokenTosShouldBeInt, {
		{ PPLang::EN,	"RS may be broken.\nTOS of RS should be an integer." },
		{ PPLang::JP,	"RS が壊れているようです。\n"
						"RS の TOS は int である必要があります。" },
	  }
	},
	{ NoParamErrorID::RsBrokenSecondShouldBeInt, {
		{ PPLang::EN,	"RS may be broken.\n"
						"The second of RS should be an integer." },
		{ PPLang::JP, 	"RS が壊れているようです。\n"
						"RS のセカンドは int である必要があります。" },
	  }
	},
	// { NoParamErrorID::RsNotEnough, {
	//	{ PPLang::EN,	"not enough element in RS." },
	//	{ PPLang::JP,	"RS に十分な要素が存在しません。" },
	//  }
	// },

	{ NoParamErrorID::IpsBroken, {
		{ PPLang::EN,	"IS may be broken."			},
		{ PPLang::JP, 	"IS が壊れているようです。" },
	  }
	},

	{ NoParamErrorID::SsBroken, {
		{ PPLang::EN,	"SS may be broken."			},
		{ PPLang::JP, 	"SS が壊れているようです。" },
	  }
	},

	{ NoParamErrorID::NoLastDefinedWord, {
		{ PPLang::EN,	"no lst defined word." },
		{ PPLang::JP,	"定義されたワードが存在しません。" },
	  }
	},

	{ NoParamErrorID::CStyleCommentMismatch, {
		{ PPLang::EN,	"C style comment mismatch. forget '/*'?" },
		{ PPLang::JP,	"C スタイルのコメントの整合性がとれていません。"
						"'/*' を書き忘れていませんか？" },
	  }
	},

	{ NoParamErrorID::HereDocumentMismatch, {
		{ PPLang::EN,	"here-document word mismatce. forget '<<<' or 'raw<<<'?" },
		{ PPLang::JP,	"ヒアドキュメント関連ワードの整合性がとれていません。"
						"'<<<' や 'raw<<<' を書き忘れていませんか？" },
	  }
	},

	{ NoParamErrorID::CanNotConvertToInt, {
		{ PPLang::EN,	"can not convert to int value." },
		{ PPLang::JP,	"整数値へ変換できません。" },
	  }
	},
	{ NoParamErrorID::CanNotConvertToIntDueToOverflow, {
		{ PPLang::EN,	"can not convert to int due to overflow." },
		{ PPLang::JP,	"オーバーフローのため int に変換できません。" },
	  }
	},
	{ NoParamErrorID::CanNotConvertToLongDueToOverflow, {
		{ PPLang::EN,	"can not convert to long due to overflow." },
		{ PPLang::JP,	"オーバーフローのため long に変換できません。" },
	  }
	},
	{ NoParamErrorID::CanNotConvertToFloatDueToOverflow, {
		{ PPLang::EN,	"can not convert to float due to overflow." },
		{ PPLang::JP,	"オーバーフローのため floatに変換できません。" },
	  }
	},
	{ NoParamErrorID::CanNotConvertToDoubleDueToOverflow, {
		{ PPLang::EN,	"can not convert to double due to overflow." },
		{ PPLang::JP,	"オーバーフローのため double に変換できません。" },
	  }
	},
	{ NoParamErrorID::CanNotConvertToSymbol, {
		{ PPLang::EN,	"can not convert to symbol." },
		{ PPLang::JP,	"シンボルに変換できません。" },
	  }
	},

	{ NoParamErrorID::ChanIsAlreadyClosed, {
		{ PPLang::EN,	"The channel is already cloased." },
		{ PPLang::JP,	"チャネルは既に閉じています。" },
	  }
	},
	{ NoParamErrorID::NoChanFromParent, {
		{ PPLang::EN,	"There are no channel from parent." },
		{ PPLang::JP,	"親からのチャネルは存在しません。" },
	  }
	},
	{ NoParamErrorID::NoChanToParent, {
		{ PPLang::EN,	"There are no channel to parent." },
		{ PPLang::JP,	"親へのチャネルは存在しません。" },
	  }
	},
	{ NoParamErrorID::NoChanFromChild, {
		{ PPLang::EN,	"There are no channel from child." },
		{ PPLang::JP,	"子からのチャネルは存在しません。" },
	  }
	},
	{ NoParamErrorID::NoChanFromPipe, {
		{ PPLang::EN,	"There are no channel from pipe." },
		{ PPLang::JP,	"パイプからのチャネルは存在しません。" },
	  }
	},

	{ NoParamErrorID::TosArrayNoElement, {
		{ PPLang::EN,	"The array at TOS has no elements." },
		{ PPLang::JP,	"TOS にある配列に要素が存在しません。" },
	  }
	},

	{ NoParamErrorID::TosListNoElement, {
		{ PPLang::EN,	"The list at TOS has no elements." },
		{ PPLang::JP,	"TOS にあるリストに要素が存在しません。" },
	  }
	},
	{ NoParamErrorID::TosListAtLeastCores, {
		{ PPLang::EN,	"The list at the TOS should have number of elements "
						"at least the number of cores or more." },
		{ PPLang::JP,	"TOS にあるリストには少なくともコア数以上の要素が必要です。" },
	  }
	},
	{ NoParamErrorID::TosLambdaList, {
		{ PPLang::EN,	"The list should be consist of lambdas." },
		{ PPLang::JP,	"リストがラムダで構成されていません。" },
	  }
	},
	{ NoParamErrorID::TosSymbolList, {
		{ PPLang::EN,	"The list should be consist of symbols." },
		{ PPLang::JP,	"リストがシンボルで構成されていません。" },
	  }
	},
	{ NoParamErrorID::TosShouldBeNonZero, {
		{ PPLang::EN,	"The TOS should be non zero value." },
		{ PPLang::JP,	"TOS はゼロでない値である必要があります。" },
	  }
	},

	// { NoParamErrorID::NoLoopBlock, {
	//	{ PPLang::EN,	"There are no loop block," },
	//	{ PPLang::JP,	"ループブロックが存在しません。" },
	//  }
	// },

	{ NoParamErrorID::ListMismatch, {
		{ PPLang::EN,	"Lists mismatch." },
		{ PPLang::JP,	"リストが対応しません。" },
	  }
	},

	{ NoParamErrorID::FormatDataMismatch, {
		{ PPLang::EN,	"The format and element data types are mismatch." },
		{ PPLang::JP,	"フォーマットと要素の型が一致しません。" },
	  }
	},

	{ NoParamErrorID::InvalidEnumList, {
		{ PPLang::EN,	"Invalid enum-list." },
		{ PPLang::JP,	"異常な enum リストです。" },
	  }
	},

	{ NoParamErrorID::FileIsAlreadyClosed, {
		{ PPLang::EN,	"The file is already cloased." },
		{ PPLang::JP,	"ファイルは既に閉じています。" },
	  }
	},
	{ NoParamErrorID::CanNotCloseFile, {
		{ PPLang::EN,	"Can not close the file." 					},
		{ PPLang::JP,	"ファイルを閉じることはできませんでした。"	},
	  }
	},
	{ NoParamErrorID::CanNotFlushFile, {
		{ PPLang::EN,	"Can not flush the file." 						   },
		{ PPLang::JP, "ファイルをフラッシュすることはできませんでした。" },
	  }
	},

	{ NoParamErrorID::RegexCollate, {
		{ PPLang::EN,	"The regex expression contained "
						"an invalid collating element name." },
		{ PPLang::JP,	"正規表現の式に不正な照合要素名が含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexCType, {
		{ PPLang::EN,	"The regex expression contained "
						"an invalid character class name." },
		{ PPLang::JP,	"正規表現の式に不正な文字クラス名が含まれている" },
	  }
	},
	{ NoParamErrorID::RegexEscape, {
		{ PPLang::EN,	"The regex expression contained an invalid escaped character, "
						"or a trailing escape." },
		{ PPLang::JP,	"正規表現の式に不正なエスケープ文字"
						"もしくは不正な後方エスケープが含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexBackRef, {
		{ PPLang::EN,	"The regex expression contained an invalid back reference." },
		{ PPLang::JP,	"正規表現の式に不正な後方参照が含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexBrack, {
		{ PPLang::EN,	"The regex expression contained mismatched brackets "
						"([ and ])." },
		{ PPLang::JP,	"正規表現の式に対応していない角カッコが含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexParen, {
		{ PPLang::EN,	"The regex expression contained mismatched parentheses "
						"(( and ))." },
		{ PPLang::JP,	"正規表現の式に対応していない丸カッコが含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexBrace, {
		{ PPLang::EN,	"The regex expression contained mismatched braces "
						"({ and })." },
		{ PPLang::JP,	"正規表現の式に対応していない波カッコが含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexBadBrace, {
		{ PPLang::EN,	"The regex expression contained "
						"an invalid range between braces ({ and })." },
		{ PPLang::JP,	"正規表現の式に不正な範囲が設定された波カッコが"
						"含まれています。" },
	  }
	},
	{ NoParamErrorID::RegexRange, {
		{ PPLang::EN,	"The regex expression contained an invalid character range." },
		{ PPLang::JP,	"正規表現の式に不正な文字範囲が含まれています。"
						"例えば [b-a] のようなものが不正な文字範囲となります。" },
	  }
	},
	{ NoParamErrorID::RegexSpace, {
		{ PPLang::EN,	"There was insufficient memory "
						"to convert the regex expression "
						"into a finite state machine." },
		{ PPLang::JP,	"メモリ不足のため与えられた正規表現の式を"
						"有限状態マシンに変換できませんでした。" },
	  }
	},
	{ NoParamErrorID::RegexBadRepeat, {
		{ PPLang::EN,	"The regex expression contained a repeat specifier "
						"(one of *?+{) that was not preceded "
						"by a valid regular expression." },
		{ PPLang::JP,	"有効な正規表現において、"
						"繰り返し*?+{が処理されませんでした。" },
	  }
	},
	{ NoParamErrorID::RegexComplexity, {
		{ PPLang::EN,	"The complexity of an attempted match "
						"against a regular expression exceeded a pre-set level." },
		{ PPLang::JP,	"正規表現の式の複雑さが事前に設定したレベルを超えています。" },
	  }
	},
	{ NoParamErrorID::RegexStack, {
		{ PPLang::EN, 	"There was insufficient memory to determine "
						"whether the regular expression could match "
						"the specified character sequence." },
		{ PPLang::JP,	"メモリ不足のため、正規表現が指定されたシーケンスに"
						"マッチするかどうかを判断できません。" },
	  }
	},
	{ NoParamErrorID::RegexUnknownError, {
		{ PPLang::EN,	"An unknown error occurred while interpreting "
						"the regex expression." },
		{ PPLang::JP,	"正規表現の解釈において、未知のエラーが発生しました。" },
	  }
	},

	// { NoParamErrorID::WpAtTosShouldPushABool, {
	//	{ PPLang::EN,	"Thw word at TOS should push a bool." },
	//	{ PPLang::JP, 	"TOS にあるワードは、bool 値を"
	//					"スタックに積まなければなりません。" },
	//  }
	// },
	// { NoParamErrorID::WpAtSecondShouldPushABool, {
	//	{ PPLang::EN,	"Thw word at the Second should push a bool." },
	//	{ PPLang::JP, "セカンドにあるワードは、bool 値を"
	//					"スタックに積まなければなりません。" },
	//  }
	// },
	{ NoParamErrorID::ShouldBeCompileMode, {
		{ PPLang::EN,	"this word should be use on the compile mode." },
		{ PPLang::JP, "本ワードはコンパイルモードで使用して下さい。" },
	  }
	},

	{ NoParamErrorID::SyntaxMismatchIf, {
		{ PPLang::EN,	"syntax mismatch. this word should use for 'if' block." },
		{ PPLang::JP,	"構文エラー。このワードは if ブロックにて使用して下さい。" },
	  }
	},
	{ NoParamErrorID::SyntaxMismatchFor, {
		{ PPLang::EN,	"syntax mismatch. "
						"this word should use for 'for+' or 'for-' block." },
		{ PPLang::JP,	"構文エラー。"
						"このワードは for+ または for- ブロックにて使用して下さい。" },
	  }
	},
	// { NoParamErrorID::SyntaxMismatchWhile, {
	//	{ PPLang::EN,	"syntax mismatch. "
	//					"this word should use for 'while' block." },
	//	{ PPLang::JP,	"構文エラー。"
	//					"このワードは while ブロックにて使用して下さい。" },
	//  }
	// },
	// { NoParamErrorID::SyntaxMismatchLoop, {
	//	{ PPLang::EN,	"syntax mismatch. "
	//					"this word should use for 'loop' block." },
	//	{ PPLang::JP,	"構文エラー。"
	//					"このワードは loop ブロックにて使用して下さい。" },
	//  }
	// },
	{ NoParamErrorID::SyntaxMismatchSwitchCond, {
		{ PPLang::EN,	"syntax mismatch. "
						"this word should use for 'switch' or 'cond' block." },
		{ PPLang::JP,	"構文エラー。"
						"このワードは switch または cond ブロックにて使用して下さい。" },
	  }
	},
	{ NoParamErrorID::SyntaxMismatchLeavableLoopGroup, {
		{ PPLang::EN,	"syntax mismatch. "
						"this word should use for iterative code-block." },
		{ PPLang::JP,	"構文エラー。"
						"このワードは繰り返し処理ブロックにて使用して下さい。" },
	  }
	},

	{ NoParamErrorID::SyntaxOpenIf, {
		{ PPLang::EN,	"Syntax error. 'if' block is not closed."   },
		{ PPLang::JP,	"構文エラー。if ブロックが閉じていません。" },
	  }
	},
	{ NoParamErrorID::SyntaxOpenFor, {
		{ PPLang::EN,	"Syntax error. 'for' block is not closed."   },
		{ PPLang::JP,	"構文エラー。for ブロックが閉じていません。" },
	  }
	},
	// { NoParamErrorID::SyntaxOpenWhile, {
	//	{ PPLang::EN,	"Syntax error. 'while' block is not closed."   },
	//	{ PPLang::JP,	"構文エラー。while ブロックが閉じていません。" },
	//  }
	// },
	// { NoParamErrorID::SyntaxOpenDo, {
	//	{ PPLang::EN,	"Syntax error. 'do' block is not closed."   },
	//	{ PPLang::JP,	"構文エラー。do ブロックが閉じていません。" },
	//  }
	// },
	{ NoParamErrorID::SyntaxOpenSwitch, {
		{ PPLang::EN,	"Syntax error. 'switch' block is not closed."   },
		{ PPLang::JP,	"構文エラー。switch ブロックが閉じていません。" },
	  }
	},

	{ NoParamErrorID::SsMiscIntInvalid, {
		{ PPLang::EN,	"System error. invalid value of MiscInt."   },
		{ PPLang::JP,	"システムエラー。無効な MiscInt 値です。" },
	  }
	},

	{ NoParamErrorID::InvalidValue, {
		{ PPLang::EN,	"Invalid value." },
		{ PPLang::JP,	"異常な値です。" },
	  }
	},

	{ NoParamErrorID::NoWordedDocol, {
		{ PPLang::EN,	"There are no worded-docol." },
		{ PPLang::JP,	"Worded-Docol が設定されていません。" },
	  }
	},
};

static InvalidTypeErrorMessageDict gInvalidTypeErrorMessageDict {
	{ InvalidTypeErrorID::TosInt, {
		{ PPLang::EN,	"TOS should be an int.\n"
               			"current TOS type is %s." },
		{ PPLang::JP,	"TOS が int ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	// { InvalidTypeErrorID::TosIntOrLong, {
	//	{ PPLang::EN,	"TOS should be an int or a long.\n"
    //        			"current TOS type is %s." },
	//	{ PPLang::JP, "TOS が int または long ではありません。\n"
    //            		"現在の TOS は %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::TosIntOrLongOrBigint, {
		{ PPLang::EN,	"TOS should be an int or a long or a big-int.\n"
            			"current TOS type is %s." },
		{ PPLang::JP,	"TOS が int または long もしくは big-int ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosDoublable, {
		{ PPLang::EN,	"TOS should be a number convertible to a double.\n"
            			"current TOS type is %s." },
		{ PPLang::JP, "TOS は double に変換可能な数値ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosNumber, {
		{ PPLang::EN,	"TOS should be a number.\n"
            			"current TOS type is %s." },
		{ PPLang::JP, "TOS は数値ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosBool, {
		{ PPLang::EN,	"TOS should be a bool.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS が bool ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosString, {
		{ PPLang::EN,	"TOS should be a string.\n"
						"current TOS type is %s." },
		{ PPLang::JP, "TOS が文字列ではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosStringOrWord, {
		{ PPLang::EN,	"TOS should be a string or word-ptr.\n"
						"current TOS type is %s." },
		{ PPLang::JP, 	"TOS が文字列またはワードへのポインタでありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosSymbol, {
		{ PPLang::EN,	"TOS should be a symbol.\n"
						"current TOS type is %s." },
		{ PPLang::JP, "TOS がシンボルではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosSymbolOrWord, {
		{ PPLang::EN,	"TOS should be a symbol or word-ptr.\n"
						"current TOS type is %s." },
		{ PPLang::JP, 	"TOS がシンボルまたはワードへのポインタでありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosSymbolOrString, {
		{ PPLang::EN,	"TOS should be a symbol or string.\n"
						"current TOS type is %s." },
		{ PPLang::JP, 	"TOS がシンボルまたは文字列ではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosSymbolOrStringOrWord, {
		{ PPLang::EN,	"TOS should be a symbol or a string or a word.\n"
						"current TOS type is %s." },
		{ PPLang::JP, 	"TOS がシンボルまたは文字列またはワードではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	// { InvalidTypeErrorID::TosSymbolOrList, {
	//	{ PPLang::EN,	"TOS should be a symbol or symbol-list.\n"
	//					"current TOS type is %s." },
	//	{ PPLang::JP, "TOS がシンボルもしくはシンボルリストではありません。\n"
    //           			"現在の TOS は %s です。" },
	//  }
	// },
	// { InvalidTypeErrorID::TosArray, {
	//	{ PPLang::EN,	"TOS should be an array."
    //            		"current TOS type is %s." },
	//	{ PPLang::JP,	"TOS が配列ではありません。"
    //            		"現在の TOS は %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::TosArrayOrList, {
		{ PPLang::EN,	"TOS should be an array or an list."
                		"current TOS type is %s." },
		{ PPLang::JP,	"TOS が配列またはリストではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosArrayOrListOrStringOrSymbol, {
		{ PPLang::EN,	"TOS should be an array,list,string or a symbol."
                		"current TOS type is %s." },
		{ PPLang::JP,	"TOS が配列、リスト、文字列またはシンボルではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosList, {
		{ PPLang::EN,	"TOS should be a list."
                		"current TOS type is %s." },
		{ PPLang::JP,	"TOS がリストではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosFile, {
		{ PPLang::EN,	"TOS should be a file.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS が file ではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosChannel, {
		{ PPLang::EN,	"TOS should be a Channel.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS がチャネルではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosContext, {
		{ PPLang::EN,	"TOS should be a Context.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS がコンテキストではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	// { InvalidTypeErrorID::TosBoolOrWp, {
	//	{ PPLang::EN,	"TOS should be a bool or a word pointer.\n"
    //            		"current TOS type is %s." },
	//	{ PPLang::JP, "TOS が bool もくくはワードポインタではありません。\n"
    //            		"現在の TOS は %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::TosNumberOrString, {
		{ PPLang::EN,	"TOS should be a number or a string.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS が数値または文字列ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosWp, {
		{ PPLang::EN,	"TOS should be a word pointer.\n"
                		"current TOS type is %s." },
		{ PPLang::JP, "TOS がワードポインタではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	// { InvalidTypeErrorID::TosWpOrStdCode, {
	//	{ PPLang::EN,	"TOS should be a word pointer or a std-code pointer.\n"
    //            		"current TOS type is %s." },
	//	{ PPLang::JP, "TOS がワードポインタもしくは標準コードポインタでは"
	//					"ありません。\n"
    //            		"現在の TOS は %s です。" },
	//  }
	// },
	// { InvalidTypeErrorID::TosAddress, {
	//	{ PPLang::EN,	"TOS should be an address." 
    //            		"current TOS type is %s." },
	//	{ PPLang::JP,	"TOS がアドレスではありません。" 
    //            		"現在の TOS は %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::TosParamDest, {
		{ PPLang::EN,	"TOS should be a param-dest." 
                		"current TOS type is %s." },
		{ PPLang::JP,	"TOS が param-dest ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosNewWord, {
		{ PPLang::EN,	"TOS should be a new-word." 
                		"current TOS type is %s." },
		{ PPLang::JP,	"TOS が new-word ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosWpOrNw, {
		{ PPLang::EN,	"TOS should be a word pointer or a new-word. " 
                		"Current TOS type is %s." },
		{ PPLang::JP,	"TOS が ワードもしくは new-word ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosWpOrStringOrKV, {
		{ PPLang::EN,	"TOS should be a word pointer , a string or an assoc. "
						"Current TOS type is %s." },
		{ PPLang::JP,	"TOS がワード、文字列または連想配列ではありません。"
						"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosWpOrStringOrSymbolOrKV, {
		{ PPLang::EN,	"TOS should be a word pointer , a string or a symbol or an assoc. "
						"Current TOS type is %s." },
		{ PPLang::JP,	"TOS がワード、文字列またはシンボルまたは連想配列ではありません。"
						"現在の TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosStdCode, {
		{ PPLang::EN,	"TOS should be a std-code. "
						"Current TOS type is %s." },
		{ PPLang::JP,	"TOS が標準コードではありません。"
						"現在の TOS は %s です。" },
	  }
	},

	{ InvalidTypeErrorID::TosContextOrContainer, {
		{ PPLang::EN,	"TOS should be a Context or a container "
						"(i.e. list, array or associateve array. "
						"Current TOS type is %s." },
		{ PPLang::JP,	"TOS がコンテキストまたはコンテナ"
						"（リスト、配列、連想配列）ではありません。"
						"現在の TOS は %s です。" },
	  }
	},

	{ InvalidTypeErrorID::TosCanNotConvertToBool, {
		{ PPLang::EN,	"The value at TOS could not be converted to bool."
						"The value type is %s." },
		{ PPLang::JP,	"TOS にある値を真偽型に変換できませんでした。"
						"この値の型は %s でした。" },
	  }
	},
	{ InvalidTypeErrorID::TosCanNotConvertToString, {
		{ PPLang::EN,	"The value at TOS could not be converted to string."
						"The value type is %s." },
		{ PPLang::JP,	"TOS にある値を文字列型に変換できませんでした。"
						"この値の型は %s でした。" },
	  }
	},


	{ InvalidTypeErrorID::SecondInt, {
		{ PPLang::EN,	"The Second should be an integer."
                		"Current the Second type is %s." },
		{ PPLang::JP,	"セカンドが整数ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondIntOrLongOrBigint, {
		{ PPLang::EN,	"The Second should be an integer or a long or a big-int.\n"
                		"Current Second type is %s." },
		{ PPLang::JP,	"セカンドが int または long または big-int ではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondBool, {
		{ PPLang::EN,	"The Second should be a bool."
                		"Current Second type is %s." },
		{ PPLang::JP,	"セカンドが bool ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondString, {
		{ PPLang::EN,	"The Second should be a string."
                		"Current Second type is %s." },
		{ PPLang::JP,	"セカンドが string ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondStringOrSymbol, {
		{ PPLang::EN,	"The Second should be a string or a symbol."
                		"Current Second type is %s." },
		{ PPLang::JP,	"セカンドが文字列またはシンボルではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	// { InvalidTypeErrorID::SecondArray, {
	//	{ PPLang::EN,	"The Second should be an array."
    //            		"Current Second type is %s." },
	//	{ PPLang::JP, "セカンドが配列ではありません。"
    //            		"現在のセカンドは %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::SecondList, {
		{ PPLang::EN,	"The Second should be a list."
                		"Current Second type is %s." },
		{ PPLang::JP,	"セカンドがリストではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondKV, {
		{ PPLang::EN,	"The Second should be an associative array. "
						"Current Second type is %s." },
		{ PPLang::JP,	"セカンドが連想配列ではありません。"
						"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondArrayOrList, {
		{ PPLang::EN,	"The second should be an array or a list."
                		"Current Second type is %s." },
		{ PPLang::JP, "スタックの 2 番目の要素が配列かリストではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondArrayOrListOrKV, {
		{ PPLang::EN,	"The second should be an array or a list "
						"or an associative array. "
                		"Current Second type is %s." },
		{ PPLang::JP, "スタックの 2 番目の要素が配列かリスト"
						"もしくは連想配列ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondFile, {
		{ PPLang::EN,	"The second should be an file."
						"current the Second type is %s.\n" },
		{ PPLang::JP,	"スタックの 2 番目の要素が配列かリストではありません。"
						"現在のセカンドは %s です。" },
	  }
	},
	// { InvalidTypeErrorID::SecondBoolOrWp, {
	//	{ PPLang::EN,	"The Second should be a bool or a word pointer.\n"
    //            		"current the Second type is %s.\n" },
	//	{ PPLang::JP, "セカンドが bool もくくはワードポインタではありません。\n"
    //            		"現在のセカンドは %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::SecondThreshold, {
		{ PPLang::EN,	"The Second should be a threshold.\n"
                		"current the Second type is %s.\n" },
		{ PPLang::JP, "セカンドが threshold ではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondWp, {
		{ PPLang::EN,	"The Second should be a word.\n"
                		"current the Second type is %s.\n" },
		{ PPLang::JP,	"セカンドがワードではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondContext, {
		{ PPLang::EN,	"The Second should be a Context.\n"
                		"current the Second type is %s.\n" },
		{ PPLang::JP,	"セカンドがコンテキストではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SecondCB, {
		{ PPLang::EN,	"The Second should be a Control-Block-Info.\n"
                		"current the Second type is %s.\n" },
		{ PPLang::JP,	"セカンドが制御ブロック情報ではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},

	{ InvalidTypeErrorID::ThirdBool, {
		{ PPLang::EN,	"The third should be an a bool."
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素が bool ではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::ThirdList, {
		{ PPLang::EN,	"The third should be a list."
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素がリストではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::ThirdString, {
		{ PPLang::EN,	"The third should be a string."
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素が文字列ではありません。"						
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::ThirdArrayOrListOrKV, {
		{ PPLang::EN,	"The third should be an array or a list "
						"or an associative array."
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素が配列かリスト"
						"もしくは連想配列ではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::ThirdWp, {
		{ PPLang::EN,	"The third should be a word. "
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素がワードではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::ThirdUserData, {
	  	{ PPLang::EN,	"The third should be an user-data. "
						"Current Third type is %s." },
		{ PPLang::JP,	"スタックの 3 番目の要素がユーザーデータではありません。"
						"現在の 3 番目の要素は %s です。" },
	  },
	},

	{ InvalidTypeErrorID::RsTosIntOrLongOrBigint, {
		{ PPLang::EN,	"The TOS on RS should be an int or a long or a big-int."
						"Current type is %s." },
		{ PPLang::JP,	"RS の TOS は int か long もしくは big-int で"
						"なければなりません。現在の型は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::RsTosNumber, {
		{ PPLang::EN,	"The TOS on RS should be an number. " 
						"Current type is %s." },
		{ PPLang::JP,	"RS の TOS は数値でなければなりません。現在の型は %s です。" }
	  }
	},

	{ InvalidTypeErrorID::RsTosWp, {
		{ PPLang::EN,	"RS's TOS should be a word pointer."
						"current TOS at RS type is %s." },
		{ PPLang::JP,	"RS の TOS がワードではありません。"
						"現在のリターンスタックの TOS は %s です。" },
	  }
	},
	// { InvalidTypeErrorID::RsTosAddress, {
	//	{ PPLang::EN,	"The RS's TOS should be an address."
	//					"current the TOS at RS type is %s." },
	//	{ PPLang::JP,	"リターンスタックの TOS がアドレス値ではありません。"
	//					"現在のリターンスタックの TOS は %s です。" },
	//  }
	// },
	{ InvalidTypeErrorID::RsTosThreshold, {
		{ PPLang::EN,	"The RS's TOS should be a threshold."
						"current the TOS at RS type is %s." },
		{ PPLang::JP,	"リターンスタックの TOS が閾値ではありません。"
						"現在のリターンスタックの TOS は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::RsSecondIntOrLongOrBigint, {
		{ PPLang::EN,	"the RS's second should be an int or a long or a big-int."
						"current the second at RS type is %s." },
		{ PPLang::JP,	"RS のセカンドが int または long もしくは big-int では"
						"ありません。"
						"現在のリターンスタックのセカンドは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::RsSecondWp, {
		{ PPLang::EN,	"the second should be a word pointer."
						"current the second at RS type is %s." },
		{ PPLang::JP,	"RS のセカンドがワードではありません。"
						"現在のリターンスタックのセカンドは %s です。" },
	  }
	},

	{ InvalidTypeErrorID::SsMiscInt, {
		{ PPLang::EN,	"The SS's TOS should be MiscInt."
						"current type is %s." },
		{ PPLang::JP,	"SS の TOS は MiscInt でなければなりません。"
						"現在の型は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::SsTosShouldBeLoop, {
		{ PPLang::EN,	"This word should be use in 'loop - repeat' block."
						"current syntax stack top is %s." },
		{ PPLang::JP,	"このワードは loop - repeat ブロックの中で使用する必要があります。"
						"現在のループは %s です。" },
	  }
	},

	{ InvalidTypeErrorID::TosCanNotConvertToDouble, {
		{ PPLang::EN,	"TOS can not convert to double."
						"current the TOS type is %s." },
		{ PPLang::JP,	"TOS を double に変換できません。"
                		"現在の TOS の型は %s です。" },
	  }
	},
	{ InvalidTypeErrorID::TosCanNotConvertToFloat, {
		{ PPLang::EN,	"TOS can not convert to float."
						"current the TOS type is %s." },
		{ PPLang::JP,	"TOS を float に変換できません。"
                		"現在の TOS の型は %s です。" },
	  }
	},

	{ InvalidTypeErrorID::FirstElementString, {
		{ PPLang::EN,	"the first element should be a string."
						"current type is %s." },
		{ PPLang::JP,	"最初の要素は string でなければなりません。"
                		"現在の型は %s です。" },
	  }
	},

	{ InvalidTypeErrorID::LvNumber, {
		{ PPLang::EN,	"the local variable should be a number type."
						"Current type is %s." },
		{ PPLang::JP,	"ローカル変数は数値型でなければなりません。"
                		"現在の型は %s です。" },
	  }
	},

	{ InvalidTypeErrorID::ElementShouldBeTuple, {
		{ PPLang::EN,	"The element should be a tuple."
						"The element type is %s." },
		{ PPLang::JP,	"要素はタプルでなければなりません。"
						"要素の型は %s でした。" },
	  }
	},

	{ InvalidTypeErrorID::ArrayElemContext, {
		{ PPLang::EN,	"The array element should be a Context."
						"The element type is %s." },
		{ PPLang::JP,	"配列の要素はコンテナでなければなりません。"
						"要素の型は %s でした。" },
	  }
	},
	{ InvalidTypeErrorID::ListElemContext, {
		{ PPLang::EN,	"The list element should be a Context."
						"The element type is %s." },
		{ PPLang::JP,	"リストの要素はコンテナでなければなりません。"
						"要素の型は %s でした。" },
	  }
	},
	{ InvalidTypeErrorID::KvElemContext, {
		{ PPLang::EN,	"The associative array element should be a Context."
						"The element type is %s." },
		{ PPLang::JP,	"連想配列の要素はコンテナでなければなりません。"
						"要素の型は %s でした。" },
	  }
	},

	{ InvalidTypeErrorID::CompiledLiteralShouldBeList, {
		{ PPLang::EN,	"The compiled literal should be a list.\n"
						"current literal type is %s." },
		{ PPLang::JP, 	"コンパイルされたリテラルがリストではありません。\n"
               			"現在のリテラルは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::CompiledLiteralShouldBeSymbolOrString, {
		{ PPLang::EN,	"The compiled literal should be a symbol or string.\n"
						"current literal type is %s." },
		{ PPLang::JP, 	"コンパイルされたリテラルがシンボルまたは文字列では"
						"ありません。\n"
               			"現在のリテラルは %s です。" },
	  }
	},
	{ InvalidTypeErrorID::CompiledLiteralShouldBeSymbolOrStringOrList, {
		{ PPLang::EN,	"The compiled literal should be a symbol or a string or a list.\n"
						"current literal type is %s." },
		{ PPLang::JP, 	"コンパイルされたリテラルがシンボルまたは文字列またはリスト"
						"ではありません。\n"
               			"現在のリテラルは %s です。" },
	  }
	},
};

static InvalidValueErrorMessageDict gInvalidValueErrorMessageDict {
	{ InvalidValueErrorID::TosNoSuchAKey, {
		{ PPLang::EN,	"no such a key (TOS value is %s)." },
		{ PPLang::JP, 	"指定されたキーは存在しません (TOS の値は %s です）。" },
	  }
	},
};

static InvalidTypeTosSecondErrorMessageDict gInvalidTypeTosSecondErrorMessageDict {
	{ InvalidTypeTosSecondErrorID::BothDataInvalid, {
		{ PPLang::EN,	"invalid data type.\n"
						"TOS is %s, Second is %s." },
		{ PPLang::JP,	"データ型が異常です。\n"
						"TOS は %s で、セカンドは %s です。" },
	  }
	},
	{ InvalidTypeTosSecondErrorID::OutOfSupportTosSecond, {
		{ PPLang::EN,	"out of support (sorry)..\n"
						"TOS is %s, The second is %s." },
		{ PPLang::JP,	"サポート外の組み合わせです。\n"
						"TOS は %s で、セカンドは %s です。" },
	  }
	},
};

static InvalidTypeStrTvTvErrorMessageDict gInvalidTypeStrTvTvErrorMessageDict {
	{ InvalidTypeStrTvTvErrorID::ForOp, {
		{ PPLang::EN,	"invalid data type for '%s'.\n"
						"1st argument type is %s,"
						"2nd argument type is %s." },
		{ PPLang::JP,	"演算 '%s' が実行できません。\n"
						"第 1 引数の型は %s で、"
						"第 2 引数の型は %s です。" },
	  }
	},
};

static ErrorWithStringMessageDict gErrorWithStringMessageDict {
	{ ErrorIdWithString::InvalidToken, {
		{ PPLang::EN,	"Invalid token: '%s'." },
		{ PPLang::JP,	"無効なトークン '%s' です。" },
	  }
	},
	{ ErrorIdWithString::InvalidFormat, {
	  	{ PPLang::EN,	"Invalid format: '%s'." },
		{ PPLang::JP,	"無効な書式 '%s' です。" },
	  }
	},
	{ ErrorIdWithString::CanNotFindTheWord, {
		{ PPLang::EN,	"Can not find the word '%s'." },
		{ PPLang::JP,	"ワード '%s' が見つかりません。" },
	  }
	},
	{ ErrorIdWithString::CanNotFindTheLocalVar, {
		{ PPLang::EN,	"Can not find the local variable '%s'." },
		{ PPLang::JP,	"ローカル変数 '%s' が見つかりません。" },
	  }
	},
	{ ErrorIdWithString::CanNotInstallUnoverWritable, {
		{ PPLang::EN,	"Can not install the word '%s',"
						"this word is already installed and it is an unoverwritable." },
		{ PPLang::JP,	"ワード '%s' は上書きできないワードとして"
						"既にインストールされています。" },
	  }
	},
	{ ErrorIdWithString::CanNotMakeShortenNameUnoverWritable, {
		{ PPLang::EN,	"Can not make a shorten word name '%s',"
						"this name is already used and it is an unoverwritable."
	    				"If you does not want to define shortend word,"
						"please use '::' instead of ':'." 	},
		{ PPLang::JP,	"短縮名 '%s' は上書きできないワードとして"
						"既に使用されています。" 
						"短縮形を使用しないのであれば、: ではなく :: を"
						"使用するようにして下さい。" },
	  }
	},

	{ ErrorIdWithString::AmbigousWordName, {
		{ PPLang::EN,	"the word name '%s' is anbigous."
						"The follow packages have this name's word: " },
		{ PPLang::JP,	"ワード名 '%s' は曖昧です。"
						"以下のパッケージでこの名前は使用されています：" },
	  }
	},

	{ ErrorIdWithString::AlreadyDefined, {
		{ PPLang::EN,	"'%s' is already defined." },
		{ PPLang::JP,	"'%s' は既に定義されています。" },
	  }
	},

	{ ErrorIdWithString::AlreadyRegisteredLocalVar, {
		{ PPLang::EN,	"the local variable of '%s' is already registered." },
		{ PPLang::JP,	"ローカル変数 '%s' は既に登録されています。" },
	  }
	},

	{ ErrorIdWithString::NoSuchLocalVar, {
		{ PPLang::EN,	"no such local variable '%s'." },
		{ PPLang::JP,	"ローカル変数 '%s' は存在しません。" },
	  }
	},

	{ ErrorIdWithString::CanNotOpenFile, {
		{ PPLang::EN,	"Can not open the file '%s'." },
		{ PPLang::JP,	"ファイル '%s' が開けません。" },
	  }
	},
	{ ErrorIdWithString::CanNotCreateFile, {
		{ PPLang::EN,	"Can not create the file '%s'." },
		{ PPLang::JP,	"ファイル '%s' を作成できません。" },
	  }
	},
	{ ErrorIdWithString::CanNotCloseTheFile, {
		{ PPLang::EN,	"Can not close the file '%s'." },
		{ PPLang::JP,	"ファイル '%s' をクローズできません。" },
	  }
	},
	{ ErrorIdWithString::CanNotReadFile, {
		{ PPLang::EN,	"Can not read the file '%s'." },
		{ PPLang::JP,	"ファイル '%s' を読み込めません。" },
	  }
	},
	{ ErrorIdWithString::CanNotFindTheEntryPoint, {
		{ PPLang::EN,	"Can not read the entry point in '%s'." },
		{ PPLang::JP,	"ファイル '%s' のエントリポイントが見つかりません。" },
	  }
	},

	{ ErrorIdWithString::NotEnoughParamValues, {
		{ PPLang::EN,	"There are not values for to the local variable '%s'." },
		{ PPLang::JP,	"ローカル変数 %s に対する値がありません。" },
	  }
	},

	{ ErrorIdWithString::StringShouldBeOneChar, {
	  	{ PPLang::EN,	"String should have one character."
						"The string is '%s'." },
		{ PPLang::JP,	"文字列は 1 文字のみからなるものでなければなりｍせん。"
						"現在の文字列の内容は '%s' です。" },
	  }
	},
};

static ErrorWithIntMessageDict gErrorWithIntMessageDict {
	{ ErrorIdWithInt::TosPositiveInt, {
		{ PPLang::EN,	"TOS should be a positive integer."
						"Current value is %d." },
		{ PPLang::JP,	"TOS が正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ ErrorIdWithInt::SecondPositiveInt, {
		{ PPLang::EN,	"The second should be a positive integer."
						"Current value is %d." },
		{ PPLang::JP,	"セカンドが正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ ErrorIdWithInt::TosNonNegative, {
		{ PPLang::EN,	"TOS should be a zero or positive integer."
						"Current value is %d." },
		{ PPLang::JP,	"TOS がゼロまたは正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ ErrorIdWithInt::SecondNonNegative, {
		{ PPLang::EN,	"The second should be a zero or positive integer."
						"current value is %d." },
		{ PPLang::JP,	"セカンドがゼロまたは正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},

	{ ErrorIdWithInt::InvalidLocalSlotIndex, {
		{ PPLang::EN,	"Invalid slot index for local variables: %d." },
		{ PPLang::JP,	"無効なローカル変数用スロットインデックスです:%d" },
	  }
	},
};

static ErrorWith2IntMessageDict gErrorWith2IntMessageDict {
	// { ErrorIdWith2int::DsNotEnoughData, {
	//	{ PPLang::EN,	"DS should have %d elements."
	//					"There are only %d elements." },
	//	{ PPLang::JP,	"DS には少なくとも %d 個の要素が存在する必要があります。"
	//					"現在は %d 個の要素しか DS に存在しません。" },
	//  }
	// },

	{ ErrorIdWith2int::DepthIndexOutOfRange, {
		{ PPLang::EN,	"stack index is out of range.\n"
               			"the depth range is 0 to %d, but index is %d." },
		{ PPLang::JP,	"インデックスの範囲が異常です。\n"
						"スタックのインデックス範囲は 1 から %d までですが、"
						"指定されたインデックス値は %d です。" },

	  }
	},
	{ ErrorIdWith2int::StringIndexOutOfRange, {
		{ PPLang::EN,	"index is out of range.\n"
						"the string index range is 0 to %d, "
						"but the given index is %d.\n" },
		{ PPLang::JP,	"インデックスの範囲が異常です。"
                		"文字列の位置として使用できるのは 0 から %d 未満ですが、"
						"指定されたインデックス値は %d です。" },
	  }
	},
	{ ErrorIdWith2int::ArrayIndexOutOfRange, {
		{ PPLang::EN,	"index is out of range.\n"
                		"the array range is 0 to %d, but the given index is %d.\n" },
		{ PPLang::JP,	"インデックスの範囲が異常です。"
                		"配列のインデックスの範囲は 0 から %d までですが、"
						"指定されたインデックス値は %d です。" },
	  }
	},
	{ ErrorIdWith2int::ListIndexOutOfRange, {
		{ PPLang::EN,	"index is out of range.\n"
                		"the list range is 0 to %d, but the given index is %d." },
		{ PPLang::JP,	"インデックスの範囲が異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定されたインデックス値は %d です。" },
	  }
	},
	{ ErrorIdWith2int::ListStartIndexOutOfRange, {
		{ PPLang::EN, "the start index is out of range.\n"
                		"the list range is 0 to %d, but the index is %d." },
		{ PPLang::JP,	"開始位置として与えられたインデックスの範囲が異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定された開始位置は %d です。" },
	  }
	},
	{ ErrorIdWith2int::ListLengthIndexOutOfRange, {
		{ PPLang::EN,	"the start+length index is out of range.\n"
                		"the list range is 0 to %d, but the given index is %d." },
		{ PPLang::JP,	"開始位置+長さとして与えられたインデックスの範囲が"
						"異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定された位置は %d です。" },
	  }
	},
};

static PPLang gLang=PPLang::JP;

static Mutex gErrorMessageMutex;

static void beginErrorMessage(const Context& inContext);
static void endErrorMessage(Context& inContext);
static void printCalleeInfo(Context& inContext);
static const char *getErrStr();

PP_API void InitErrorMessage() {
	initMutex(gErrorMessageMutex);
}

bool Error(Context& inContext,const NoParamErrorID inErrorID) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	const char *message=NULL;

	if(gNoParamErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 1-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gNoParamErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 1-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: %s\n",errorStr,wordName,message);
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,
		   const InvalidTypeErrorID inErrorID,const std::string& inStr) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gInvalidTypeErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 2-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 2-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr.c_str());
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,
		   const InvalidValueErrorID inErrorID,const std::string& inStr) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gInvalidValueErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error(InvalidValue 1)"
					   " (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidValueErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error(InvalidValue 2)"
					   " (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr.c_str());
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,
		   const ErrorIdWithString inErrorID,const char *inStr) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gErrorWithStringMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 3-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWithStringMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 3-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr);
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,const InvalidTypeTosSecondErrorID inErrorID,
		   const TypedValue& inTos,const TypedValue& inSecond) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gInvalidTypeTosSecondErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 4-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeTosSecondErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 4-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inTos.GetTypeStr().c_str(),
							   inSecond.GetTypeStr().c_str());
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,const InvalidTypeStrTvTvErrorID inErrorID,
		   const std::string& inStr,const TypedValue inTV1,const TypedValue& inTV2) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	
	if(gInvalidTypeStrTvTvErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 5-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeStrTvTvErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 5-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr.c_str(),
							   inTV1.GetTypeStr().c_str(),
							   inTV2.GetTypeStr().c_str());
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,const ErrorIdWithInt inErrorID,
		   const int inIntValue) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	if(gErrorWithIntMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 6-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWithIntMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 6-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inIntValue);
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

bool Error(Context& inContext,const ErrorIdWith2int inErrorID,
		   const int inIntValue1,const int inIntValue2) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	if(gErrorWith2IntMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 7-1 (inErrorID=%d).\n",(int)inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWith2IntMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,
				"SYSTEM ERROR at Error func 7-2 (inErrorID=%d).\n",(int)inErrorID);
		fprintf(stderr,"gLang=%d\n",(int)gLang);
		exit(-1);
	}

	beginErrorMessage(inContext);
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inIntValue1,inIntValue2);
		fprintf(stderr,"\n");
	endErrorMessage(inContext);
	return false;
}

static void beginErrorMessage(const Context& inContext) {
	Lock(gErrorMessageMutex);
	if( gErrorMessageWithTrace ) { ShowTrace(inContext); }
}
static void endErrorMessage(Context& inContext) {
	Unlock(gErrorMessageMutex);
}

static void printCalleeInfo(Context& inContext) {
	if(inContext.IS.size()>0) {
		const Word**& callee=inContext.IS.back();
		fprintf(stderr,"In word '%s': ",(*callee)->longName.c_str());
	}
}

static const char *getErrStr() {
	const char *ret=NULL;
	switch(gLang) {
		case PPLang::EN:	ret="ERROR";	break;
		case PPLang::JP:	ret="エラー";	break;
	}
	return ret;
}

