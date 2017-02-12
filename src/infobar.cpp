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

    const char* getName(const char* path)
    {
        if (path != nullptr)
        {
            const char* n = ::strrchr(path, '/');
            return n != nullptr ? n + 1 : path;
        }

        return "n/a";
    }

}

cInfoBar::cInfoBar(const sConfig& config)
    : m_config(config)
{
}

void cInfoBar::init()
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor({ 0, 0, 25, 240 });
}

void cInfoBar::setRatio(float scale)
{
    scale = 1.0f;
    // if (m_scale != scale)
    {
        m_scale = scale;

        const int DesiredFontSize = 30;
        createFont(DesiredFontSize * scale);
    }

    const float DesiredHeight = 36;
    m_height = DesiredHeight * scale;
}

void cInfoBar::createFont(int fontSize)
{
    m_ft.reset(new cFTString(fontSize));
    m_ft->setColor({ 255, 255, 127, 255 });

    if (m_config.debug)
    {
        m_fps.reset(new cFTString(fontSize));
        m_fps->setColor({ 0, 0, 0, 255 });
    }
}

void cInfoBar::render()
{
    int width;
    int height;
    glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

    Vectorf pos { 0.0f, height - m_height };
    m_bg->setSpriteSize({ (float)width, m_height });
    m_bg->render(pos);

    pos += Vectorf{ 3, (m_height - m_bounds.y) * 0.5f - 2.0f };
    m_ft->draw(pos, m_bottominfo.c_str());

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
        m_fps->draw({ 20.0f, 20.0f }, buffer);
    }
}

void cInfoBar::setInfo(const sInfo& p)
{
    const char* name = getName(p.path);

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
    m_bounds = m_ft->getBounds(title);

    glfwSetWindowTitle(cRenderer::getWindow(), name);
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
