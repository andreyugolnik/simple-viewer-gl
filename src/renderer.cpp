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
int cRenderer::m_texture_max_size = 256;

void cRenderer::init()
{
    //if(!m_inited)
    {
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_texture_max_size);
        m_texture_max_size = std::min<int>(512, m_texture_max_size);
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

GLuint cRenderer::createTexture(const unsigned char* _data, int _w, int _h, GLenum _format)
{
    GLuint tex = 0;
    //if(m_inited)
    {
        if(_data)
        {
            glGenTextures(1, &tex);
        }

        bindTexture(tex);

        if(_data)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            //std::cout << "creating " << tw << " x " << th << " texture" << std::endl;
            GLint bytes = ((_format == GL_RGBA || _format == GL_BGRA) ? 4 : 3);
            glTexImage2D(GL_TEXTURE_2D, 0, bytes, _w, _h, 0, _format, GL_UNSIGNED_BYTE, _data);
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

void cRenderer::deleteTexture(GLuint _tex)
{
    //if(m_inited)
    {
        bindTexture(0);
        if(_tex != 0)
        {
            glDeleteTextures(1, &_tex);
        }
    }
}

void cRenderer::bindTexture(GLuint _tex)
{
    //if(m_inited)
    {
        if(m_tex != _tex)
        {
            m_tex = _tex;
            glBindTexture(GL_TEXTURE_2D, m_tex);
        }
    }
}

void cRenderer::calculateTextureSize(int* _tex_w, int* _tex_h, int _img_w, int _img_h)
{
    int tw = std::min<int>(m_texture_max_size, _img_w);
    int th = std::min<int>(m_texture_max_size, _img_h);

    // correct texture size
    if(m_pow2 == false)
    {
        float power_w = logf(tw) / logf(2.0f);
        float power_h = logf(th) / logf(2.0f);
        if(static_cast<int>(power_w) != power_w || static_cast<int>(power_h) != power_h)
        {
            tw = static_cast<int>(powf(2.0f, static_cast<int>(ceilf(power_w))));
            th = static_cast<int>(powf(2.0f, static_cast<int>(ceilf(power_h))));
        }
    }
    //std::cout << "  select texture size: " << tw << " x " << th << std::endl;
    *_tex_w = tw;
    *_tex_h = th;
}

void cRenderer::setColor(sLine* _line, int _r, int _g, int _b, int _a)
{
    //_line->v[0].r = _line->v[1].r = _r;
    //_line->v[0].g = _line->v[1].g = _g;
    //_line->v[0].b = _line->v[1].b = _b;
    //_line->v[0].a = _line->v[1].a = _a;
    for(int i = 0; i < 2; i++)
    {
        _line->v[i].r = _r;
        _line->v[i].g = _g;
        _line->v[i].b = _b;
        _line->v[i].a = _a;
    }
}

void cRenderer::setColor(sQuad* _quad, int _r, int _g, int _b, int _a)
{
    //_quad->v[0].r = _quad->v[1].r = _quad->v[2].r = _quad->v[3].r = _r;
    //_quad->v[0].g = _quad->v[1].g = _quad->v[2].g = _quad->v[3].g = _g;
    //_quad->v[0].b = _quad->v[1].b = _quad->v[2].b = _quad->v[3].b = _b;
    //_quad->v[0].a = _quad->v[1].a = _quad->v[2].a = _quad->v[3].a = _a;
    for(int i = 0; i < 4; i++)
    {
        _quad->v[i].r = _r;
        _quad->v[i].g = _g;
        _quad->v[i].b = _b;
        _quad->v[i].a = _a;
    }
}

void cRenderer::render(sLine* _line)
{
    //if(m_inited)
    {
        bindTexture(_line->tex);

        m_vb[0] = _line->v[0];
        m_vb[1] = _line->v[1];

        glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, m_ib);
    }
}

void cRenderer::render(sQuad* _quad)
{
    //if(m_inited)
    {
        bindTexture(_quad->tex);

        m_vb[0] = _quad->v[0];
        m_vb[1] = _quad->v[1];
        m_vb[2] = _quad->v[2];
        m_vb[3] = _quad->v[3];

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, m_ib);
    }
}

void cRenderer::setGlobals(const cVector* _delta, float _angle, float _zoom)
{
    float zoom = 1.0f / _zoom;
    float w = glutGet(GLUT_WINDOW_WIDTH) * zoom;
    float h = glutGet(GLUT_WINDOW_HEIGHT) * zoom;

    float x = floorf((_delta ? _delta->x : 0) - w * 0.5f);
    float y = floorf((_delta ? _delta->y : 0) - h * 0.5f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
            x,
            x + w,
            y + h,
            y,
            -1.0, 1.0);

    glRotatef(_angle, 0, 0, -1);
}

