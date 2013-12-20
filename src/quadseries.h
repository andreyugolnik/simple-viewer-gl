/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUADSERIES_H
#define QUADSERIES_H

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

private:
    CQuadSeries();
};

#endif // QUADSERIES_H

