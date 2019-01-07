#pragma once

enum PP_Lang {
	PPL_ENGLISH,
	PPL_JAPANESE,
};

enum NoParamErrorID {
	E_SYSTEM_ERROR,
	E_SORRY_NOT_SUPPORT_THIS_OPERATION,
	E_OPEN_DOUBLE_QUOTE,
	E_OPEN_SINGLE_QUOTE,

	E_THREAD_MAY_BE_BROKEN,

	E_DS_IS_EMPTY,
	E_DS_AT_LEAST_2,
	E_DS_AT_LEAST_3,

	E_RS_IS_EMPTY,
	E_RS_AT_LEAST_2,

	E_RS_BROKEN,
	E_RS_BROKEN_TOS_SHOULD_BE_INT,
	E_RS_BROKEN_SECOND_SHOULD_BE_INT,
	E_RS_NOT_ENOUGH,

	E_IS_BROKEN,

	E_NO_LAST_DEFINED_WORD,

	E_C_STYLE_COMMENT_MISMATCH,

	E_CAN_NOT_CONVERT_TO_INT,
	E_CAN_NOT_CONVERT_TO_INT_DUE_TO_OVERFLOW,
	E_CAN_NOT_CONVERT_TO_LONG_DUE_TO_OVERFLOW,
	E_CAN_NOT_CONVERT_TO_DOUBLE_DUE_TO_OVERFLOW,
	E_CAN_NOT_CONVERT_TO_FLOAT_DUE_TO_OVERFLOW,

	E_CHAN_IS_ALREADY_CLOSED,
	E_NO_CHAN_FROM_PARENT,
	E_NO_CHAN_TO_PARENT,
	E_NO_CHAN_FROM_CHILD,
	E_NO_CHAN_FROM_PIPE,

	E_TOS_LIST_NO_ELEMENT,
	E_TOS_LIST_AT_LEAST_CORES,

	E_FORMAT_DATA_MISMATCH,

	E_FILE_IS_ALREADY_CLOSED,
	E_CAN_NOT_CLOSE_FILE,
	E_CAN_NOT_FLUSH_FILE,

	E_WP_AT_TOS_SHOULD_PUSH_A_BOOL,
	E_WP_AT_SECOND_SHOULD_PUSH_A_BOOL,

	E_SHOULD_BE_COMPILE_MODE,
};

enum InvalidTypeErrorID {
	E_TOS_INT,
	E_TOS_INT_OR_LONG,
	E_TOS_INT_OR_LONG_OR_BIGINT,
	E_TOS_DOUBLABLE,
	E_TOS_NUMBER,
	E_TOS_BOOL,
	E_TOS_STRING,
	E_TOS_ARRAY,
	E_TOS_LIST,
	E_TOS_FILE,
	E_TOS_BOOL_OR_WP,
	E_TOS_NUMBER_OR_STRING,
	E_TOS_WP,
	E_TOS_ADDRESS,
	E_TOS_PARAMDEST,
	E_TOS_NEW_WORD,
	E_TOS_WP_OR_NW,
	
	E_SECOND_INT,
	E_SECOND_INT_OR_LONG_OR_BIGINT,
	E_SECOND_BOOL,
	E_SECOND_STRING,
	E_SECOND_ARRAY,
	E_SECOND_LIST,
	E_SECOND_ARRAY_OR_LIST,
	E_SECOND_FILE,
	E_SECOND_BOOL_OR_WP,
	E_SECOND_THRESHOLD,
	E_SECOND_WP,

	E_THIRD_BOOL,
	E_THIRD_LIST,
	E_THIRD_ARRAY_OR_LIST,

	E_RS_TOS_INT_OR_LONG_OR_BIGINT,
	E_RS_TOS_ADDRESS,
	E_RS_TOS_THRESHOLD,

	E_RS_SECOND_INT_OR_LONG_OR_BIGINT,
	E_RS_SECOND_WP,

	E_TOS_CAN_NOT_CONVERT_TO_DOUBLE,
	E_TOS_CAN_NOT_CONVERT_TO_FLOAT,

	E_FIRST_ELEMENT_STRING,
};

enum InvalidTypeTosSecondErrorID {
	E_INVALID_DATA_TYPE_TOS_SECOND,
	E_OUT_OF_SUPPORT_TOS_SECOND,
};

enum InvalidTypeStrTvTvErrorID {
	E_INVALID_DATA_TYPE_FOR_OP,
};

enum ErrorID_withString {
	E_CAN_NOT_FIND_THE_WORD,
	E_CAN_NOT_INSTALL_UNOVERWRITABLE,
	E_CAN_NOT_MAKE_SHORTEN_NAME_UNOVERWRITABLE,

	E_AMBIGOUS_WORD_NAME,

	E_ALREADY_DEFINED,

	E_CAN_NOT_OPEN_FILE,
	E_CAN_NOT_CREATE_FILE,
	E_CAN_NOT_CLOSE_THE_FILE,
	E_CAN_NOT_READ_FILE,

	E_CAN_NOT_FIND_THE_ENTRY_POINT,
};

enum ErrorID_withInt {
	E_TOS_POSITIVE_INT,
	E_SECOND_POSITIVE_INT,

	E_TOS_NON_NEGATIVE,
	E_SECOND_NON_NEGATIVE,
};

enum ErrorID_with2int {
	E_DEPTH_INDEX_OUT_OF_RANGE,

	E_ARRAY_INDEX_OUT_OF_RANGE,
	E_LIST_INDEX_OUT_OF_RANGE,
	
	E_LIST_START_INDEX_OUT_OF_RANGE,
	E_LIST_LENGTH_INDEX_OUT_OF_RANGE,
};

