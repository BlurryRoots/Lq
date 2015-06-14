
//
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>

extern "C"
{
    #include "../src/token.h"
    #include "../src/lexer.h"

    #include <string-library.h>

    #include <stdio.h>
    #include <wchar.h>
    #include <locale.h>
}

TEST_CASE( "token-list", "[token]" )
{
    setlocale( LC_ALL, "" );

    const wchar_t* symbol = L"my-symböl";
    const wchar_t* number = L"1337";
    const size_t line = 23, column = 42;

    string_t* s = string_new();
    string_copy_cstr( s, symbol );
    wprintf( L"s: -%ls-\n", s->data );

    string_t* n = string_new();
    string_copy_cstr( n, number );

    token_list_t list;

    REQUIRE( token_list_init( & list ) == 1 );
    REQUIRE( list.size == 0 );
    REQUIRE( list.tokens == NULL );

    REQUIRE(
        token_list_append(
            & list,
            s,
            SYMBOL,
            line,
            column )
        == 1
    );

    REQUIRE( list.size == 1 );
    wprintf( L"list.tokens[0].symbol: -%ls-\n",
             list.tokens[0].symbol->data );
    REQUIRE(
        wcsncmp( list.tokens[0].symbol->data,
                 symbol,
                 list.tokens[0].symbol->length ) == 0 );
    REQUIRE( list.tokens[0].line == line );
    REQUIRE( list.tokens[0].column == column );

    REQUIRE(
        token_list_append(
            & list,
            n,
            NUMBER,
            1,
            column + 1 )
        == 1
    );
    REQUIRE( list.size == 2 );

    REQUIRE(
        wcsncmp( list.tokens[0].symbol->data,
                 symbol,
                 list.tokens[0].symbol->length ) == 0 );
    REQUIRE(
        wcsncmp( list.tokens[1].symbol->data,
                 number,
                 list.tokens[1].symbol->length ) == 0 );

    token_list_print( & list );

    token_list_free( & list );

    REQUIRE( list.tokens == NULL );
    REQUIRE( list.size == 0 );

    string_free( n );
    string_free( s );
}

TEST_CASE( "lexer success", "[lexer]" )
{
    setlocale( LC_ALL, "" );

    const wchar_t* str =
        L"(38.1 'ärschle 52)";
    const token_type tt[] =
    {
        LIST_BEGIN, NUMBER, QUOTE, SYMBOL, NUMBER, LIST_END
    };
    string_t* script = string_new();
    lexer_t lexer = lexer_create();

    string_copy_cstr( script, str );

    CHECK( lexer_scan( & lexer, script ) == 1 );

    if( lexer.state == LEXER_ERROR )
    {
       wprintf( L"syntax error: %ls\n", lexer.errorMessage );
    }

    std::cout
        << "lexer.tokenlist.size: "
        << lexer.tokenlist.size << "\n";
    token_list_print( & lexer.tokenlist );

    lexer_free( & lexer );
    string_free( script );
}
