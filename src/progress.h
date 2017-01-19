/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <memory>

class CQuad;

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

    std::unique_ptr<CQuad> m_back;
    struct sDot
    {
        float alpha;
        std::unique_ptr<CQuad> dot;
    };
    sDot m_dot[4];

    unsigned m_index;
    float m_time;
};
