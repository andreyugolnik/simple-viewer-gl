/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef INFOBAR_H
#define INFOBAR_H

#include "ftstring.h"
#include "quad.h"
#include <string>
#include <memory>

struct sInfoBar
{
    sInfoBar()
        : path(0)
        , type(0)
        , index(0)
        , width(0)
        , height(0)
        , bpp(0)
        , scale(0)
        , sub_image(0)
        , sub_count(0)
        , file_size(0)
        , mem_size(0)
        , files_count(0)
    {
    }

    const char* path;
    const char* type;
    int index;
    int width, height, bpp;
    float scale;
    int sub_image, sub_count;
    long file_size;
    size_t mem_size;
    int files_count;
};

class CInfoBar
{
public:
    CInfoBar();
    virtual ~CInfoBar();

    void Init();
    void Render();
    bool Visible() const { return m_visible; }
    void Show(bool show = true) { m_visible = show; }
    void Update(const sInfoBar& p);
    float GetHeight() const { return (m_visible == true ? m_height : 0); }

private:
    bool m_visible;
    const float m_height;
    const int m_fntSize;
    std::string m_bottominfo;
    std::auto_ptr<CQuad> m_bg;
    std::auto_ptr<CFTString> m_ft;

private:
    const char* getHumanSize(float& size);
};

#endif // INFOBAR_H

