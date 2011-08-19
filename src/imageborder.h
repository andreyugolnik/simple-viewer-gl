/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGEBORDER_H
#define IMAGEBORDER_H

#include "renderer.h"

class CImageBorder
{
public:
    CImageBorder();
    virtual ~CImageBorder();

    void SetColor(int r, int g, int b, int a);
    void Render(int x, int y, int w, int h);
    int GetBorderWidth() const;

private:
    sLine m_line;

private:
    void renderLine(float x1, float y1, float x2, float y2);
};

#endif // IMAGEBORDER_H

