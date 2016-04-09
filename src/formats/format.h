/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMAT_H
#define FORMAT_H

#include <string>
#include <vector>
#if defined(__linux__)
#   include <GL/glut.h>
#else
#   include <glut.h>
#endif

//#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

class iCallbacks;
class cFile;

typedef std::vector<unsigned char> Buffer;

class CFormat
{
    friend class CImageLoader;

public:
    CFormat(const char* libName, const char* formatName);
    virtual ~CFormat();

    virtual void dumpFormat();

    virtual void setCallbacks(iCallbacks* callbacks) { m_callbacks = callbacks; }
    virtual bool isSupported(cFile& /*file*/, Buffer& /*buffer*/) const { return false; }

    virtual bool Load(const char* filename, unsigned subImage = 0) = 0;
    virtual void FreeMemory();

    void progress(int percent);

private:
    iCallbacks* m_callbacks;
    int m_percent;
    std::string m_formatName;

protected:
    void* m_lib;
    enum class eSupport
    {
        Unsupported,
        ExternalLib,
        Internal
    };
    eSupport m_support;
    Buffer m_bitmap;
    GLenum m_format;
    unsigned m_width, m_height, m_pitch; // width, height, row pitch of image in buffer
    unsigned m_bpp;                      // bit per pixel of image in buffer
    unsigned m_bppImage;                 // bit per pixel of original image
    long m_size;                         // file size on disk
    unsigned m_subImage;
    unsigned m_subCount;
    std::string m_info;                  // additional info, such as EXIF

protected:
    void reset();

private:
    CFormat();
};

#endif // FORMAT_H

