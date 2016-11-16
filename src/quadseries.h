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

class CQuadSeries : public CQuad
{
public:
    CQuadSeries(unsigned tw, unsigned th, const unsigned char* data, GLenum bitmapFormat);
    virtual ~CQuadSeries();

    void Setup(unsigned width, unsigned height, unsigned count);
    void SetFrame(unsigned frame);

private:
    unsigned m_count;
    unsigned m_frame;
};
