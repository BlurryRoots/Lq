/**/
#include <string-library.h>
#include <lexer.h>
#include <parser.h>

#include <stdio.h>
#include <locale.h>

int
main (void) {
    setlocale (LC_ALL, "");

    string_t *script = string_new ();
    string_copy_cstr (script,
        L"(func Soßen-köche (Ä b)\n"
            L"\t(+ 'Ä b))\n"
        L"(Soßen-köche 23 42)\n"
    );
    fwprintf (stdout, L"First here\n");
    fwprintf (stdout, L"%ls", script->data);

    fwprintf (stdout, L"Here\n");
    lexer_t lexer = lexer_create ();
    if (! lexer_scan (&lexer, script)) {
        fwprintf (stderr, L"Error scanning source!\n");
        goto LABEL_exit;
    }

    fwprintf (stdout, L"WTF?\n");
    parse_tree_element_t* result = parser_parse (lexer);
    parse_tree_element_print (result);
    fwprintf (stdout, L"\n");
    parse_tree_element_free (result);

LABEL_exit:
    fwprintf (stdout, L"EXIT?!");
    lexer_free (&lexer);
    string_free (script);

    return 0;
}
