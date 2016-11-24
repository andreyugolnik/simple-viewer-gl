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

class CInfoBar final
{
public:
    void init();

    void setRatio(float ratio);
    float getHeight() const;

    struct sInfo
    {
        const char* path = nullptr;
        const char* type = nullptr;
        unsigned index = 0;
        unsigned width = 0;
        unsigned height = 0;
        unsigned bpp = 0;
        float scale = 0.0f;
        unsigned images = 0;
        unsigned current = 0;
        long file_size = 0;
        size_t mem_size = 0;
        unsigned files_count = 0;
    };

    void setInfo(const sInfo& p);
    void render();

private:
    const char* getHumanSize(float& size);
    void createFont();

private:
    float m_ratio = 1.0f;
    std::string m_bottominfo;
    std::unique_ptr<CQuad> m_bg;
    std::unique_ptr<CFTString> m_ft;
    std::unique_ptr<CFTString> m_fps;
};
