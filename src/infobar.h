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
    const char* path = nullptr;
    const char* type = nullptr;
    unsigned index;
    unsigned width;
    unsigned height;
    unsigned bpp;
    float scale;
    unsigned images;
    unsigned current;
    long file_size;
    size_t mem_size;
    unsigned files_count;
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
