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

const int texSize = 128;

void CCheckerboard::init()
{
    unsigned char* buffer = new unsigned char[texSize * texSize * 3];
    unsigned char* p = buffer;
    bool checker_height_odd = true;
    for(int y = 0; y < texSize; y++)
    {
        if(y % 16 == 0)
        {
            checker_height_odd = !checker_height_odd;
        }

        bool checker_width_odd = checker_height_odd;
        for(int x = 0; x < texSize; x++)
        {
            if(x % 16 == 0)
            {
                checker_width_odd = !checker_width_odd;
            }

            unsigned char color = (checker_width_odd == true ? 0xc8 : 0x7d);
            *p++ = color;
            *p++ = color;
            *p++ = color;
        }
    }

    m_cb.reset(new CQuad(texSize, texSize, buffer, GL_RGB));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    delete[] buffer;
}

void CCheckerboard::SetColor(int r, int g, int b)
{
    m_r = r / 255.0f;
    m_g = g / 255.0f;
    m_b = b / 255.0f;
}

void CCheckerboard::Render()
{
    if(m_enabled == true)
    {
        int width;
        int height;
        glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);
        m_cb->SetSpriteSize(width, height);
        m_cb->Render(-width * 0.5f, -height * 0.5f);
    }
    else
    {
        glClearColor(m_r, m_g, m_b, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

