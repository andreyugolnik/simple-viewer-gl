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

class CProgress final
{
public:
    void init();

    void show()
    {
        m_visible = true;
    }

    void setProgress(float progress)
    {
        m_progress = progress;
    }

    void hide()
    {
        m_visible = false;
    }

    void render();

private:
    float m_visible = false;
    float m_progress = 0.0f;
    std::unique_ptr<CQuad> m_loading;
    std::unique_ptr<CQuad> m_square;
};
