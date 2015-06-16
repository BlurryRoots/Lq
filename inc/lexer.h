
#include "token.h"

#include <string-library.h>

#include <stdlib.h>

#ifndef _LQ_LEXER_
#define _LQ_LEXER_

enum lexer_state
{
    IDLE,

    READ_SYMBOL,
    READ_COMMENT,
    READ_STRING,
    READ_NUMBER,

    LEXER_SUCCESS,
    LEXER_ERROR
};
typedef
    enum lexer_state
    lexer_state;

struct lexer_t
{
    token_list_t tokenlist;

    lexer_state state;

    string_t* errorMessage;
};
typedef
    struct lexer_t
    lexer_t;

const char*
lexer_state_to_string( lexer_state someState );

lexer_t
lexer_create();

int
lexer_reset( lexer_t* someLexer );

int
lexer_scan( lexer_t* someLexer, const string_t* someScript );

void
lexer_free( lexer_t* someLexer );

#endif //_LQ_LEXER_
