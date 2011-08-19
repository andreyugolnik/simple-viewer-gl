////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 19-Aug-2011
// changed: 20-Aug-2011
// version: 0.0.0.42
//
////////////////////////////////////////////////

#include "renderer.h"
#include <iostream>

bool cRenderer::m_inited = false;
GLuint cRenderer::m_tex = 0;
sVertex cRenderer::m_vb[4];
unsigned short cRenderer::m_ib[6] = { 0, 1, 2, 0, 2, 3 };

void cRenderer::init()
{
    if(!m_inited)
    {
        m_inited = true;

        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, sizeof(sVertex), &m_vb->x);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(sVertex), &m_vb->r);
        glTexCoordPointer(2, GL_FLOAT, sizeof(sVertex), &m_vb->tx);
    }
}

GLuint cRenderer::createTexture(const unsigned char* _data, int _w, int _h, GLenum _format)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);

    bindTexture(tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if(_data)
    {
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

    return tex;
}

void cRenderer::deleteTexture(GLuint _tex)
{
    bindTexture(0);
    if(_tex != 0)
    {
        glDeleteTextures(1, &_tex);
    }
}

void cRenderer::bindTexture(GLuint _tex)
{
    if(m_tex != _tex)
    {
        m_tex = _tex;
        glBindTexture(GL_TEXTURE_2D, m_tex);
    }
}

void cRenderer::setColor(sQuad* _quad, int _r, int _g, int _b, int _a)
{
    _quad->v[0].r = _quad->v[1].r = _quad->v[2].r = _quad->v[3].r = _r;
    _quad->v[0].g = _quad->v[1].g = _quad->v[2].g = _quad->v[3].g = _g;
    _quad->v[0].b = _quad->v[1].b = _quad->v[2].b = _quad->v[3].b = _b;
    _quad->v[0].a = _quad->v[1].a = _quad->v[2].a = _quad->v[3].a = _a;
}

void cRenderer::render(sQuad* _quad)
{
    bindTexture(_quad->tex);

    m_vb[0] = _quad->v[0];
    m_vb[1] = _quad->v[1];
    m_vb[2] = _quad->v[2];
    m_vb[3] = _quad->v[3];

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, m_ib);
}

void cRenderer::render(sLine* _line)
{
    bindTexture(_line->tex);

    m_vb[0] = _line->v[0];
    m_vb[1] = _line->v[1];

    glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, m_ib);
}

