/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "quad.h"
#include <memory>

class CCheckerboard final
{
public:
    void init();

    void render(bool checkboardEanbled);
    void setColor(float r, float g, float b)
    {
        m_r = r;
        m_g = g;
        m_b = b;
    }

private:
    GLfloat m_r = 0.0f;
    GLfloat m_g = 0.0f;
    GLfloat m_b = 1.0f;
    std::unique_ptr<CQuad> m_cb;

    // double m_lastTime = 0.0f;

    const unsigned m_cellSize = 16;
    const unsigned m_texSize = m_cellSize * 2;
    // float m_texOffset = 0.0f;
};

