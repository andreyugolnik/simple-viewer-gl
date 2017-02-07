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
    cFTSymbol(GLuint texId, float tw, float th, float tx, float ty, float w, float h);
    ~cFTSymbol();

    void render(const Vectorf& pos, const cColor& color);

private:
    float m_w;
    float m_h;
    sQuad m_quad;
};
