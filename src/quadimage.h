/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef QUADIMAGE_H
#define QUADIMAGE_H

#include "quad.h"

class CQuadImage : public CQuad
{
public:
    CQuadImage(unsigned tw, unsigned th, const unsigned char* data, GLenum bitmapFormat);
    virtual ~CQuadImage();

    void SetCell(unsigned col, unsigned row)
    {
        m_col = col;
        m_row = row;
    }
    int GetCol() const { return m_col; }
    int GetRow() const { return m_row; }

private:
    unsigned m_col;
    unsigned m_row;

private:
    CQuadImage();
};

#endif // QUADIMAGE_H

