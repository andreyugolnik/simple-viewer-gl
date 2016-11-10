/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "infobar.h"
#include <string.h>
#include <math.h>

const float DesiredHeight = 18;
const int DesiredFontSize = 12;

void CInfoBar::init()
{
    m_bg.reset(new CQuad(0, 0));
    m_bg->SetColor(0, 0, 25, 240);

    createFont();
}

void CInfoBar::setRatio(float ratio)
{
    if(m_ratio != ratio)
    {
        m_ratio = ratio;
        createFont();
    }
}

void CInfoBar::createFont()
{
    m_ft.reset(new CFTString(DesiredFontSize * m_ratio));
    m_ft->SetColor(255, 255, 127, 255);
}

float CInfoBar::getHeight() const
{
    return m_ratio * DesiredHeight;
}

void CInfoBar::Render()
{
    int width;
    int height;
    glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

    const float x = 0.0f;
    const float y = height;
    m_bg->SetSpriteSize(width, DesiredHeight * m_ratio);
    m_bg->Render(x, y - DesiredHeight * m_ratio);

    m_ft->Render(x, y - (DesiredHeight - DesiredFontSize) * m_ratio);
}

static const char* GetName(const char* path)
{
    if (path != nullptr)
    {
        const char* n = strrchr(path, '/');
        return n != nullptr ? n + 1 : path;
    }

    return "n/a";
}

void CInfoBar::Update(const sInfoBar& p)
{
    const char* name = GetName(p.path);

    char idx_img[20] = { 0 };
    if(p.files_count > 1)
    {
        snprintf(idx_img, sizeof(idx_img), "%d out %d | ", p.index + 1, p.files_count);
    }

    char sub_image[20] = { 0 };
    if(p.sub_count > 1)
    {
        snprintf(sub_image, sizeof(sub_image), " | %d / %d", p.sub_image + 1, p.sub_count);
    }

    float file_size = p.file_size;
    std::string file_s = getHumanSize(file_size);
    float mem_size = p.mem_size;
    std::string mem_s = getHumanSize(mem_size);

    char title[1000] = { 0 };
    snprintf(title, sizeof(title)
            , "%s%s%s | %s | %d x %d x %d bpp (%d%%) | mem: %.1f %s (%.1f %s)"
            , idx_img
            , name
            , sub_image
            , p.type
            , p.width, p.height, p.bpp, (int)(100.0f * p.scale)
            , file_size, file_s.c_str()
            , mem_size, mem_s.c_str());

    m_bottominfo = title;

    glfwSetWindowTitle(cRenderer::getWindow(), name);

    m_ft->Update(m_bottominfo.c_str());
}

const char* CInfoBar::getHumanSize(float& size)
{
    static const char* s[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB" };
    int idx = 0;
    for( ; size > 1024.0f; size /= 1024.0f)
    {
        idx++;
    }

    return s[idx];
}

