/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "renderer.h"
#include "math/vector.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

static GLFWwindow* m_window = nullptr;
static cVector<float> m_viewportSize;
static unsigned m_tex = 0;
static sVertex m_vb[4];
static unsigned short m_ib[6] = { 0, 1, 2, 0, 2, 3 };
static bool m_npot = false;
static unsigned MaxTextureSize = 1024;

void cRenderer::setWindow(GLFWwindow* window)
{
    m_window = window;
    m_tex = 0;

    int texture_max_size = (int)MaxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_max_size);
    MaxTextureSize = std::min<unsigned>(MaxTextureSize, texture_max_size);
    //printf("Max texture size: %d x %d.\n", MaxTextureSize, MaxTextureSize);

    m_npot = glfwExtensionSupported("GL_ARB_texture_non_power_of_two");
    //printf("Non Power of Two extension %s\n", m_npot ? "available." : "not available.");

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
}

GLFWwindow* cRenderer::getWindow()
{
    return m_window;
}

void cRenderer::setData(GLuint tex, const unsigned char* data, unsigned w, unsigned h, GLenum format)
{
    bindTexture(tex);

    if (tex != 0 && data != nullptr)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //std::cout << "creating " << tw << " x " << th << " texture" << std::endl;
        GLenum type = 0;
        GLint fmt = format;
        if (format == GL_RGB || format == GL_BGR)
        {
            fmt    = GL_RGB;
            type   = GL_UNSIGNED_BYTE;
        }
        else if (format == GL_RGBA || format == GL_BGRA)
        {
            fmt    = GL_RGBA;
            type   = GL_UNSIGNED_BYTE;
        }
        else if (format == GL_UNSIGNED_SHORT_4_4_4_4)
        {
            format = GL_RGBA;
            fmt    = GL_RGBA;
            type   = GL_UNSIGNED_SHORT_4_4_4_4;
        }
        else if (format == GL_UNSIGNED_SHORT_5_6_5)
        {
            format = GL_RGB;
            fmt    = GL_RGB;
            type   = GL_UNSIGNED_SHORT_5_6_5;
        }
        else if (format == GL_UNSIGNED_SHORT_5_5_5_1)
        {
            format = GL_RGBA;
            fmt    = GL_RGBA;
            type   = GL_UNSIGNED_SHORT_5_5_5_1;
        }
        else if (format == GL_LUMINANCE || format == GL_LUMINANCE_ALPHA || format == GL_ALPHA)
        {
            type   = GL_UNSIGNED_BYTE;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, format, type, data);
        checkError("setData");
    }
}

GLuint cRenderer::createTexture()
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    checkError("createTexture");

    return tex;
}

bool cRenderer::checkError(const char* msg)
{
    bool result = false;
    for (GLenum e = glGetError(); e != GL_NO_ERROR; e = glGetError())
    {
        result = true;
        //const GLubyte* s   = gluErrorString(e);
        printf("%s : 0x%x\n", msg, e);
    }

    return result;
}

void cRenderer::deleteTexture(GLuint tex)
{
    if (m_tex == tex)
    {
        bindTexture(0);
    }
    glDeleteTextures(1, &tex);
}

void cRenderer::bindTexture(GLuint tex)
{
    if (m_tex != tex)
    {
        m_tex = tex;
        glBindTexture(GL_TEXTURE_2D, m_tex);
    }
}

static unsigned nextPOT(unsigned n, bool npot)
{
    if (npot)
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

unsigned cRenderer::calculateTextureSize(unsigned size)
{
    return std::min<unsigned>(MaxTextureSize, nextPOT(size, m_npot));
}

void cRenderer::setColor(sLine* line, int r, int g, int b, int a)
{
    for (unsigned i = 0; i < 2; i++)
    {
        line->v[i].r = r;
        line->v[i].g = g;
        line->v[i].b = b;
        line->v[i].a = a;
    }
}

void cRenderer::setColor(sQuad* quad, int r, int g, int b, int a)
{
    for (unsigned i = 0; i < 4; i++)
    {
        quad->v[i].r = r;
        quad->v[i].g = g;
        quad->v[i].b = b;
        quad->v[i].a = a;
    }
}

void cRenderer::render(const sLine& line)
{
    bindTexture(line.tex);

    m_vb[0] = line.v[0];
    m_vb[1] = line.v[1];

    glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, m_ib);
}

void cRenderer::render(const sQuad& quad)
{
    bindTexture(quad.tex);

    m_vb[0] = quad.v[0];
    m_vb[1] = quad.v[1];
    m_vb[2] = quad.v[2];
    m_vb[3] = quad.v[3];

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, m_ib);
}

const cVector<float>& cRenderer::getViewportSize()
{
    return m_viewportSize;
}

void cRenderer::setViewportSize(const cVector<float>& size)
{
    glViewport(0, 0, size.x, size.y);
    m_viewportSize = size;
}

void cRenderer::resetGlobals()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        0.0f,
        0.0f + m_viewportSize.x,
        0.0f + m_viewportSize.y,
        0.0f,
        -1.0f, 1.0f);

    glRotatef(0.0f, 0.0f, 0.0f, -1.0f);
}

void cRenderer::setGlobals(const cVector<float>& delta, float angle, float zoom)
{
    const float z = 1.0f / zoom;
    const float w = m_viewportSize.x * z;
    const float h = m_viewportSize.y * z;

    const float x = delta.x - w * 0.5f;
    const float y = delta.y - h * 0.5f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        x,
        x + w,
        y + h,
        y,
        -1.0f, 1.0f);

    glRotatef(angle, 0.0f, 0.0f, -1.0f);
}
