/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "notavailable.h"
#include "img-na.c"

CNotAvailable::CNotAvailable()
    : CFormat(0, "n/a")
{
}

CNotAvailable::~CNotAvailable()
{
}

bool CNotAvailable::Load(const char* filename, unsigned subImage)
{
    (void)filename;
    (void)subImage;

    m_format   = imgNa.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA;
    m_width    = imgNa.width;
    m_height   = imgNa.height;
    m_bpp      = imgNa.bytes_per_pixel * 8;
    m_bppImage = 0;
    m_pitch    = m_width * imgNa.bytes_per_pixel;

    const unsigned size = m_pitch * m_height;
    m_bitmap.resize(size);
    ::memcpy(&m_bitmap[0], imgNa.pixel_data, size);

    return true;
}

