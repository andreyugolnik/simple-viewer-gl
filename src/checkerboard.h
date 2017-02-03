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

class cCheckerboard final
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
    std::unique_ptr<cQuad> m_cb;
};

