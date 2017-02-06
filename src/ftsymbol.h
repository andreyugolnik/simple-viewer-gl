/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "renderer.h"

class cFTSymbol final
{
public:
    cFTSymbol(GLuint texId, int tw, int th, float tx, float ty, int w, int h);
    ~cFTSymbol();

    void render(int x, int y, const cColor& color);

private:
    int m_w;
    int m_h;
    sQuad m_quad;
};
