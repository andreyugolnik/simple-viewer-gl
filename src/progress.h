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

class cQuad;

class cProgress final
{
public:
    cProgress() = default;

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
    bool m_visible = false;
    float m_progress = 0.0f;

    std::unique_ptr<cQuad> m_back;
    struct sDot
    {
        float alpha;
        std::unique_ptr<cQuad> dot;
    };
    sDot m_dot[4];

    unsigned m_index = 0;
    float m_time = 0.0f;
};
