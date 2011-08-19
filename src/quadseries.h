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
    CQuadSeries(int tw, int th, const unsigned char* data, GLenum bitmapFormat);
    virtual ~CQuadSeries();

    void Setup(int width, int height, int count);
    void SetFrame(int frame);

private:
    int m_count;
    int m_frame;

private:
    CQuadSeries();
};

#endif // QUADSERIES_H

