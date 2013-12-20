////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 19-Aug-2011
// changed: 10-Jun-2012
// version: 0.0.0.56
//
////////////////////////////////////////////////

#ifndef RENDERER_H_E44EFE71C29EF8
#define RENDERER_H_E44EFE71C29EF8

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

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

    //static void disable(bool _disable);
    static GLuint createTexture(const unsigned char* data, unsigned w, unsigned h, GLenum format);
    static void deleteTexture(GLuint tex);
    static void bindTexture(GLuint tex);
    static void calculateTextureSize(unsigned* tex_w, unsigned* tex_h, unsigned img_w, unsigned img_h);
    static void setColor(sLine* line, int r, int g, int b, int a);
    static void setColor(sQuad* quad, int r, int g, int b, int a);
    static void render(sLine* quad);
    static void render(sQuad* quad);
    static void setGlobals(const cVector* delta = 0, float angle = 0.0f, float zoom = 1.0f);

private:
    //static bool m_inited;
    static unsigned m_tex;
    static sVertex m_vb[4];
    static unsigned short m_ib[6];
    static bool m_pow2;
    static unsigned m_texture_max_size;
};

#endif /* end of include guard: RENDERER_H_E44EFE71C29EF8 */

