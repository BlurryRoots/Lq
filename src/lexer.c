
#include "lexer.h"

#include <wchar.h>

#define LEXER_ERROR_BUFFER_LENGTH 64

/**/
#define is_blank( c ) \
    (c == L' ')
#define is_tab( c ) \
    (c == L'\t')
#define is_newline( c ) \
    (c == L'\n')
#define is_whitespace( c ) \
    (is_blank( c ) || is_tab( c ) || is_newline( c ))

/**/
#define is_number( c ) \
    (c >= L'0' && c <= L'9')
#define is_decimal_point( c ) \
    (c == L'.')
#define is_symbol( c ) \
    (c >= L'!' && c <= L'~') \
 || (c >= L'¡' && c <= L'ÿ')

/**/
#define is_list_begin( c ) \
    (c == L'(')
#define is_list_end( c ) \
    (c == L')')

/**/
#define is_string_operator( c ) \
    (c == L'"')

/**/
#define is_quotation_operator( c ) \
    (c == L'\'')

/**/
#define is_comment( c ) \
    (c == L';')

const char* IDLE_STRING = "IDLE";
const char* READ_SYMBOL_STRING = "READ_SYMBOL";
const char* READ_STRING_STRING = "READ_STRING";
const char* READ_COMMENT_STRING = "READ_COMMENT";
const char* READ_NUMBER_STRING = "READ_NUMBER";
const char* LEXER_SUCCESS_STRING = "LEXER_SUCCESS";
const char* LEXER_ERROR_STRING = "LEXER_ERROR";
const char* UNKNOWN_STRING = "UNKNOWN";


const char*
lexer_state_to_string( lexer_state someState )
{
    switch( someState )
    {
        case IDLE:
            return IDLE_STRING;
        break;

        case READ_SYMBOL:
            return READ_SYMBOL_STRING;
        break;

        case READ_STRING:
            return READ_STRING_STRING;
        break;

        case READ_COMMENT:
            return READ_COMMENT_STRING;
        break;

        case READ_NUMBER:
            return READ_NUMBER_STRING;
        break;

        case LEXER_SUCCESS:
            return LEXER_SUCCESS_STRING;
        break;

        case LEXER_ERROR:
            return LEXER_ERROR_STRING;
        break;

        default:
            return UNKNOWN_STRING;
        break;
    }
}

lexer_t
lexer_create()
{
    lexer_t lexer;
    lexer.tokenlist.tokens = NULL;

    token_list_init( & lexer.tokenlist );
    lexer.state = IDLE;
    lexer.errorMessage = string_new();

    return lexer;
}

int
lexer_reset( lexer_t* someLexer )
{
    if( ! someLexer )
    {
        return 0;
    }

    string_free( someLexer->errorMessage );    
    someLexer->errorMessage = NULL;

    token_list_free( & someLexer->tokenlist );

    *someLexer = lexer_create();

    return 1;
}

int
lexer_scan( lexer_t* someLexer, const string_t* someScript )
{
    size_t iterator, line, column;
    string_t* buffer;
    token_type typeBuffer;
    wchar_t c;

    if( ! someLexer )
    {
        return 0;
    }

    if( someScript->length == 0 )
    {
        someLexer->state = LEXER_SUCCESS;
    }

    iterator = 0;
    line = column = 0;
    buffer = string_new();

    while( 1 )
    {
        c = someScript->data[iterator];

        ++column;

        if( someLexer->state == IDLE )
        {
            if( is_whitespace( c ) )
            {
                if( is_newline( c ) )
                {
                    column = 0;
                    ++line;
                }

                goto LABEL_continue;
            }

            if( is_comment( c ) )
            {
                someLexer->state = READ_COMMENT;

                goto LABEL_continue;
            }

            if( is_number( c ) )
            {
                someLexer->state = READ_NUMBER;

                string_append( buffer, c );

                goto LABEL_continue;
            }

            if( is_list_begin( c )
             || is_list_end( c ) )
            {
                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = 
                    (is_list_begin( c )
                        ? LIST_BEGIN
                        : LIST_END);

                goto LABEL_append_token;
            }

            if( is_quotation_operator( c ) )
            {
                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = QUOTE;

                goto LABEL_append_token;
            }

            if( is_symbol( c ) )
            {
                someLexer->state = READ_SYMBOL;

                string_append( buffer, c );
            }

            goto LABEL_continue;
        }

        if( someLexer->state == READ_COMMENT )
        {
            if( is_newline( c ) )
            {
                someLexer->state = IDLE;
            }

            goto LABEL_continue;
        }

        if( someLexer->state == READ_STRING )
        {
            if( is_string_operator( c ) )
            {
                someLexer->state = IDLE;

                typeBuffer = STRING;
                goto LABEL_append_token;
            }

            string_append( buffer, c );

            goto LABEL_continue;
        }

        if( someLexer->state == READ_SYMBOL )
        {
            static int finalize = 0;

            if( is_comment( c ) )
            {
                someLexer->state = READ_COMMENT;
                finalize = 1;
            }
            else if( is_string_operator( c ) )
            {
                someLexer->state = READ_STRING;
                finalize = 1;
            }
            else if( is_whitespace( c ) )
            {
                someLexer->state = IDLE;
                finalize = 1;                
            }
            else if( is_list_begin( c )
                  || is_list_end( c ) )
            {
                token_list_append(
                    & someLexer->tokenlist,
                    buffer,
                    SYMBOL,
                    line,
                    column
                );
                string_free( buffer );
                buffer = string_new();

                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = 
                    (is_list_begin( c )
                        ? LIST_BEGIN
                        : LIST_END);

                goto LABEL_append_token;
            }
            else if( is_quotation_operator( c ) )
            {
                token_list_append(
                    & someLexer->tokenlist,
                    buffer,
                    NUMBER,
                    line,
                    column
                );
                string_free( buffer );
                buffer = string_new();

                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = QUOTE;

                goto LABEL_append_token;
            }

            if( finalize )
            {
                finalize = 0;

                typeBuffer = SYMBOL;
                goto LABEL_append_token;
            }

            string_append( buffer, c );

            goto LABEL_continue;
        }

        if( someLexer->state == READ_NUMBER )
        {
            static int finalize = 0;
            static int hadDecimalPoint = 0;

            if( is_whitespace( c ) )
            {
                someLexer->state = IDLE;
                finalize = 1;                
            }
            else if( is_comment( c ) )
            {
                someLexer->state = READ_COMMENT;
                finalize = 1;
            }
            else if( is_string_operator( c ) )
            {
                someLexer->state = READ_STRING;
                finalize = 1;
            }
            else if( is_list_begin( c )
                  || is_list_end( c ) )
            {
                finalize = 0;
                hadDecimalPoint = 0;

                token_list_append(
                    & someLexer->tokenlist,
                    buffer,
                    NUMBER,
                    line,
                    column
                );
                string_free( buffer );
                buffer = string_new();

                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = 
                    (is_list_begin( c )
                        ? LIST_BEGIN
                        : LIST_END);

                goto LABEL_append_token;
            }
            else if( is_quotation_operator( c ) )
            {
                finalize = 0;
                hadDecimalPoint = 0;

                token_list_append(
                    & someLexer->tokenlist,
                    buffer,
                    NUMBER,
                    line,
                    column
                );
                string_free( buffer );
                buffer = string_new();

                someLexer->state = IDLE;

                string_append( buffer, c );

                typeBuffer = QUOTE;

                goto LABEL_append_token;
            }
            else if( is_decimal_point( c ) )
            {
                if( hadDecimalPoint )
                {
                    someLexer->state = LEXER_ERROR;

                    string_copy_cstr(
                        someLexer->errorMessage,
                        L"Error: while reading number -> second decimal point!"
                    );
                }
                else
                {
                    hadDecimalPoint = 1;

                    string_append( buffer, c );
                }

                goto LABEL_continue;
            }
            else if( is_number( c ) )
            {
                string_append( buffer, c );

                goto LABEL_continue;
            }
            else if( is_symbol( c ) )
            {
                someLexer->state = LEXER_ERROR;
                string_copy_cstr(
                    someLexer->errorMessage,
                    L"Error: while reading number -> found symbol!"
                );

                goto LABEL_continue;
            }

            if( finalize )
            {
                finalize = 0;
                hadDecimalPoint = 0;

                someLexer->state = IDLE;

                typeBuffer = NUMBER;

                goto LABEL_append_token;
            }

            goto LABEL_continue;
        }

LABEL_append_token:    
        token_list_append(
            & someLexer->tokenlist,
            buffer,
            typeBuffer,
            line,
            column
        );
        string_free( buffer );
        buffer = string_new();

LABEL_continue:
        if( someLexer->state == LEXER_ERROR )
        {
            break;
        }

        if( c == '\0' || iterator >= someScript->length )
        {
            if( someLexer->state == READ_STRING )
            {
                string_copy_cstr(
                    someLexer->errorMessage,
                    L"Error: while reading string -> unexpected end of file!"
                );

                someLexer->state = LEXER_ERROR;

                break;
            }

            if( buffer->length > 0 )
            {
                if( someLexer->state == READ_SYMBOL )
                {
                    typeBuffer = SYMBOL;

                    goto LABEL_append_token;
                }
                else if( someLexer->state == READ_NUMBER )
                {
                    typeBuffer = NUMBER;

                    goto LABEL_append_token;
                }
            }

            someLexer->state = LEXER_SUCCESS;

            break;
        }
        else
        {
            ++iterator;
        }
    }
    string_free( buffer );


    return someLexer->state == LEXER_SUCCESS;
}

void
lexer_free( lexer_t* someLexer )
{
    if( ! someLexer )
    {
        return;
    }

    string_free( someLexer->errorMessage );    
    token_list_free( & someLexer->tokenlist );    
}

/**/
