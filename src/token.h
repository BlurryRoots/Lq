
#include <string-library.h>

#include <wchar.h>
#include <stddef.h>

#ifndef _LQ_TOKEN_
#define _LQ_TOKEN_

/**/
enum token_type
{
    SYMBOL,
    STRING,
    NUMBER,
    LIST_BEGIN,
    LIST_END,
    QUOTE
};
typedef
    enum token_type
    token_type;

/**/
struct token_t
{
    string_t* symbol;

    token_type type;

    size_t line;
    size_t column;
};
typedef
    struct token_t
    token_t;

/**/
struct token_list_t
{
    token_t* tokens;

    size_t size;
};
typedef
    struct token_list_t
    token_list_t;

/**/
void
token_free( token_t* someToken );

/*
    Initializes a token list.
    If token list has previously been used, its tokens are deleted.
*/
int
token_list_init( token_list_t* someList );

/**/
void
token_list_free( token_list_t* someList );

/*
    Creates and appends a new token to given token list.
*/
int
token_list_append( 
    token_list_t* someList,
    const string_t* someSymbol,
    token_type someType,
    size_t someLine,
    size_t someColumn );

void
token_list_print( const token_list_t* someList );

#endif
