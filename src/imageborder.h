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
    void Render(float _x, float _y, float _w, float _h, float _scale);
    int GetBorderWidth() const;

private:
    sQuad m_line;

private:
    void renderLine(float _x, float _y, float _w, float _h);
};

#endif /* end of include guard: IMAGEBORDER_H_7978ADDAEE195E */

