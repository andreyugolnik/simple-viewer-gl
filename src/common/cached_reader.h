/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "common/buffer.h"

class cFile;

class cCachedReader final
{
public:
    cCachedReader(cFile& file, uint32_t bufferSize);

    bool read(uint8_t* buffer, uint32_t count);

private:
    cFile& m_file;
    Buffer m_buffer;
    uint32_t m_offset = 0;
    uint32_t m_remain = 0;
};
