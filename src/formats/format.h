/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <dlfcn.h>
#if defined(__linux__)
#   include <GL/glut.h>
#else
#   include <glut.h>
#endif

//#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

class cFile
{
public:
    cFile()
        : m_file(0)
        , m_size(0)
    {
    }

    virtual ~cFile()
    {
        close();
    }

    bool open(const char* path, const char* mode = "rb")
    {
        FILE* file = fopen(path, mode);
        if(file)
        {
            m_file = file;

            (void)fseek(file, 0, SEEK_END);
            m_size = ftell(file);
            (void)fseek(file, 0, SEEK_SET);

            return true;
        }

        printf("Can't open \"%s\".", path);
        return false;
    }

    void close()
    {
        if(m_file)
        {
            fclose(m_file);
            m_file = 0;
        }
    }

    int seek(long offset, int whence)
    {
        return fseek(m_file, offset, whence);
    }

    size_t read(void* ptr, size_t size)
    {
        if(m_file)
        {
            return fread(ptr, 1, size, m_file);
        }

        return 0;
    }

    long getOffset() const
    {
        if(m_file)
        {
            return ftell(m_file);
        }

        return 0;
    }

    long getSize() const { return m_size; }

    FILE* getHandle() const { return m_file; }

private:
    FILE* m_file;
    long m_size;
};



typedef void (*Callback)(int);

class CFormat
{
    friend class CImageLoader;

public:
    CFormat(Callback callback, const char* lib, const char* name);
    virtual ~CFormat();

    virtual bool Load(const char* filename, unsigned subImage = 0) = 0;
    virtual void FreeMemory();

private:
    Callback m_callback;
    int m_percent;

protected:
    void* m_lib;
    std::vector<unsigned char> m_bitmap;
    GLenum m_format;
    unsigned m_width, m_height, m_pitch; // width, height, row pitch of image in buffer
    unsigned m_bpp;                      // bit per pixel of image in buffer
    unsigned m_bppImage;                 // bit per pixel of original image
    long m_size;                         // file size on disk
    unsigned m_subImage;
    unsigned m_subCount;
    std::string m_info;                  // additional info, such as EXIF

protected:
    void progress(int percent);
    void reset();
    uint16_t read_uint16(uint8_t* p);
    uint32_t read_uint32(uint8_t* p);
    void swap_long(uint8_t* bp, uint32_t n);

private:
    CFormat();
};

#endif // FORMAT_H

