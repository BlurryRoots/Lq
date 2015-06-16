
#include <wchar.h>

#ifndef _LQ_KEYWORDS_
#define _LQ_KEYWORDS_

#define LQ_QUOTE        0

#define LQ_BOOL_TRUE    1
#define LQ_BOOL_FALSE   2

#define LQ_LIST_CREATE  3
#define LQ_LIST_LENGTH  4
#define LQ_LIST_HEAD    5
#define LQ_LIST_TAIL    6
#define LQ_LIST_APPEND  7
#define LQ_LIST_AT      8

#define LQ_FUNC         9

#define LQ_CLASS        10

//
#define LQ_KEYWORD_COUNT 11

static int LqKeywordKeys [] =
{
    LQ_QUOTE,

    LQ_BOOL_TRUE,
    LQ_BOOL_FALSE,

    LQ_LIST_CREATE,
    LQ_LIST_LENGTH,
    LQ_LIST_HEAD,
    LQ_LIST_TAIL,
    LQ_LIST_APPEND,
    LQ_LIST_AT,

    LQ_FUNC,

    LQ_CLASS
};

static const wchar_t* LqKeywordValues [] = 
{
    L"'",

    L"true",
    L"false",

    L"#",
    L"#:length",
    L"#:head",
    L"#:tail",
    L"#:append",
    L"#:at",

    L"func",

    L"class"
};

const wchar_t*
keywords_get_value( int someKey );

int
keywords_get_key( const wchar_t* someVaule );

#endif
