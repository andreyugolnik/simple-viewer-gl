/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "infobar.h"
#include "common/config.h"
#include "quad.h"

#include <cstring>
#include <cmath>

namespace
{

    const float DesiredHeight = 18;
    const int DesiredFontSize = 12;

    const char* GetName(const char* path)
    {
        if (path != nullptr)
        {
            const char* n = ::strrchr(path, '/');
            return n != nullptr ? n + 1 : path;
        }

        return "n/a";
    }

}

cInfoBar::cInfoBar(const sConfig* config)
    : m_config(config)
{
}

void cInfoBar::init()
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->SetColor(0, 0, 25, 240);

    createFont();
}

void cInfoBar::setRatio(float ratio)
{
    if (m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void cInfoBar::createFont()
{
    m_ft.reset(new cFTString(DesiredFontSize * m_ratio));
    m_ft->SetColor(255, 255, 127, 255);

    if (m_config->debug)
    {
        m_fps.reset(new cFTString(DesiredFontSize * m_ratio));
        m_fps->SetColor(0, 0, 0, 255);
    }
}

float cInfoBar::getHeight() const
{
    return m_ratio * DesiredHeight;
}

void cInfoBar::render()
{
    int width;
    int height;
    glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

    const float x = 0.0f;
    const float y = height;
    m_bg->SetSpriteSize(width, DesiredHeight * m_ratio);
    m_bg->Render(x, y - DesiredHeight * m_ratio);

    m_ft->Render(x, y - (DesiredHeight - DesiredFontSize) * m_ratio);

    if (m_fps.get() != nullptr)
    {
        static unsigned frame = 0;
        static float fps = 0.0f;

        frame++;
        static auto last = glfwGetTime();
        const auto now = glfwGetTime();
        const auto delta = now - last;
        if (delta > 0.5f)
        {
            fps = frame / delta;
            last = now;
            frame = 0;
        }

        char buffer[20];
        ::snprintf(buffer, sizeof(buffer), "%.1f", fps);
        m_fps->Update(buffer);
        m_fps->Render(20, 20);
    }
}

void cInfoBar::setInfo(const sInfo& p)
{
    const char* name = GetName(p.path);

    char idx_img[20] = { 0 };
    if (p.files_count > 1)
    {
        ::snprintf(idx_img, sizeof(idx_img), "%u out %u | ", p.index + 1, p.files_count);
    }

    char sub_image[20] = { 0 };
    if (p.images > 1)
    {
        ::snprintf(sub_image, sizeof(sub_image), " | %u / %u", p.current + 1, p.images);
    }

    float file_size = p.file_size;
    std::string file_s = getHumanSize(file_size);
    float mem_size = p.mem_size;
    std::string mem_s = getHumanSize(mem_size);

    char title[1000] = { 0 };
    ::snprintf(title, sizeof(title)
             , "%s%s%s | %s | %u x %u x %u bpp (%.1f%%) | mem: %.1f %s (%.1f %s)"
             , idx_img
             , name
             , sub_image
             , p.type
             , p.width, p.height, p.bpp, p.scale * 100.0f
             , file_size, file_s.c_str()
             , mem_size, mem_s.c_str());

    m_bottominfo = title;

    glfwSetWindowTitle(cRenderer::getWindow(), name);

    m_ft->Update(m_bottominfo.c_str());
}

const char* cInfoBar::getHumanSize(float& size)
{
    static const char* s[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
    int idx = 0;
    for (; size > 1024.0f; size /= 1024.0f)
    {
        idx++;
    }

    return s[idx];
}
