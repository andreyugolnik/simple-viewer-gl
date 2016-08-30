/**********************************************\
*
*  AGE by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

namespace RAW
{

enum class Format : unsigned
{
    ALPHA,
    RGB,
    RGBA,
};

enum class Compression : unsigned
{
    NONE,
    RLE,
    RLE4,
};

struct Header
{
    unsigned id;
    Format format;
    Compression compression;
    unsigned w;
    unsigned h;
    unsigned data_size;
};

void filEmpty(Header& header);
bool isRawHeader(const Header& header);

}
