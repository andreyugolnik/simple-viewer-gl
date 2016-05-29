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
    void init();

    void setRatio(float ratio);
    float getHeight() const;

    void Update(const sInfoBar& p);
    void Render();

private:
    const char* getHumanSize(float& size);
    void createFont();

private:
    float m_ratio = 1.0f;
    std::string m_bottominfo;
    std::unique_ptr<CQuad> m_bg;
    std::unique_ptr<CFTString> m_ft;
};

