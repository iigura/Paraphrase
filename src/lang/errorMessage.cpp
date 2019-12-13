#include <stdio.h>

#include "typedValue.h"
#include "paraphrase.h"
#include "externals.h"
#include "word.h"
#include "stack.h"
#include "context.h"
#include "errorID.h"

typedef std::unordered_map<NoParamErrorID,
			std::unordered_map<PP_Lang,std::string> > NoParamErrorMessageDict;

typedef std::unordered_map<InvalidTypeErrorID,
			std::unordered_map<PP_Lang,std::string> > InvalidTypeErrorMessageDict;

typedef std::unordered_map<InvalidTypeTosSecondErrorID,
			std::unordered_map<PP_Lang,std::string> >
				InvalidTypeTosSecondErrorMessageDict;

typedef std::unordered_map<InvalidTypeStrTvTvErrorID,
			std::unordered_map<PP_Lang,std::string> >
				InvalidTypeStrTvTvErrorMessageDict;

typedef std::unordered_map<ErrorID_withString,
			std::unordered_map<PP_Lang,std::string> > ErrorWithStringMessageDict;

typedef std::unordered_map<ErrorID_withInt,
			std::unordered_map<PP_Lang,std::string> > ErrorWithIntMessageDict;

typedef std::unordered_map<ErrorID_with2int,
			std::unordered_map<PP_Lang,std::string> > ErrorWith2IntMessageDict;

static NoParamErrorMessageDict gNoParamErrorMessageDict {
	{ E_SYSTEM_ERROR, {
		{ PPL_ENGLISH,	"SYSTEM ERROR." },
		{ PPL_JAPANESE,	"システムエラー。" },
	  }
	},

	{ E_SYSTEM_ERROR, {
		{ PPL_ENGLISH,	"Illegal LVOP." },
		{ PPL_JAPANESE,	"不正な LVOP です" },
	  }
	},

	{ E_SHOULD_BE_EXECUTED_IN_DEFINITION, {
		{ PPL_ENGLISH,	"should be executed in definition "
						"(forgot ':' or {{ ... }} ?)." },
		{ PPL_JAPANESE,	"ワード定義中に実行する必要があります"
						"（ : や {{ ... }} を忘れましたか？)" },
	  }
	},
	{ E_INVALID_USE, {
		{ PPL_ENGLISH,	"The usage of the Word is incorrect (see word reference)." },
		{ PPL_JAPANESE,	"ワードの使用方法が間違っています"
						"（ワードリファレンスを参照して下さい）。" },
	  }
	},

	{ E_SORRY_NOT_SUPPORT_THIS_OPERATION, {
		{ PPL_ENGLISH,	"sorry, can not support this operation.." },
		{ PPL_JAPANESE,	"この操作は現段階ではまだサポートされていません。" },
	  }
	},
	{ E_OPEN_DOUBLE_QUOTE, {
		{ PPL_ENGLISH,	"open string about double-quote." },
		{ PPL_JAPANESE, "ダブルクォーテーションが閉じていません。"},
	  }
	},
	{ E_OPEN_SINGLE_QUOTE, {
		{ PPL_ENGLISH,	"open string about single-quote." },
		{ PPL_JAPANESE, "シングルクォーテーションが閉じていません。"},
	  }
	},
	{ E_INVALID_WORD_NAME, {
		{ PPL_ENGLISH,	"invalid word name (might not it start with the '$' sign?)" },
		{ PPL_JAPANESE,	"有効なワード名ではありません"
						"（ワード名が '$' 記号で始まっていませんか？）" },
	  }
	},
	{ E_THREAD_MAY_BE_BROKEN, {
		{ PPL_ENGLISH,	"thread may be broken." },
		{ PPL_JAPANESE,	"スレッドの情報が異常です。" },
	  }
	},

	{ E_LOCAL_VAR_USE_ONLY_WORD_DEF, {
		{ PPL_ENGLISH,	"local variables can only use in word definition." },
		{ PPL_JAPANESE,	"ローカル変数はワード定義中にのみ使用できます。" },
	  }
	},

	{ E_NO_LOCAL_VAR_FRAME, {
		{ PPL_ENGLISH,	"no local variable frames." },
		{ PPL_JAPANESE,	"ローカル変数用のフレームが存在しません。" },
	  }
	},

	{ E_DS_IS_EMPTY, {
		{ PPL_ENGLISH,	"DS is empty." },
		{ PPL_JAPANESE, "DS が空です。"},
	  }
	},
	{ E_DS_AT_LEAST_2, {
		{ PPL_ENGLISH,	"DS should have two elements at least."},
		{ PPL_JAPANESE,	"DS に 2 つの値が格納されていません。" },
	  }
	},
	{ E_DS_AT_LEAST_3, {
		{ PPL_ENGLISH,	"DS should have three elements at least."},
		{ PPL_JAPANESE,	"DS に 3 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_4, {
		{ PPL_ENGLISH,	"DS should have four elements at least."},
		{ PPL_JAPANESE,	"DS に 4 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_5, {
		{ PPL_ENGLISH,	"DS should have five elements at least."},
		{ PPL_JAPANESE,	"DS に 5 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_6, {
		{ PPL_ENGLISH,	"DS should have six elements at least."},
		{ PPL_JAPANESE,	"DS に 6 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_7, {
		{ PPL_ENGLISH,	"DS should have seven elements at least."},
		{ PPL_JAPANESE,	"DS に 7 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_8, {
		{ PPL_ENGLISH,	"DS should have eight elements at least."},
		{ PPL_JAPANESE,	"DS に 8 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_9, {
		{ PPL_ENGLISH,	"DS should have nine elements at least."},
		{ PPL_JAPANESE,	"DS に 9 つの値が格納されていません。"	 },
	  }
	},
	{ E_DS_AT_LEAST_10, {
		{ PPL_ENGLISH,	"DS should have ten elements at least."},
		{ PPL_JAPANESE,	"DS に 10 個の値が格納されていません。"	 },
	  }
	},

	{ E_RS_IS_EMPTY, {
		{ PPL_ENGLISH,	"RS is empty."  },
		{ PPL_JAPANESE,	"RS が空です。" },
	  }
	},
	{ E_RS_AT_LEAST_2, {
		{ PPL_ENGLISH,	"RS should have two elements at least."},
		{ PPL_JAPANESE,	"RS に 2 つの値が格納されていません。" },
	  }
	},
	{ E_RS_AT_LEAST_3, {
		{ PPL_ENGLISH,	"RS should have three elements at least."},
		{ PPL_JAPANESE,	"RS に 3 つの値が格納されていません。" },
	  }
	},
	{ E_RS_BROKEN, {
		{ PPL_ENGLISH,	"RS may be broken."			},
		{ PPL_JAPANESE, "RS が壊れているようです。" },
	  }
	},
	{ E_RS_BROKEN_TOS_SHOULD_BE_INT, {
		{ PPL_ENGLISH,	"RS may be broken.\nTOS of RS should be an integer." },
		{ PPL_JAPANESE,	"RS が壊れているようです。\n"
						"RS の TOS は int である必要があります。" },
	  }
	},
	{ E_RS_BROKEN_SECOND_SHOULD_BE_INT, {
		{ PPL_ENGLISH,	"RS may be broken.\n"
						"The second of RS should be an integer." },
		{ PPL_JAPANESE, "RS が壊れているようです。\n"
						"RS のセカンドは int である必要があります。" },
	  }
	},
	{ E_RS_NOT_ENOUGH, {
		{ PPL_ENGLISH,	"not enough element in RS." },
		{ PPL_JAPANESE,	"RS に十分な要素が存在しません。" },
	  }
	},

	{ E_IS_BROKEN, {
		{ PPL_ENGLISH,	"IS may be broken."			},
		{ PPL_JAPANESE, "IS が壊れているようです。" },
	  }
	},

	{ E_SS_BROKEN, {
		{ PPL_ENGLISH,	"SS may be broken."			},
		{ PPL_JAPANESE, "SS が壊れているようです。" },
	  }
	},

	{ E_NO_LAST_DEFINED_WORD, {
		{ PPL_ENGLISH,	"no lst defined word." },
		{ PPL_JAPANESE,	"定義されたワードが存在しません。" },
	  }
	},

	{ E_C_STYLE_COMMENT_MISMATCH, {
		{ PPL_ENGLISH,	"C style comment mismatch. forget '/*'?" },
		{ PPL_JAPANESE,	"C スタイルのコメントの整合性がとれていません。"
						"'/*' を書き忘れていませんか？" },
	  }
	},

	{ E_CAN_NOT_CONVERT_TO_INT, {
		{ PPL_ENGLISH,	"can not convert to int value." },
		{ PPL_JAPANESE,	"整数値へ変換できません。" },
	  }
	},
	{ E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW, {
		{ PPL_ENGLISH,	"can not convert to int due to overflow." },
		{ PPL_JAPANESE,	"オーバーフローのため int に変換できません。" },
	  }
	},
	{ E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW, {
		{ PPL_ENGLISH,	"can not convert to long due to overflow." },
		{ PPL_JAPANESE,	"オーバーフローのため long に変換できません。" },
	  }
	},
	{ E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW, {
		{ PPL_ENGLISH,	"can not convert to double due to overflow." },
		{ PPL_JAPANESE,	"オーバーフローのため double に変換できません。" },
	  }
	},
	{ E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW, {
		{ PPL_ENGLISH,	"can not convert to float due to overflow." },
		{ PPL_JAPANESE,	"オーバーフローのため floatに変換できません。" },
	  }
	},
	{ E_CAN_NOT_CONVERT_TO_SYMBOL, {
		{ PPL_ENGLISH,	"can not convert to symbol." },
		{ PPL_JAPANESE,	"シンボルに変換できません。" },
	  }
	},

	{ E_CHAN_IS_ALREADY_CLOSED, {
		{ PPL_ENGLISH,	"The channel is already cloased." },
		{ PPL_JAPANESE,	"チャネルは既に閉じています。" },
	  }
	},
	{ E_NO_CHAN_FROM_PARENT, {
		{ PPL_ENGLISH,	"There are no channel from parent." },
		{ PPL_JAPANESE,	"親からのチャネルは存在しません。" },
	  }
	},
	{ E_NO_CHAN_TO_PARENT, {
		{ PPL_ENGLISH,	"There are no channel to parent." },
		{ PPL_JAPANESE,	"親へのチャネルは存在しません。" },
	  }
	},
	{ E_NO_CHAN_FROM_CHILD, {
		{ PPL_ENGLISH,	"There are no channel from child." },
		{ PPL_JAPANESE,	"子からのチャネルは存在しません。" },
	  }
	},
	{ E_NO_CHAN_FROM_PIPE, {
		{ PPL_ENGLISH,	"There are no channel from pipe." },
		{ PPL_JAPANESE,	"パイプからのチャネルは存在しません。" },
	  }
	},

	{ E_TOS_LIST_NO_ELEMENT, {
		{ PPL_ENGLISH,	"The list at TOS has no elements." },
		{ PPL_JAPANESE,	"TOS にあるリストに要素が存在しません。" },
	  }
	},
	{ E_TOS_LIST_AT_LEAST_CORES, {
		{ PPL_ENGLISH,	"The list at the TOS should have number of elements "
						"at least the number of cores or more." },
		{ PPL_JAPANESE,	"TOS にあるリストには少なくともコア数以上の要素が必要です。" },
	  }
	},
	{ E_TOS_LAMBDA_LIST, {
		{ PPL_ENGLISH,	"The list should be consist of lambdas." },
		{ PPL_JAPANESE,	"リストがラムダで構成されていません。" },
	  }
	},
	{ E_TOS_SYMBOL_LIST, {
		{ PPL_ENGLISH,	"The list should be consist of symbols." },
		{ PPL_JAPANESE,	"リストがシンボルで構成されていません。" },
	  }
	},
	{ E_TOS_SHOULD_BE_NONZERO, {
		{ PPL_ENGLISH,	"The TOS should be non zero value." },
		{ PPL_JAPANESE,	"TOS はゼロでない値である必要があります。" },
	  }
	},

	{ E_LIST_MISMATCH, {
		{ PPL_ENGLISH,	"Lists mismatch." },
		{ PPL_JAPANESE,	"リストが対応しません。" },
	  }
	},

	{ E_FORMAT_DATA_MISMATCH, {
		{ PPL_ENGLISH,	"The format and element data types are mismatch." },
		{ PPL_JAPANESE,	"フォーマットと要素の型が一致しません。" },
	  }
	},

	{ E_FILE_IS_ALREADY_CLOSED, {
		{ PPL_ENGLISH,	"The file is already cloased." },
		{ PPL_JAPANESE,	"ファイルは既に閉じています。" },
	  }
	},
	{ E_CAN_NOT_CLOSE_FILE, {
		{ PPL_ENGLISH,	"Can not close the file." 					},
		{ PPL_JAPANESE,	"ファイルを閉じることはできませんでした。"	},
	  }
	},
	{ E_CAN_NOT_FLUSH_FILE, {
		{ PPL_ENGLISH,	"Can not flush the file." 						   },
		{ PPL_JAPANESE, "ファイルをフラッシュすることはできませんでした。" },
	  }
	},
	{ E_WP_AT_TOS_SHOULD_PUSH_A_BOOL, {
		{ PPL_ENGLISH,	"Thw word at TOS should push a bool." },
		{ PPL_JAPANESE, "TOS にあるワードは、bool 値を"
						"スタックに積まなければなりません。" },
	  }
	},
	{ E_WP_AT_SECOND_SHOULD_PUSH_A_BOOL, {
		{ PPL_ENGLISH,	"Thw word at the Second should push a bool." },
		{ PPL_JAPANESE, "セカンドにあるワードは、bool 値を"
						"スタックに積まなければなりません。" },
	  }
	},
	{ E_SHOULD_BE_COMPILE_MODE, {
		{ PPL_ENGLISH,	"this word should be use on the compile mode." },
		{ PPL_JAPANESE, "本ワードはコンパイルモードで使用して下さい。" },
	  }
	},

	{ E_SYNTAX_MISMATCH_IF, {
		{ PPL_ENGLISH,	"syntax mismatch. this word should use for 'if' block." },
		{ PPL_JAPANESE,	"構文エラー。このワードは if ブロックにて使用して下さい。" },
	  }
	},
	{ E_SYNTAX_MISMATCH_FOR, {
		{ PPL_ENGLISH,	"syntax mismatch. "
						"this word should use for 'for+' or 'for-' block." },
		{ PPL_JAPANESE,	"構文エラー。"
						"このワードは for+ または for- ブロックにて使用して下さい。" },
	  }
	},
	{ E_SYNTAX_MISMATCH_WHILE, {
		{ PPL_ENGLISH,	"syntax mismatch. "
						"this word should use for 'while' block." },
		{ PPL_JAPANESE,	"構文エラー。"
						"このワードは while ブロックにて使用して下さい。" },
	  }
	},
	{ E_SYNTAX_MISMATCH_DO, {
		{ PPL_ENGLISH,	"syntax mismatch. "
						"this word should use for 'do' block." },
		{ PPL_JAPANESE,	"構文エラー。"
						"このワードは do ブロックにて使用して下さい。" },
	  }
	},
	{ E_SYNTAX_MISMATCH_SWITCH, {
		{ PPL_ENGLISH,	"syntax mismatch. "
						"this word should use for 'switch' block." },
		{ PPL_JAPANESE,	"構文エラー。"
						"このワードは switch ブロックにて使用して下さい。" },
	  }
	},

	{ E_SYNTAX_OPEN_IF, {
		{ PPL_ENGLISH,	"Syntax error. 'if' block is not closed."   },
		{ PPL_JAPANESE,	"構文エラー。if ブロックが閉じていません。" },
	  }
	},
	{ E_SYNTAX_OPEN_FOR, {
		{ PPL_ENGLISH,	"Syntax error. 'for' block is not closed."   },
		{ PPL_JAPANESE,	"構文エラー。for ブロックが閉じていません。" },
	  }
	},
	{ E_SYNTAX_OPEN_WHILE, {
		{ PPL_ENGLISH,	"Syntax error. 'while' block is not closed."   },
		{ PPL_JAPANESE,	"構文エラー。while ブロックが閉じていません。" },
	  }
	},
	{ E_SYNTAX_OPEN_DO, {
		{ PPL_ENGLISH,	"Syntax error. 'do' block is not closed."   },
		{ PPL_JAPANESE,	"構文エラー。do ブロックが閉じていません。" },
	  }
	},
	{ E_SYNTAX_OPEN_SWITCH, {
		{ PPL_ENGLISH,	"Syntax error. 'switch' block is not closed."   },
		{ PPL_JAPANESE,	"構文エラー。switch ブロックが閉じていません。" },
	  }
	},

	{ E_SS_MISCINT_INVALID, {
		{ PPL_ENGLISH,	"System error. invalid value of MiscInt."   },
		{ PPL_JAPANESE,	"システムエラー。無効な MiscInt 値です。" },
	  }
	},
};

static InvalidTypeErrorMessageDict gInvalidTypeErrorMessageDict {
	{ E_TOS_INT, {
		{ PPL_ENGLISH,	"TOS should be an int.\n"
               			"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が int ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_INT_OR_LONG, {
		{ PPL_ENGLISH,	"TOS should be an int or a long.\n"
            			"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が int または long ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_INT_OR_LONG_OR_BIGINT, {
		{ PPL_ENGLISH,	"TOS should be an int or a long or a big-int.\n"
            			"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が int または long もしくは big-int ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_DOUBLABLE, {
		{ PPL_ENGLISH,	"TOS should be a number convertible to a double.\n"
            			"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS は double に変換可能な数値ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_NUMBER, {
		{ PPL_ENGLISH,	"TOS should be a number.\n"
            			"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS は数値ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_BOOL, {
		{ PPL_ENGLISH,	"TOS should be a bool.\n"
                		"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が bool ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_STRING, {
		{ PPL_ENGLISH,	"TOS should be a string.\n"
						"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が文字列ではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_SYMBOL, {
		{ PPL_ENGLISH,	"TOS should be a symbol.\n"
						"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS がシンボルではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_SYMBOL_OR_LIST, {
		{ PPL_ENGLISH,	"TOS should be a symbol or symbol-list.\n"
						"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS がシンボルもしくはシンボルリストではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_ARRAY, {
		{ PPL_ENGLISH,	"TOS should be an array."
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が配列ではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_ARRAY_OR_LIST_OR_STRING_OR_SYMBOL, {
		{ PPL_ENGLISH,	"TOS should be an array,list,string or a symbol."
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が配列、リスト、文字列またはシンボルではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_LIST, {
		{ PPL_ENGLISH,	"TOS should be a list."
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS がリストではありません。"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_FILE, {
		{ PPL_ENGLISH,	"TOS should be a file.\n"
                		"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が file ではありません。\n"
               			"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_BOOL_OR_WP, {
		{ PPL_ENGLISH,	"TOS should be a bool or a word pointer.\n"
                		"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が bool もくくはワードポインタではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_NUMBER_OR_STRING, {
		{ PPL_ENGLISH,	"TOS should be a number or a string.\n"
                		"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS が数値または文字列ではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_WP, {
		{ PPL_ENGLISH,	"TOS should be a word pointer.\n"
                		"current TOS type is %s." },
		{ PPL_JAPANESE, "TOS がワードポインタではありません。\n"
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_ADDRESS, {
		{ PPL_ENGLISH,	"TOS should be an address." 
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS がアドレスではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_PARAMDEST, {
		{ PPL_ENGLISH,	"TOS should be a param-dest." 
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が param-dest ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_NEW_WORD, {
		{ PPL_ENGLISH,	"TOS should be a new-word." 
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が new-word ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},
	{ E_TOS_WP_OR_NW, {
		{ PPL_ENGLISH,	"TOS should be a word pointer or a new-word." 
                		"current TOS type is %s." },
		{ PPL_JAPANESE,	"TOS が ワードもしくは new-word ではありません。" 
                		"現在の TOS は %s です。" },
	  }
	},

	{ E_SECOND_INT, {
		{ PPL_ENGLISH,	"The Second should be an integer."
                		"Current the Second type is %s." },
		{ PPL_JAPANESE,	"セカンドが整数ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_INT_OR_LONG_OR_BIGINT, {
		{ PPL_ENGLISH,	"The Second should be an integer or a long or a big-int.\n"
                		"Current Second type is %s." },
		{ PPL_JAPANESE,	"セカンドが int または long または big-int ではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_BOOL, {
		{ PPL_ENGLISH,	"The Second should be a bool."
                		"Current Second type is %s." },
		{ PPL_JAPANESE,	"セカンドが bool ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_STRING, {
		{ PPL_ENGLISH,	"The Second should be a string."
                		"Current Second type is %s." },
		{ PPL_JAPANESE,	"セカンドが string ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_ARRAY, {
		{ PPL_ENGLISH,	"The Second should be an array."
                		"Current Second type is %s." },
		{ PPL_JAPANESE, "セカンドが配列ではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_LIST, {
		{ PPL_ENGLISH,	"The Second should be a list."
                		"Current Second type is %s." },
		{ PPL_JAPANESE,	"セカンドがリストではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_ARRAY_OR_LIST, {
		{ PPL_ENGLISH,	"The second should be an array or a list."
                		"Current Second type is %s." },
		{ PPL_JAPANESE, "スタックの 2 番目の要素が配列かリストではありません。"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_FILE, {
		{ PPL_ENGLISH,	"The second should be an file."
						"current the Second type is %s.\n" },
		{ PPL_JAPANESE,	"スタックの 2 番目の要素が配列かリストではありません。"
						"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_BOOL_OR_WP, {
		{ PPL_ENGLISH,	"The Second should be a bool or a word pointer.\n"
                		"current the Second type is %s.\n" },
		{ PPL_JAPANESE, "セカンドが bool もくくはワードポインタではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_THRESHOLD, {
		{ PPL_ENGLISH,	"The Second should be a threshold.\n"
                		"current the Second type is %s.\n" },
		{ PPL_JAPANESE, "セカンドが threshold ではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},
	{ E_SECOND_WP, {
		{ PPL_ENGLISH,	"The Second should be a word.\n"
                		"current the Second type is %s.\n" },
		{ PPL_JAPANESE, "セカンドがワードではありません。\n"
                		"現在のセカンドは %s です。" },
	  }
	},

	{ E_THIRD_BOOL, {
		{ PPL_ENGLISH,	"The third should be an a bool."
						"Current Third type is %s." },
		{ PPL_JAPANESE,	"スタックの 3 番目の要素が bool ではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ E_THIRD_LIST, {
		{ PPL_ENGLISH,	"The third should be an a list."
						"Current Third type is %s." },
		{ PPL_JAPANESE,	"スタックの 3 番目の要素がリストではありません。"
						"現在の 3 番目の要素は %s です。" },
	  }
	},
	{ E_THIRD_ARRAY_OR_LIST, {
		{ PPL_ENGLISH,	"The third should be an array or a list."
						"Current Third type is %s." },
		{ PPL_JAPANESE,	"スタックの 3 番目の要素が配列かリストではありません。"
						"現在のセカンドは %s です。" },
	  }
	},
	{ E_RS_TOS_INT_OR_LONG_OR_BIGINT, {
		{ PPL_ENGLISH,	"The TOS on RS should be an int or a long or a big-int." },
		{ PPL_JAPANESE,	"RS の TOS は int か long もしくは big-int で"
						"なければなりません。" }
	  }
	},
	{ E_RS_TOS_WP, {
		{ PPL_ENGLISH,	"RS's TOS should be a word pointer."
						"current TOS at RS type is %s." },
		{ PPL_JAPANESE,	"RS の TOS がワードではありません。"
						"現在のリターンスタックの TOS は %s です。" },
	  }
	},
	{ E_RS_TOS_ADDRESS, {
		{ PPL_ENGLISH,	"The RS's TOS should be an address."
						"current the TOS at RS type is %s." },
		{ PPL_JAPANESE,	"リターンスタックの TOS がアドレス値ではありません。"
						"現在のリターンスタックの TOS は %s です。" },
	  }
	},
	{ E_RS_TOS_THRESHOLD, {
		{ PPL_ENGLISH,	"The RS's TOS should be a threshold."
						"current the TOS at RS type is %s." },
		{ PPL_JAPANESE,	"リターンスタックの TOS が閾値ではありません。"
						"現在のリターンスタックの TOS は %s です。" },
	  }
	},
	{ E_RS_SECOND_INT_OR_LONG_OR_BIGINT, {
		{ PPL_ENGLISH,	"the RS's second should be an int or a long or a big-int."
						"current the second at RS type is %s." },
		{ PPL_JAPANESE,	"RS のセカンドが int または long もしくは big-int では"
						"ありません。"
						"現在のリターンスタックのセカンドは %s です。" },
	  }
	},
	{ E_RS_SECOND_WP, {
		{ PPL_ENGLISH,	"the second should be a word pointer."
						"current the second at RS type is %s." },
		{ PPL_JAPANESE,	"RS のセカンドがワードではありません。"
						"現在のリターンスタックのセカンドは %s です。" },
	  }
	},
	{ E_SS_MISCINT, {
		{ PPL_ENGLISH,	"The SS's TOS should be MiscInt."
						"current type is %s." },
		{ PPL_JAPANESE,	"SS の TOS は MiscInt でなければなりません。"
						"現在の型は %s です。" },
	  }
	},
	{ E_TOS_CAN_NOT_CONVERT_TO_DOUBLE, {
		{ PPL_ENGLISH,	"TOS can not convert to double."
						"current the TOS type is %s." },
		{ PPL_JAPANESE,	"TOS を double に変換できません。"
                		"現在の TOS の型は %s です。" },
	  }
	},
	{ E_TOS_CAN_NOT_CONVERT_TO_FLOAT, {
		{ PPL_ENGLISH,	"TOS can not convert to float."
						"current the TOS type is %s." },
		{ PPL_JAPANESE,	"TOS を float に変換できません。"
                		"現在の TOS の型は %s です。" },
	  }
	},

	{ E_FIRST_ELEMENT_STRING, {
		{ PPL_ENGLISH,	"the first element should be a string."
						"current type is %s." },
		{ PPL_JAPANESE,	"最初の要素は string でなければなりません。"
                		"現在の型は %s です。" },
	  }
	},
};

static InvalidTypeTosSecondErrorMessageDict gInvalidTypeTosSecondErrorMessageDict {
	{ E_INVALID_DATA_TYPE_TOS_SECOND, {
		{ PPL_ENGLISH,	"invalid data type.\n"
						"TOS is %s, Second is %s." },
		{ PPL_JAPANESE,	"データ型が異常です。\n"
						"TOS は %s で、セカンドは %s です。" },
	  }
	},
	{ E_OUT_OF_SUPPORT_TOS_SECOND, {
		{ PPL_ENGLISH,	"out of support (sorry)..\n"
						"TOS is %s, The second is %s." },
		{ PPL_JAPANESE,	"サポート外の組み合わせです。\n"
						"TOS は %s で、セカンドは %s です。" },
	  }
	},
};

static InvalidTypeStrTvTvErrorMessageDict gInvalidTypeStrTvTvErrorMessageDict {
	{ E_INVALID_DATA_TYPE_FOR_OP, {
		{ PPL_ENGLISH,	"invalid data type for '%s'.\n"
						"1st argument type is %s,"
						"2nd argument type is %s." },
		{ PPL_JAPANESE,	"演算 '%s' が実行できません。\n"
						"第 1 引数の型は %s で、"
						"第 2 引数の型は %s です。" },
	  }
	},
};

static ErrorWithStringMessageDict gErrorWithStringMessageDict {
	{ E_INVALID_TOKEN, {
		{ PPL_ENGLISH,	"Invalid token: '%s'." },
		{ PPL_JAPANESE,	"無効なトークン '%s' です。" },
	  }
	},
	{ E_CAN_NOT_FIND_THE_WORD, {
		{ PPL_ENGLISH,	"Can not find the word '%s'." },
		{ PPL_JAPANESE,	"ワード '%s' が見つかりません。" },
	  }
	},
	{ E_CAN_NOT_FIND_THE_LOCAL_VAR, {
		{ PPL_ENGLISH,	"Can not find the local variable '%s'." },
		{ PPL_JAPANESE,	"ローカル変数 '%s' が見つかりません。" },
	  }
	},
	{ E_CAN_NOT_INSTALL_UNOVERWRITABLE, {
		{ PPL_ENGLISH,	"Can not install the word '%s',"
						"this word is already installed and it is an unoverwritable." },
		{ PPL_JAPANESE,	"ワード '%s' は上書きできないワードとして"
						"既にインストールされています。" },
	  }
	},
	{ E_CAN_NOT_MAKE_SHORTEN_NAME_UNOVERWRITABLE, {
		{ PPL_ENGLISH,	"Can not make a shorten word name '%s',"
						"this name is already used and it is an unoverwritable."
	    				"If you does not want to define shortend word,"
						"please use '::' instead of ':'." 	},
		{ PPL_JAPANESE,	"短縮名 '%s' は上書きできないワードとして"
						"既に使用されています。" 
						"短縮形を使用しないのであれば、: ではなく :: を"
						"使用するようにして下さい。" },
	  }
	},

	{ E_AMBIGOUS_WORD_NAME, {
		{ PPL_ENGLISH,	"the word name '%s' is anbigous."
						"The follow packages have this name's word: " },
		{ PPL_JAPANESE,	"ワード名 '%s' は曖昧です。"
						"以下のパッケージでこの名前は使用されています：" },
	  }
	},

	{ E_ALREADY_DEFINED, {
		{ PPL_ENGLISH,	"'%s' is already defined." },
		{ PPL_JAPANESE,	"'%s' は既に定義されています。" },
	  }
	},

	{ E_ALREADY_REGISTERED_LOCAL_VAR, {
		{ PPL_ENGLISH,	"the local variable of '%s' is already registered." },
		{ PPL_JAPANESE,	"ローカル変数 '%s' は既に登録されています。" },
	  }
	},

	{ E_NO_SUCH_LOCAL_VAR, {
		{ PPL_ENGLISH,	"no such local variable '%s'." },
		{ PPL_JAPANESE,	"ローカル変数 '%s' は存在しません。" },
	  }
	},

	{ E_CAN_NOT_OPEN_FILE, {
		{ PPL_ENGLISH,	"Can not open the file '%s'." },
		{ PPL_JAPANESE,	"ファイル '%s' が開けません。" },
	  }
	},
	{ E_CAN_NOT_CREATE_FILE, {
		{ PPL_ENGLISH,	"Can not create the file '%s'." },
		{ PPL_JAPANESE,	"ファイル '%s' を作成できません。" },
	  }
	},
	{ E_CAN_NOT_CLOSE_THE_FILE, {
		{ PPL_ENGLISH,	"Can not close the file '%s'." },
		{ PPL_JAPANESE,	"ファイル '%s' をクローズできません。" },
	  }
	},
	{ E_CAN_NOT_READ_FILE, {
		{ PPL_ENGLISH,	"Can not read the file '%s'." },
		{ PPL_JAPANESE,	"ファイル '%s' を読み込めません。" },
	  }
	},
	{ E_CAN_NOT_FIND_THE_ENTRY_POINT, {
		{ PPL_ENGLISH,	"Can not read the entry point in '%s'." },
		{ PPL_JAPANESE,	"ファイル '%s' のエントリポイントが見つかりません。" },
	  }
	},
};

static ErrorWithIntMessageDict gErrorWithIntMessageDict {
	{ E_TOS_POSITIVE_INT, {
		{ PPL_ENGLISH,	"TOS should be a positive integer."
						"Current value is %d." },
		{ PPL_JAPANESE,	"TOS が正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ E_SECOND_POSITIVE_INT, {
		{ PPL_ENGLISH,	"The second should be a positive integer."
						"Current value is %d." },
		{ PPL_JAPANESE,	"セカンドが正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ E_TOS_NON_NEGATIVE, {
		{ PPL_ENGLISH,	"TOS should be a zero or positive integer."
						"Current value is %d." },
		{ PPL_JAPANESE,	"TOS がゼロまたは正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},
	{ E_SECOND_NON_NEGATIVE, {
		{ PPL_ENGLISH,	"The second should be a zero or positive integer."
						"current value is %d." },
		{ PPL_JAPANESE,	"セカンドがゼロまたは正の整数ではありません。"
						"現在の値は %d です。" },
	  }
	},

	{ E_INVALID_LOCAL_SLOT_INDEX, {
		{ PPL_ENGLISH,	"Invalid slot index for local variables: %d." },
		{ PPL_JAPANESE,	"無効なローカル変数用スロットインデックスです:%d" },
	  }
	},
};

static ErrorWith2IntMessageDict gErrorWith2IntMessageDict {
	{ E_DS_NOT_ENOUGH_DATA, {
		{ PPL_ENGLISH,	"DS should have %d elements."
						"There are only %d elements." },
		{ PPL_JAPANESE,	"DS には少なくとも %d 個の要素が存在する必要があります。"
						"現在は %d 個の要素しか DS に存在しません。" },
	  }
	},

	{ E_DEPTH_INDEX_OUT_OF_RANGE, {
		{ PPL_ENGLISH,	"stack index is out of range.\n"
               			"the depth range is 0 to %d, but index is %d." },
		{ PPL_JAPANESE,	"インデックスの範囲が異常です。\n"
						"スタックのインデックス範囲は 1 から %d までですが、"
						"指定されたインデックス値は %d です。" },

	  }
	},
	{ E_ARRAY_INDEX_OUT_OF_RANGE, {
		{ PPL_ENGLISH,	"index is out of range.\n"
                		"the array range is 0 to %d, but index is %d.\n" },
		{ PPL_JAPANESE,	"インデックスの範囲が異常です。"
                		"配列のインデックスの範囲は 0 から %d までですが、"
						"指定されたインデックス値は %d です。" },
	  }
	},
	{ E_LIST_INDEX_OUT_OF_RANGE, {
		{ PPL_ENGLISH,	"index is out of range.\n"
                		"the list range is 0 to %d, but index is %d." },
		{ PPL_JAPANESE,	"インデックスの範囲が異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定されたインデックス値は %d です。" },
	  }
	},
	{ E_LIST_START_INDEX_OUT_OF_RANGE, {
		{ PPL_ENGLISH, "the start index is out of range.\n"
                		"the list range is 0 to %d, but the index is %d." },
		{ PPL_JAPANESE,	"開始位置として与えられたインデックスの範囲が異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定された開始位置は %d です。" },
	  }
	},
	{ E_LIST_LENGTH_INDEX_OUT_OF_RANGE, {
		{ PPL_ENGLISH,	"the start+length index is out of range.\n"
                		"the list range is 0 to %d, but the given index is %d." },
		{ PPL_JAPANESE,	"開始位置+長さとして与えられたインデックスの範囲が"
						"異常です。\n"
                		"リストのインデックスの範囲は 0 から %d までですが、"
						"指定された位置は %d です。" },
	  }
	},
};

static PP_Lang gLang=PPL_JAPANESE;

static Mutex gErrorMessageMutex;

static void beginErrorMessage();
static void endErrorMessage();
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
		fprintf(stderr,"SYSTEM ERROR at Error func 1-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gNoParamErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 1-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: %s\n",errorStr,wordName,message);
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,
		   const InvalidTypeErrorID inErrorID,const std::string& inStr) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gInvalidTypeErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 2-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 2-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr.c_str());
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,
		   const ErrorID_withString inErrorID,const char *inStr) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gErrorWithStringMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 3-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWithStringMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 3-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr);
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,const InvalidTypeTosSecondErrorID inErrorID,
		   const TypedValue& inTos,const TypedValue& inSecond) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();

	if(gInvalidTypeTosSecondErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 4-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeTosSecondErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 4-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inTos.GetTypeStr().c_str(),
							   inSecond.GetTypeStr().c_str());
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,const InvalidTypeStrTvTvErrorID inErrorID,
		   const std::string& inStr,const TypedValue inTV1,const TypedValue& inTV2) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	
	if(gInvalidTypeStrTvTvErrorMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 5-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gInvalidTypeStrTvTvErrorMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 5-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inStr.c_str(),
							   inTV1.GetTypeStr().c_str(),
							   inTV2.GetTypeStr().c_str());
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,const ErrorID_withInt inErrorID,
		   const int inIntValue) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	if(gErrorWithIntMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 6-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWithIntMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 6-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inIntValue);
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

bool Error(Context& inContext,const ErrorID_with2int inErrorID,
		   const int inIntValue1,const int inIntValue2) {
	const char *wordName=inContext.GetExecutingWordName();
	const char *errorStr=getErrStr();
	if(gErrorWith2IntMessageDict.count(inErrorID)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 7-1 (inErrorID=%d).\n",inErrorID);
		exit(-1);
	}
	auto messageDict=gErrorWith2IntMessageDict[inErrorID];
	if(messageDict.count(gLang)!=1) {
		fprintf(stderr,"SYSTEM ERROR at Error func 7-2 (inErrorID=%d).\n",inErrorID);
		fprintf(stderr,"gLang=%d\n",gLang);
		exit(-1);
	}

	beginErrorMessage();
		printCalleeInfo(inContext);
		const char *message=messageDict[gLang].c_str();
		fprintf(stderr,"%s %s: ",errorStr,wordName);
		fprintf(stderr,message,inIntValue1,inIntValue2);
		fprintf(stderr,"\n");
	endErrorMessage();
	return false;
}

static void beginErrorMessage() {
	Lock(gErrorMessageMutex);
}
static void endErrorMessage() {
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
		case PPL_ENGLISH:	ret="ERROR";	break;
		case PPL_JAPANESE:	ret="エラー";	break;
	}
	return ret;
}

