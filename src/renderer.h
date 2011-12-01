////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
// created: 19-Aug-2011
// changed: 01-Dec-2011
// version: 0.0.0.54
//
////////////////////////////////////////////////

#ifndef RENDERER_H_E44EFE71C29EF8
#define RENDERER_H_E44EFE71C29EF8

#define GL_GLEXT_PROTOTYPES
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
    static GLuint createTexture(const unsigned char* _data, int _w, int _h, GLenum _format);
    static void deleteTexture(GLuint _tex);
    static void bindTexture(GLuint _tex);
    static void calculateTextureSize(int* _tex_w, int* _tex_h, int _img_w, int _img_h);
    static void setColor(sLine* _line, int _r, int _g, int _b, int _a);
    static void setColor(sQuad* _quad, int _r, int _g, int _b, int _a);
    static void render(sLine* _quad);
    static void render(sQuad* _quad);
    static void setGlobals(float _x = 0, float _y = 0, float _angle = 0, float _zoom = 1);

private:
    //static bool m_inited;
    static unsigned m_tex;
    static sVertex m_vb[4];
    static unsigned short m_ib[6];
    static bool m_pow2;
    static int m_texture_max_size;
};

#endif /* end of include guard: RENDERER_H_E44EFE71C29EF8 */

