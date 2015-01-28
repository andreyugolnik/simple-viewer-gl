/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef IMAGEBORDER_H_7978ADDAEE195E
#define IMAGEBORDER_H_7978ADDAEE195E

#include "renderer.h"

class CImageBorder
{
public:
    CImageBorder();
    virtual ~CImageBorder();

    void SetColor(int r, int g, int b, int a);
    void Render(float x, float y, float w, float h, float scale);
    float GetBorderWidth() const;

private:
    sQuad m_line;

private:
    void renderLine(float x, float y, float w, float h);
};

#endif /* end of include guard: IMAGEBORDER_H_7978ADDAEE195E */

