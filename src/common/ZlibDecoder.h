/**********************************************\
*
*  AGE by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class cZlibDecoder final
{
public:
    cZlibDecoder();
    ~cZlibDecoder();

    unsigned decode(const unsigned char* packed, unsigned packedSize, unsigned char* const out, unsigned outSize);
};
