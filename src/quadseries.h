/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "quad.h"

class cQuadSeries : public cQuad
{
public:
    cQuadSeries(unsigned tw, unsigned th, const unsigned char* data, GLenum bitmapFormat);
    virtual ~cQuadSeries();

    void Setup(unsigned width, unsigned height, unsigned count);
    void SetFrame(unsigned frame);

private:
    unsigned m_count;
    unsigned m_frame;
};
