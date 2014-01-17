
#include "keywords.h"

const wchar_t*
keywords_get_value( int someKey )
{
    if( someKey >= 0 && someKey < LQ_KEYWORD_COUNT )
    {
        return LqKeywordValues[someKey];
    }

    return NULL;
}

int
keywords_get_key( const wchar_t* someValue )
{
    size_t i, l;
    int r = -1;    

    for( i = 0; i < LQ_KEYWORD_COUNT; ++i )
    {
        l = wcslen( LqKeywordValues[i] );
        
        if( wcscmp( LqKeywordValues[i], someValue ) == 0 )
        {
            r = i;
            break;
        }
    }

    return r;
}
