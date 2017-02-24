/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "checkerboard.h"
#include "common/config.h"
#include "common/helpers.h"

#include <cmath>
#include <vector>

void cCheckerboard::init(const sConfig& config)
{
    const uint32_t cellSize = helpers::nextPot(config.bgCellSize);
    const uint32_t texSize = cellSize * 2;

    std::vector<uint8_t> buffer(texSize * texSize);
    auto p = buffer.data();

    uint32_t idx = 0;
    const uint8_t colors[2] = { 0xc8, 0x7d };

    for (uint32_t y = 0; y < texSize; y++)
    {
        if (y % cellSize == 0)
        {
            idx = (idx + 1) % 2;
        }

        for (uint32_t x = 0; x < texSize; x++)
        {
            if (x % cellSize == 0)
            {
                idx = (idx + 1) % 2;
            }

            const auto color = colors[idx];
            *p++ = color;
        }
    }

    m_cb.reset(new cQuad(texSize, texSize, buffer.data(), GL_LUMINANCE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    m_cb->useFilter(false);
}

void cCheckerboard::render(bool checkboardEanbled)
{
    if (checkboardEanbled)
    {
        int width;
        int height;
        glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

        m_cb->setTextureRect({ 0.0f, 0.0f }, { (float)width, (float)height });
        m_cb->render({ 0.0f, 0.0f });
    }
    else
    {
        const auto& c = m_color;
        const float inv = 1.0f / 255.0f;
        glClearColor(c.r * inv, c.g * inv, c.b * inv, c.a * inv);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
