/**********************************************\
*
*  AGE by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "AGEheader.h"

#include <cstring>

namespace AGE
{

const char RawId[] = { 'A', 'G', 'E', 1 };

void filEmpty(Header& header)
{
    memcpy(&header.id, RawId, sizeof(header.id));
}

bool isRawHeader(const Header& header)
{
    return memcmp(&header.id, RawId, sizeof(header.id)) == 0;
}

const char* FormatToStr(Format format)
{
    static const char* Names[] =
    {
        "alpha",
        "rgb",
        "rgba",
    };

    return Names[(unsigned)format];
}

const char* CompressionToStr(Compression compression)
{
    static const char* Names[] =
    {
        "none",
        "rle",
        "rle4",
        "zlib",
    };

    return Names[(unsigned)compression];
}

}
