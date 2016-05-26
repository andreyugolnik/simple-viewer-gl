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

class CFTSymbol final
{
public:
    CFTSymbol(const sQuad& quad, int tw, int th, float tx, float ty, int w, int h);
    ~CFTSymbol();

    void Render(int x, int y);

private:
    sQuad m_quad;
    int m_w;
    int m_h;
};

