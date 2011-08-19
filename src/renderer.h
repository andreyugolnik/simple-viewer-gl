////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 19-Aug-2011
// changed: 20-Aug-2011
// version: 0.0.0.29
//
////////////////////////////////////////////////

#ifndef RENDERER_H_E44EFE71C29EF8
#define RENDERER_H_E44EFE71C29EF8

#include <GL/glut.h>

struct sVertex
{
    sVertex()
        : r(255), g(255), b(255), a(255)
    { }
    GLfloat x, y;
    GLfloat tx, ty;
    GLubyte r, g, b, a;
};

struct sQuad
{
    sQuad()
        : tex(0)
    { }
    GLuint tex;
    sVertex v[4];
};

struct sLine
{
    sLine()
        : tex(0)
    { }
    GLuint tex;
    sVertex v[2];
};

class cRenderer
{
public:
    static void init();

    static GLuint createTexture(const unsigned char* _data, int _w, int _h, GLenum _format);
    static void deleteTexture(GLuint _tex);
    static void bindTexture(GLuint _tex);
    static void setColor(sQuad* _quad, int _r, int _g, int _b, int _a);
    static void render(sQuad* _quad);
    static void render(sLine* _quad);

private:
    static bool m_inited;
    static GLuint m_tex;
    static sVertex m_vb[4];
    static unsigned short m_ib[6];
};

#endif /* end of include guard: RENDERER_H_E44EFE71C29EF8 */

