/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "file.h"

#include <vector>

struct z_stream_s;

class cFileZlib : public cFileInterface
{
public:
    cFileZlib(cFile* file);
    virtual ~cFileZlib();

    virtual int seek(long offset, int whence);
    virtual uint32_t read(void* ptr, uint32_t size);
    virtual long getSize() const;

protected:
    cFile* m_file;
    z_stream_s* m_zipStream;
    std::vector<uint8_t> m_buffer;
};
