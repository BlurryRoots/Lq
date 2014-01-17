
#include "parser.h"

#include "keywords.h"

#include <stdio.h>

enum parser_state
{
    P_START,
    P_PROGRAM,
    P_QUOTED,
    P_BOOL,
    P_LIST
};
typedef
    enum parser_state
    parser_state;

parse_tree_element_t*
parse_tree_element_new( expression_type someType )
{
    parse_tree_element_t* el;

    el = malloc( sizeof( parse_tree_element_t ) );
    if( el == NULL )
    {
        return NULL;
    }

    el->type = someType;

    el->parent = NULL;
    el->parameters = NULL;
    el->parameterCount = 0;

    return el;
}

void
parse_tree_element_free( parse_tree_element_t* someElement )
{
    size_t i;

    if( someElement == NULL )
    {
        return;
    }

    someElement->type = -1;

    if( someElement->parameters != NULL )
    {
        for( i = 0; i < someElement->parameterCount; ++i )
        {
            parse_tree_element_free( someElement->parameters[i] );
        }

        free( someElement->parameters );
        someElement->parameterCount = 0;
    }

    free( someElement );
}

int
parse_tree_element_add_parameter( parse_tree_element_t* someElement,
                                  parse_tree_element_t* someParameter )
{
    parse_tree_element_t** buffer = NULL;

    if( someElement->parameters == NULL )
    {
        someElement->parameterCount = 0;
        buffer = malloc( sizeof( parse_tree_element_t * ) );
        if( buffer == NULL )
        {
            return 0;
        }
    }
    else
    {
        buffer = realloc( 
            someElement->parameters,
            (someElement->parameterCount + 1) * sizeof( parse_tree_element_t * )
        );
        if( buffer == NULL )
        {
            return 0;
        }
    }

    someParameter->parent = someElement;

    someElement->parameters = buffer;
    someElement->parameters[someElement->parameterCount] = someParameter;
    someElement->parameterCount += 1;

    return 1;
}

parse_tree_element_t*
parser_parse( lexer_t someLexer )
{
    int keywordKey;
    int quoted;
    size_t i;
    parser_state state;
    token_t* t;
    parse_tree_element_t* root;    
    parse_tree_element_t* currentList;
    parse_tree_element_t* buffer;

    //<Start> -> <Program> .
    quoted = 0;
    i = 0;
    state = P_START;
    t = NULL;

    root = parse_tree_element_new( EXP_LIST );

    //<Program> -> <empty> .
    if( someLexer.tokenlist.size == 0 )
    {
        goto LABEL_return;
    }
    else
    {        
        state = P_PROGRAM;
    }

    currentList = root;
    buffer = NULL;

    while( 1 )
    {
        t = & someLexer.tokenlist.tokens[i];

        if( state == P_PROGRAM )
        {
LABEL_program:
    //<Program> -> number <Program> .
            if( t->type == NUMBER )
            {
                buffer = parse_tree_element_new( EXP_NUMBER );
                buffer->value.num = wcstod( t->symbol->data, NULL );

                parse_tree_element_add_parameter(
                    currentList,
                    buffer
                );

                goto LABEL_continue;
            }
    //<Program> -> string <Program> .
            if( t->type == STRING )
            {
                buffer = parse_tree_element_new( EXP_STRING );
                buffer->value.num = wcstod( t->symbol->data, NULL );

                parse_tree_element_add_parameter(
                    currentList,
                    buffer
                );

                goto LABEL_continue;                
            }
    //<Program> -> symbol <Program> .
    //<Program> -> <Bool> <Program> .
    //<Bool> -> true .
    //<Bool> -> false .  
            if( t->type == SYMBOL )
            {
LABEL_symbol:
                keywordKey = keywords_get_key( t->symbol->data );
                if( keywordKey == -1 )
                {
                    buffer = parse_tree_element_new( EXP_VARIABLE );
                }
                else
                {
                    buffer = parse_tree_element_new( EXP_KEYWORD );
                }

                buffer->value.str = string_new();
                string_copy( buffer->value.str, t->symbol );
                parse_tree_element_add_parameter(
                    currentList,
                    buffer
                );

                if( quoted > 0 )
                {
                    --quoted;
                    currentList = currentList->parent;
                }

                goto LABEL_continue;                
            }

    //<Program> -> <List> <Program> .
            if( t->type == LIST_BEGIN )
            {
LABEL_list_begin:
                buffer = parse_tree_element_new( EXP_LIST );

                parse_tree_element_add_parameter(
                    currentList,
                    buffer
                );

                currentList = buffer;

                state = P_LIST;

                goto LABEL_continue;            
            }

    //<Program> -> quote <Quoted> .
            if( t->type == QUOTE )
            {
                buffer = parse_tree_element_new( EXP_QUOTED );

                parse_tree_element_add_parameter(
                    currentList,
                    buffer
                );

                currentList = buffer;

                state = P_QUOTED;

                goto LABEL_continue;            
            }

            if( t->type == LIST_END )
            {
                goto LABEL_list_list_end;
            }

            printf( "ERROR: this isnt a program!" );
        }

        if( state == P_LIST )
        {
    //<List> -> ( <Program> ) .
            if( t->type == LIST_END )
            {
LABEL_list_list_end:
                currentList = currentList->parent;

                if( quoted > 0 )
                {
                    --quoted;
                    currentList = currentList->parent;
                }

                state = P_PROGRAM;

                goto LABEL_continue; 
            }

            goto LABEL_program;
        }

        if( state == P_QUOTED )
        {
            ++quoted;
    //<Quoted> -> symbol <Program> .
            if( t->type == SYMBOL )
            {
                state = P_PROGRAM;
                
                goto LABEL_symbol;
            }
    //<Quoted> -> <List> <Program> .
            if( t->type == LIST_BEGIN )
            {
                goto LABEL_list_begin;
            }

            printf( "ERROR: cannot quote this!" );
        }

        // ERROR !
        printf( "ERROR: no options defined! \n\tstate = %i token.type = %i\n", state, t->type );

LABEL_continue:
        ++i;

        if( i == someLexer.tokenlist.size )
        {
            if( state != P_PROGRAM )
            {
                // ERROR !
                printf( "ERROR: wrong end state!\n" );
            }

            break;
        }
    }

LABEL_return:
    return root;
}
