/**********************************************\
*
*  AGE by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ZlibDecoder.h"

#include <zlib.h>
#include <cstring>

cZlibDecoder::cZlibDecoder()
{
}

cZlibDecoder::~cZlibDecoder()
{
}

unsigned cZlibDecoder::decode(const unsigned char* packed, unsigned packedSize, unsigned char* const out, unsigned outSize)
{
    z_stream_s zip;
    z_streamp st = (z_streamp)&zip;
    ::memset(st, 0, sizeof(z_stream_s));

    st->data_type = Z_BINARY;
    inflateInit(st);

    st->next_out  = (Bytef*)out;
    st->avail_out = outSize;

    st->next_in  = (Byte*)packed;
    st->avail_in = packedSize;

    inflate(st, Z_FULL_FLUSH);

    inflateEnd(&zip);

    return outSize;
}
