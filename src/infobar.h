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

#include <string>
#include <memory>

class cQuad;
struct sConfig;

class cInfoBar final
{
public:
    explicit cInfoBar(const sConfig& config);

    void init();

    void setScale(float scale);
    float getHeight() const
    {
        return m_height;
    }

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
    void createFont(int fontSize);

private:
    const sConfig& m_config;

    float m_scale = 0.0f;
    std::string m_bottominfo;
    Vectorf m_bounds;

    float m_height = 0.0f;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cFTString> m_ft;
    std::unique_ptr<cFTString> m_fps;
};
