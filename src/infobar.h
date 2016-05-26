/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

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

class CInfoBar final
{
public:
    void Init(GLFWwindow* window);

    void Render();
    bool Visible() const { return m_visible; }
    void Show(bool show = true) { m_visible = show; }
    void Update(const sInfoBar& p);
    float GetHeight() const { return (m_visible == true ? m_height : 0); }

private:
    const char* getHumanSize(float& size);

private:
    GLFWwindow* m_window = nullptr;
    bool m_visible = true;
    const float m_height = 18;
    const int m_fntSize = 12;
    std::string m_bottominfo;
    std::unique_ptr<CQuad> m_bg;
    std::unique_ptr<CFTString> m_ft;
};

