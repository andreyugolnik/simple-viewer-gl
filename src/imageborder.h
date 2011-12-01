/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef IMAGEBORDER_H_7978ADDAEE195E
#define IMAGEBORDER_H_7978ADDAEE195E

#include "renderer.h"

class CImageBorder
{
public:
    CImageBorder();
    virtual ~CImageBorder();

    void SetColor(int _r, int _g, int _b, int _a);
    void Render(int _w, int _h);
    int GetBorderWidth() const;

private:
    sLine m_line;

private:
    void renderLine(float _x1, float _y1, float _x2, float _y2);
};

#endif /* end of include guard: IMAGEBORDER_H_7978ADDAEE195E */

