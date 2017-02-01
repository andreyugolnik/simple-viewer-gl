/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "checkerboard.h"
#include <cmath>
#include <vector>

void CCheckerboard::init()
{
    std::vector<unsigned char> buffer(m_texSize * m_texSize * 3);
    auto p = buffer.data();

    unsigned idx = 0;
    const unsigned char colors[2] = { 0xc8, 0x7d };

    for (unsigned y = 0; y < m_texSize; y++)
    {
        if (y % m_cellSize == 0)
        {
            idx = (idx + 1) % 2;
        }

        for (unsigned x = 0; x < m_texSize; x++)
        {
            if (x % m_cellSize == 0)
            {
                idx = (idx + 1) % 2;
            }

            const auto color = colors[idx];
            *p++ = color;
            *p++ = color;
            *p++ = color;
        }
    }

    m_cb.reset(new CQuad(m_texSize, m_texSize, buffer.data(), GL_RGB));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // m_lastTime = glfwGetTime();
}

void CCheckerboard::render(bool checkboardEanbled)
{
    if (checkboardEanbled)
    {
        int width;
        int height;
        glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

        // const float current = glfwGetTime();
        // const float dt = current - m_lastTime;
        // m_lastTime = current;

        // const float speed = 5.0f * M_PI / 180.0f;
        // m_texOffset += dt * speed;
        // if (m_texOffset >= M_PI)
        // {
            // m_texOffset -= M_PI * 2.0f;
        // }

        // const float radius = 16.0f;
        // const float x = radius + cosf(m_texOffset) * radius;
        // const float y = radius + sinf(m_texOffset) * radius;

        // m_cb->SetTextureRect(x, y, width, height);
        m_cb->SetTextureRect(0, 0, width, height);
        m_cb->Render(0.0f, 0.0f);
    }
    else
    {
        glClearColor(m_r, m_g, m_b, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
