/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "checkerboard.h"
#include <math.h>

const int tex_size = 128;

CCheckerboard::CCheckerboard()
    : m_enabled(true)
    , m_r(0)
    , m_g(0)
    , m_b(1)
{
}

CCheckerboard::~CCheckerboard()
{
}

void CCheckerboard::Init()
{
    unsigned char* buffer = new unsigned char[tex_size * tex_size * 3];
    unsigned char* p = buffer;
    bool checker_height_odd = true;
    for(int y = 0; y < tex_size; y++)
    {
        if(y % 16 == 0)
        {
            checker_height_odd = !checker_height_odd;
        }

        bool checker_width_odd = checker_height_odd;
        for(int x = 0; x < tex_size; x++)
        {
            if(x % 16 == 0)
            {
                checker_width_odd = !checker_width_odd;
            }

            unsigned char color	= (checker_width_odd == true ? 0xc8 : 0x7d);
            *p++ = color;
            *p++ = color;
            *p++ = color;
        }
    }

    m_cb.reset(new CQuad(tex_size, tex_size, buffer, GL_RGB));
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
        float w	= (float)glutGet(GLUT_WINDOW_WIDTH);
        float h	= (float)glutGet(GLUT_WINDOW_HEIGHT);
        m_cb->SetSpriteSize(w, h);
        m_cb->Render(-w/2, -h/2);
    }
    else
    {
        glClearColor(m_r, m_g, m_b, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

