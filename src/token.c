
#include <token.h>

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>

/**/
void
token_free( token_t* someToken )
{
    if( ! someToken )
    {
        return;
    }

    string_free( someToken->symbol );
    someToken->symbol = NULL;

    someToken->line = 0;
    someToken->column = 0;
}

/*
    Initializes a token list.
    If token list has previously been used, its tokens are deleted.
*/
int
token_list_init( token_list_t* someList )
{
    if( ! someList )
    {
        return 0;
    }

    token_list_free( someList );

    someList->tokens = NULL;
    someList->size = 0;

    return 1;
}

/**/
void
token_list_free( token_list_t* someList )
{
    size_t i;

    if( ! someList || ! someList->tokens )
    {
        return;
    }

    for( i = 0; i < someList->size; ++i )
    {
        token_free( & (someList->tokens[i]) );
    }
    free( someList->tokens );

    someList->tokens = NULL;
    someList->size = 0;
}

/*
    Creates and appends a new token to given token list.
*/
int
token_list_append(
    token_list_t* someList,
    const string_t* someSymbol,
    token_type someType,
    size_t someLine,
    size_t someColumn )
{
    size_t i;
    token_t* oldOnes;
    token_t t;

    if( ! someList->tokens )
    {
        someList->tokens = calloc(
            1,
            sizeof( token_t )
        );
    }
    else
    {
        oldOnes = someList->tokens;
        someList->tokens = calloc(
            someList->size + 1,
            sizeof( token_t )
        );
        if( ! someList->tokens )
        {
            someList->tokens = oldOnes;
            return 0;
        }

        for( i = 0; i < someList->size; ++i )
        {
            someList->tokens[i].type = oldOnes[i].type;
            someList->tokens[i].line = oldOnes[i].line;
            someList->tokens[i].column = oldOnes[i].column;

            someList->tokens[i].symbol = string_new();
            string_copy( someList->tokens[i].symbol,
                         oldOnes[i].symbol );

            token_free( & oldOnes[i] );
        }
        free( oldOnes );
    }

    someList->tokens[someList->size].symbol = string_new();

    if( someList->tokens[someList->size].symbol == NULL
     || ! string_copy(
            someList->tokens[someList->size].symbol,
            someSymbol ) )
    {
        return 0;
    }

    someList->tokens[someList->size].type = someType;
    someList->tokens[someList->size].line = someLine;
    someList->tokens[someList->size].column = someColumn;

    ++someList->size;

    return 1;
}

void
token_list_print( const token_list_t* someList )
{
    size_t i, j;
    for( i = 0; i < someList->size; ++i )
    {
        wprintf(
            L"i: %i v: %ls\n",
            i,
            someList->tokens[i].symbol->data
        );
    }
}
