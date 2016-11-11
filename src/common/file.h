/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>

class cFileInterface
{
public:
    virtual ~cFileInterface() { }

    virtual int seek(long offset, int whence) = 0;
    virtual uint32_t read(void* ptr, uint32_t size) = 0;
    virtual long getSize() const = 0;
};



class cFile : public cFileInterface
{
public:
    cFile();
    virtual ~cFile();

    bool open(const char* path, const char* mode = "rb");
    void close();

    long getOffset() const;
    void* getHandle() const { return m_file; }

    virtual int seek(long offset, int whence);
    virtual uint32_t read(void* ptr, uint32_t size);
    virtual long getSize() const { return m_size; }

protected:
    void* m_file;
    long m_size;
};
