/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef RENDERER_H_E44EFE71C29EF8
#define RENDERER_H_E44EFE71C29EF8

#define GL_GLEXT_PROTOTYPES
#if defined(__linux__)
#   include <GL/glut.h>
#else
#   include <glut.h>
#endif

class cVector;

struct sVertex
{
    sVertex()
        : r(255), g(255), b(255), a(255)
    { }
    GLfloat x, y;
    GLfloat tx, ty;
    GLubyte r, g, b, a;
};

struct sLine
{
    sLine()
        : tex(0)
    { }
    GLuint tex;
    sVertex v[2];
};

struct sQuad
{
    sQuad()
        : tex(0)
    { }
    GLuint tex;
    sVertex v[4];
};

class cRenderer
{
public:
    static void init();

    static GLuint createTexture(const unsigned char* data, unsigned w, unsigned h, GLenum format);
    static void deleteTexture(GLuint tex);
    static void bindTexture(GLuint tex);
    static void calculateTextureSize(unsigned* tex_w, unsigned* tex_h, unsigned img_w, unsigned img_h);
    static void setColor(sLine* line, int r, int g, int b, int a);
    static void setColor(sQuad* quad, int r, int g, int b, int a);
    static void render(sLine* quad);
    static void render(sQuad* quad);

    static void setWindowSize(const cVector& size);
    static const cVector& getWindowSize();
    static void resetGlobals();
    static void setGlobals(const cVector& delta, float angle, float zoom);
};

#endif /* end of include guard: RENDERER_H_E44EFE71C29EF8 */

