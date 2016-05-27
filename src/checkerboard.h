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

    void Render();
    void Enable(bool enable = true) { m_enabled = enable; }
    bool IsEnabled() const { return m_enabled; }
    void SetColor(int r, int g, int b);

private:
    bool m_enabled = true;
    GLfloat m_r = 0.0f;
    GLfloat m_g = 0.0f;
    GLfloat m_b = 1.0f;
    std::unique_ptr<CQuad> m_cb;
};

