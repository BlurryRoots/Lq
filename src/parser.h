
#include "lexer.h"

#include <string-library.h>

#include <stdlib.h>

#ifndef _LQ_PARSER_
#define _LQ_PARSER_

enum expression_type
{
    EXP_LIST,
    EXP_KEYWORD,
    EXP_QUOTED,
    EXP_VARIABLE,
    EXP_STRING,
    EXP_NUMBER
};
typedef
    enum expression_type
    expression_type;

struct parse_tree_element_t
{
    expression_type type;

    union { string_t* str;
            double num; } value;

    struct parse_tree_element_t* parent;
    struct parse_tree_element_t** parameters;
    size_t parameterCount;
};
typedef
    struct parse_tree_element_t
    parse_tree_element_t;

parse_tree_element_t*
parse_tree_element_new( expression_type someType );

void
parse_tree_element_free( parse_tree_element_t* someElement );

int
parse_tree_element_add_parameter( parse_tree_element_t* someElement,
                                  parse_tree_element_t* someParameter );

parse_tree_element_t*
parser_parse( lexer_t someLexer );

#endif
