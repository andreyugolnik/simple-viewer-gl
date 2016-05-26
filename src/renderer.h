/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "math/vector.h"

#include <GLFW/glfw3.h>

struct sVertex
{
    GLfloat x, y;
    GLfloat tx, ty;
    GLubyte r = 255;
    GLubyte g = 255;
    GLubyte b = 255;
    GLubyte a = 255;
};

struct sLine
{
    GLuint tex = 0;
    sVertex v[2];
};

struct sQuad
{
    GLuint tex = 0;
    sVertex v[4];
};

class cRenderer
{
public:
    static void init();

    static GLuint createTexture(const unsigned char* data, unsigned w, unsigned h, GLenum format);
    static void deleteTexture(GLuint tex);
    static void bindTexture(GLuint tex);
    static void calculateTextureSize(int* tex_w, int* tex_h, int img_w, int img_h);
    static void setColor(sLine* line, int r, int g, int b, int a);
    static void setColor(sQuad* quad, int r, int g, int b, int a);
    static void render(const sLine& quad);
    static void render(const sQuad& quad);

    static void setWindowSize(const cVector<float>& size);
    static const cVector<float>& getWindowSize();
    static void resetGlobals();
    static void setGlobals(const cVector<float>& delta, float angle, float zoom);
};

