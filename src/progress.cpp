/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "progress.h"
#include "img-loading.c"

#include <iostream>
#include <cmath>

void CProgress::init()
{
    int format = (imgLoading.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
    m_loading.reset(new CQuad(imgLoading.width, imgLoading.height, imgLoading.pixel_data, format));
    m_square.reset(new CQuad(4, 4));
}

void CProgress::render()
{
    if (m_visible)
    {
        int w, h;
        glfwGetFramebufferSize(cRenderer::getWindow(), &w, &h);
        float x = ceilf((w - imgLoading.width) * 0.5f);
        float y = ceilf((h - imgLoading.height) * 0.5f);

        m_loading->Render(x, y);

        const int count = 20;
        const float step = imgLoading.width / count;
        x = ceilf((w - step * count) / 2);
        for (int i = 0, size = count * m_progress; i < size; i++)
        {
            m_square->Render(x + i * step, y + imgLoading.height);
        }
    }
}
