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
#include <GL/gl.h>

//#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

typedef void (*Callback)(int);

class CFormat
{
    friend class CImageLoader;

public:
    CFormat(Callback callback = 0);
    virtual ~CFormat();

    virtual bool Load(const char* filename, int subImage = 0) = 0;
    virtual void FreeMemory();

private:
    Callback m_callback;
    int m_percent;

protected:
    FILE* m_file;
    std::vector<unsigned char> m_bitmap;
    GLenum m_format;
    int m_width, m_height, m_pitch;	// width, height, row pitch of image in buffer
    int m_bpp;						// bit per pixel of image in buffer
    int m_bppImage;					// bit per pixel of original image
    long m_size;					// file size on disk
    int m_subImage;
    int m_subCount;
    std::string m_info;				// additional info, such as EXIF

protected:
    bool openFile(const char* path);
    void progress(int percent);
    void reset();
    uint16_t read_uint16(uint8_t* p);
    uint32_t read_uint32(uint8_t* p);
    void swap_long(uint8_t* bp, uint32_t n);
};

#endif // FORMAT_H

