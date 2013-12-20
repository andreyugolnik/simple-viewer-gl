////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 19-Aug-2011
// changed: 10-Jun-2012
// version: 0.0.0.116
//
////////////////////////////////////////////////

#include "renderer.h"
#include "vector.h"

#include <iostream>
#include <cmath>

//bool cRenderer::m_inited = false;
unsigned cRenderer::m_tex = 0;
sVertex cRenderer::m_vb[4];
unsigned short cRenderer::m_ib[6] = { 0, 1, 2, 0, 2, 3 };
bool cRenderer::m_pow2 = false;
unsigned cRenderer::m_texture_max_size = 256;

void cRenderer::init()
{
    //if(!m_inited)
    {
        int texture_max_size = (int)m_texture_max_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_max_size);
        m_texture_max_size = std::min<unsigned>(512, texture_max_size);
        std::cout << "Using texture size: " << m_texture_max_size << "x"  << m_texture_max_size << "." << std::endl;

        m_pow2 = glutExtensionSupported("GL_ARB_texture_non_power_of_two");
        std::cout << "Non Power of Two extension " << (m_pow2 ? "available." : "not available.") << std::endl;

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, sizeof(sVertex), &m_vb->x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(sVertex), &m_vb->r);
        glTexCoordPointer(2, GL_FLOAT, sizeof(sVertex), &m_vb->tx);

        //disable(false);
    }
}

//void cRenderer::disable(bool _disable)
//{
    //m_inited = !_disable;

    ////std::cout << "Renderer " << (m_inited ? "inited" : "disabled") << std::endl;
//}

GLuint cRenderer::createTexture(const unsigned char* data, unsigned w, unsigned h, GLenum format)
{
    GLuint tex = 0;
    //if(m_inited)
    {
        if(data)
        {
            glGenTextures(1, &tex);
        }

        bindTexture(tex);

        if(data)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            //std::cout << "creating " << tw << " x " << th << " texture" << std::endl;
            GLint bytes = ((format == GL_RGBA || format == GL_BGRA) ? 4 : 3);
            glTexImage2D(GL_TEXTURE_2D, 0, bytes, w, h, 0, format, GL_UNSIGNED_BYTE, data);
            GLenum e = glGetError();
            if(GL_NO_ERROR != e)
            {
                //const GLubyte* s   = gluErrorString(e);
                std::cout << "can't update texture " << tex << ": " << e << std::endl;
            }
        }
    }

    return tex;
}

void cRenderer::deleteTexture(GLuint tex)
{
    //if(m_inited)
    {
        bindTexture(0);
        if(tex != 0)
        {
            glDeleteTextures(1, &tex);
        }
    }
}

void cRenderer::bindTexture(GLuint tex)
{
    //if(m_inited)
    {
        if(m_tex != tex)
        {
            m_tex = tex;
            glBindTexture(GL_TEXTURE_2D, m_tex);
        }
    }
}

unsigned nextPOT(unsigned n, bool pow2)
{
    if(pow2)
    {
        return n;
    }

    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    return n + 1;
}

void cRenderer::calculateTextureSize(unsigned* tex_w, unsigned* tex_h, unsigned img_w, unsigned img_h)
{
    unsigned tw = std::min<unsigned>(m_texture_max_size, nextPOT(img_w, m_pow2));
    unsigned th = std::min<unsigned>(m_texture_max_size, nextPOT(img_h, m_pow2));
    //std::cout << "  select texture size: " << tw << " x " << th << std::endl;
    *tex_w = tw;
    *tex_h = th;
}

void cRenderer::setColor(sLine* line, int r, int g, int b, int a)
{
    for(unsigned i = 0; i < 2; i++)
    {
        line->v[i].r = r;
        line->v[i].g = g;
        line->v[i].b = b;
        line->v[i].a = a;
    }
}

void cRenderer::setColor(sQuad* quad, int r, int g, int b, int a)
{
    for(unsigned i = 0; i < 4; i++)
    {
        quad->v[i].r = r;
        quad->v[i].g = g;
        quad->v[i].b = b;
        quad->v[i].a = a;
    }
}

void cRenderer::render(sLine* line)
{
    //if(m_inited)
    {
        bindTexture(line->tex);

        m_vb[0] = line->v[0];
        m_vb[1] = line->v[1];

        glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, m_ib);
    }
}

void cRenderer::render(sQuad* quad)
{
    //if(m_inited)
    {
        bindTexture(quad->tex);

        m_vb[0] = quad->v[0];
        m_vb[1] = quad->v[1];
        m_vb[2] = quad->v[2];
        m_vb[3] = quad->v[3];

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, m_ib);
    }
}

void cRenderer::setGlobals(const cVector* delta, float angle, float zoom)
{
    const float z = 1.0f / zoom;
    const float w = glutGet(GLUT_WINDOW_WIDTH) * z;
    const float h = glutGet(GLUT_WINDOW_HEIGHT) * z;

    const float x = floorf((delta ? delta->x : 0.0f) - w * 0.5f);
    const float y = floorf((delta ? delta->y : 0.0f) - h * 0.5f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
            x,
            x + w,
            y + h,
            y,
            -1.0, 1.0);

    glRotatef(angle, 0.0f, 0.0f, -1.0f);
}

