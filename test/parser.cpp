//
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>

extern "C"
{
    #include "../src/lexer.h"
    #include "../src/parser.h"

    #include <string-library.h>

    #include <stdio.h>
    #include <wchar.h>
}

TEST_CASE( "parser utility functions", "[parser]" )
{
    SECTION( "parse_tree_element_new & parse_tree_element_free" )
    {
        parse_tree_element_t* element =
            parse_tree_element_new( EXP_LIST );

        REQUIRE( element != NULL );
        REQUIRE( element->parent == NULL );
        REQUIRE( element->parameters == NULL );
        CHECK( element->parameterCount == 0 );

        parse_tree_element_free( element );
    }

    SECTION( "parse_tree_element_add_parameter" )
    {
        parse_tree_element_t* element = NULL;
        parse_tree_element_t* buffer = NULL;

        element = parse_tree_element_new( EXP_LIST );
        REQUIRE( element != NULL );

        buffer = parse_tree_element_new( EXP_NUMBER );
        REQUIRE( buffer != NULL );
        REQUIRE(
            parse_tree_element_add_parameter(
                    element,
                    buffer
                 ) == 1
        );
        REQUIRE( element != NULL );
        REQUIRE( element->parameters != NULL );
        REQUIRE( element->parameterCount == 1 );
        REQUIRE( buffer->parent == element );
        REQUIRE( element->parameters[0] == buffer );

        buffer = parse_tree_element_new( EXP_STRING );
        REQUIRE( buffer != NULL );
        REQUIRE(
            parse_tree_element_add_parameter(
                    element,
                    buffer
                 ) == 1
        );
        REQUIRE( element != NULL );
        REQUIRE( element->parameters != NULL );
        REQUIRE( element->parameterCount == 2 );
        REQUIRE( buffer->parent == element );
        REQUIRE( element->parameters[1] == buffer );

        buffer = parse_tree_element_new( EXP_VARIABLE );
        REQUIRE( buffer != NULL );
        REQUIRE(
            parse_tree_element_add_parameter(
                    element,
                    buffer
                 ) == 1
        );
        REQUIRE( element != NULL );
        REQUIRE( element->parameters != NULL );
        REQUIRE( element->parameterCount == 3 );
        REQUIRE( buffer->parent == element );
        REQUIRE( element->parameters[2] == buffer );

        parse_tree_element_free( element );
    }
}

TEST_CASE( "parser functions", "[parser]" )
{
    SECTION( "parse" )
    {
        const wchar_t* scriptStr =
            L"(func Soßen-köche (Ä b)\n"
             L"\t'(+ Ä b))\n"
            L"(Soßen-köche 'a 23 42)\n";
        const token_type tt[] =
        {
            LIST_BEGIN,
                SYMBOL,
                SYMBOL,
                LIST_BEGIN,
                    SYMBOL,
                    SYMBOL,
                LIST_END,
                QUOTE,
                LIST_BEGIN,
                    SYMBOL,
                    SYMBOL,
                    SYMBOL,
                LIST_END,
            LIST_END,

            LIST_BEGIN,
                SYMBOL,
                QUOTE, SYMBOL,
                NUMBER,
                NUMBER,
            LIST_END
        };
        size_t i;
        string_t* script;
        lexer_t lexer;
        parse_tree_element_t* result;
        parse_tree_element_t* quoted;

        script = string_new();
        string_copy_cstr( script, scriptStr );
        lexer = lexer_create();
        result = NULL;
        quoted = NULL;

        REQUIRE( lexer_scan( & lexer, script ) == 1 );
        for( i = 0; i < lexer.tokenlist.size; ++i )
        {
            if( lexer.tokenlist.tokens[i].type != tt[i] )
            {
                printf( "i: %i\n", i );
                REQUIRE( lexer.tokenlist.tokens[i].type == tt[i] );
            }
        }
        string_free( script );

        result = parser_parse( lexer );
        REQUIRE( result != NULL );

        REQUIRE( result->parameterCount == 2 ); // script list
        REQUIRE( result->parameters[1]->parameterCount == 4 ); // list items
        quoted = result->parameters[1]->parameters[1]->parameters[0];
        REQUIRE( wcscmp( quoted->value.str->data, L"a" ) == 0 );

        parse_tree_element_free( result );
        lexer_free( & lexer );
    }
}
