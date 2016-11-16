/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "progress.h"
#include "quad.h"

#include <algorithm>
#include <cmath>

const float dotSize = 14.0f;
const float distance = dotSize + 2.0f;

void CProgress::init()
{
    for(auto& dot : m_dot)
    {
        dot.dot.reset(new CQuad(dotSize, dotSize));
    }
}

void CProgress::render()
{
    if (m_visible)
    {
        const float dt = 1.0f / 30.0f;
        m_time -= dt;
        if(m_time <= 0.0f)
        {
            const float nextSpeed = 0.2f;
            m_time = nextSpeed;
            m_index = (m_index + 1) % (sizeof(m_dot) / sizeof(m_dot[0]));
            m_dot[m_index].alpha = 255.0f;
        }

        int w, h;
        glfwGetFramebufferSize(cRenderer::getWindow(), &w, &h);

        const float pos_x = w - distance * 2;
        const float pos_y = h - distance * 2;

        static const unsigned idx[4] = { 0, 1, 3, 2 };
        for(size_t i = 0; i < sizeof(m_dot)/sizeof(m_dot[0]); i++)
        {
            auto& dot = m_dot[i];
            float alpha = dot.alpha;
            if(alpha > 0.0f)
            {
                const float alphaSpeed = 255.0f * 2.0f;
                alpha = std::max<float>(0.0f, alpha - dt * alphaSpeed);
                dot.alpha = alpha;
                dot.dot->SetColor(255.0f, 255.0f, 255.0f, alpha);
                const float x = pos_x + (idx[i] % 2) * distance;
                const float y = pos_y + (idx[i] / 2) * distance;
                dot.dot->Render(x, y);
            }
        }
    }
}
