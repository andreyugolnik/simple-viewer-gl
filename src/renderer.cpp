/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
*  Texture filtration
*  by Timo Suoranta <tksuoran@gmail.com>
*
\**********************************************/

#include "renderer.h"
#include "common/helpers.h"
#include "types/vector.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace
{
    GLFWwindow* Window = nullptr;
    Rectf ViewRect;
    float ViewZoom = 1.0f;
    int ViewAngle = 0.0f;
    Vectori ViewportSize;
    uint32_t CurrentTextureId = 0;
    sVertex Vb[4];
    uint16_t Ib[6] = { 0, 1, 2, 0, 2, 3 };
    bool Npot = false;
    uint32_t TextureSizeLimit = 1024;
    bool IsMipmapEnabled = false;
}

void cRenderer::init(GLFWwindow* window, uint32_t maxTextureSize)
{
    Window = window;
    CurrentTextureId = 0;

    int maxSize = maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    // ::printf("(II) Max texture size: %d.\n", maxSize);

    TextureSizeLimit = std::min<uint32_t>(maxSize, maxTextureSize);
    // ::printf("(II) Limit texture size: %u.\n", TextureSizeLimit);

    Npot = glfwExtensionSupported("GL_ARB_texture_non_power_of_two");
    // ::printf("(II) Non Power of Two extension %s\n", Npot ? "available." : "not available.");
}

void cRenderer::shutdown()
{
    Window = nullptr;
    ViewZoom = 1.0f;
    ViewAngle = 0.0f;
    CurrentTextureId = 0;
    Npot = false;
    TextureSizeLimit = 1024;
    IsMipmapEnabled = false;
}

void cRenderer::beginFrame()
{
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

    glVertexPointer(2, GL_FLOAT, sizeof(sVertex), &Vb->x);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(sVertex), &Vb->color);
    glTexCoordPointer(2, GL_FLOAT, sizeof(sVertex), &Vb->tx);
}

void cRenderer::endFrame()
{
}

GLFWwindow* cRenderer::getWindow()
{
    return Window;
}

void cRenderer::enableMipmap(bool enable)
{
    IsMipmapEnabled = enable;
}

bool cRenderer::isMipmapEnabled()
{
    return IsMipmapEnabled;
}

void cRenderer::setData(GLuint tex, const uint8_t* data, uint32_t w, uint32_t h, GLenum format)
{
    bindTexture(tex);

    if (tex != 0 && data != nullptr)
    {
        if (isMipmapEnabled())
        {
            glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //std::cout << "creating " << tw << " x " << th << " texture" << std::endl;
        GLenum type = 0;
        GLint internalFormat = format;
        if (format == GL_RGB || format == GL_BGR)
        {
            internalFormat = GL_RGB;
            type = GL_UNSIGNED_BYTE;
        }
        else if (format == GL_RGBA || format == GL_BGRA)
        {
            internalFormat = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
        }
        else if (format == GL_UNSIGNED_SHORT_4_4_4_4)
        {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_4_4_4_4;
        }
        else if (format == GL_UNSIGNED_SHORT_5_6_5)
        {
            internalFormat = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
        }
        else if (format == GL_UNSIGNED_SHORT_5_5_5_1)
        {
            internalFormat = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_SHORT_5_5_5_1;
        }
        else if (format == GL_LUMINANCE || format == GL_LUMINANCE_ALPHA || format == GL_ALPHA)
        {
            type = GL_UNSIGNED_BYTE;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data);
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
    if (CurrentTextureId == tex)
    {
        bindTexture(0);
    }
    glDeleteTextures(1, &tex);
}

void cRenderer::bindTexture(GLuint tex)
{
    if (CurrentTextureId != tex)
    {
        CurrentTextureId = tex;
        glBindTexture(GL_TEXTURE_2D, CurrentTextureId);
    }
}

uint32_t cRenderer::calculateTextureSize(uint32_t size)
{
    const auto npotSize = Npot ? size : helpers::nextPot(size);
    const auto newSize = std::min<uint32_t>(TextureSizeLimit, npotSize);
    // ::printf("(II) Texutre size: %u.\n", newSize);
    return newSize;
}

void cRenderer::setColor(sLine* line, const cColor& color)
{
    for (uint32_t i = 0; i < 2; i++)
    {
        line->v[i].color = color;
    }
}

void cRenderer::setColor(sQuad* quad, const cColor& color)
{
    for (uint32_t i = 0; i < 4; i++)
    {
        quad->v[i].color = color;
    }
}

void cRenderer::render(const sLine& line)
{
    bindTexture(line.tex);

    Vb[0] = line.v[0];
    Vb[1] = line.v[1];

    glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, Ib);
}

void cRenderer::render(const sQuad& quad)
{
    bindTexture(quad.tex);

    Vb[0] = quad.v[0];
    Vb[1] = quad.v[1];
    Vb[2] = quad.v[2];
    Vb[3] = quad.v[3];

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, Ib);
}

const Vectori& cRenderer::getViewportSize()
{
    return ViewportSize;
}

void cRenderer::setViewportSize(const Vectori& size)
{
    glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
    ViewportSize = size;
}

void cRenderer::resetGlobals()
{
    ViewRect = { { 0.0f, 0.0f }, { (float)ViewportSize.x, (float)ViewportSize.y } };
    ViewZoom = 1.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        0.0f,
        0.0f + ViewportSize.x,
        0.0f + ViewportSize.y,
        0.0f,
        -1.0f, 1.0f);

    glRotatef(0.0f, 0.0f, 0.0f, -1.0f);
}

const Rectf& cRenderer::getRect()
{
    return ViewRect;
}

float cRenderer::getZoom()
{
    return ViewZoom;
}

int cRenderer::getAngle()
{
    return ViewAngle;
}

void cRenderer::setGlobals(const Vectorf& offset, int angle, float zoom)
{
    const float z = 1.0f / zoom;
    const float w = ViewportSize.x * z;
    const float h = ViewportSize.y * z;

    const float x = offset.x - w * 0.5f;
    const float y = offset.y - h * 0.5f;

    ViewRect = { { x, y }, { x + w, y + h } };
    ViewZoom = zoom;
    ViewAngle = angle;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        x,
        x + w,
        y + h,
        y,
        -1.0f, 1.0f);

    glRotatef((float)angle, 0.0f, 0.0f, -1.0f);
}
