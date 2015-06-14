/**/
#include <string-library.h>
#include <lexer.h>

#include <stdio.h>
#include <locale.h>

int
main (void) {
    setlocale (LC_ALL, "");

    string_t *script;
    lexer_t lexer;

    script = string_new ();
    string_copy_cstr (script,
        L"(func Soßen-köche (Ä b)\n"
            L"\t(+ Ä b))\n"
        L"(Soßen-köche 23 42)\n"
    );
    wprintf (L"%ls\n", script->data);

    return 0;
}
