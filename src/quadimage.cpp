/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "quadimage.h"

CQuadImage::CQuadImage(unsigned tw, unsigned th, const unsigned char* data, GLenum bitmapFormat)
    : CQuad(tw, th, data, bitmapFormat)
    , m_col(0)
    , m_row(0)
{
}

CQuadImage::~CQuadImage()
{
}

