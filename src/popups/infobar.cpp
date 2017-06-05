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
#include "common/unicode.h"
#include "imgui/imgui.h"
#include "renderer.h"

#include <cmath>
#include <cstring>

cInfoBar::cInfoBar(const sConfig& config)
    : m_config(config)
{
}

void cInfoBar::render()
{
    int width;
    int height;
    glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

    // Vectorf pos{ 0.0f, height - m_height * scale };
    // m_bg->setSpriteSize({ (float)width, m_height * scale });
    // m_bg->render(pos);

    // pos += Vectorf{ 3, (m_height - m_bounds.y) * 0.5f - 2.0f } * scale;
    // m_ft->draw(pos, m_bottominfo.c_str(), scale);

    // if (m_fps.get() != nullptr)
    // {
        // static unsigned frame = 0;
        // static float fps = 0.0f;

        // frame++;
        // static auto last = glfwGetTime();
        // const auto now = glfwGetTime();
        // const auto delta = now - last;
        // if (delta > 0.5f)
        // {
            // fps = frame / delta;
            // last = now;
            // frame = 0;
        // }

        // char buffer[20];
        // ::snprintf(buffer, sizeof(buffer), "%.1f", fps);
        // m_fps->draw({ 20.0f, 20.0f }, buffer);
    // }
}

void cInfoBar::setInfo(const sInfo& p)
{
    const auto& shortName = shortenFilename(p.path);
    const char* name = shortName.empty() == false ? shortName.c_str() : "n/a";

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
    ::snprintf(title, sizeof(title), "%s%s%s | %s | %u x %u x %u bpp (%.1f%%) | %.1f %s (%.1f %s)", idx_img, name, sub_image, p.type, p.width, p.height, p.bpp, p.scale * 100.0f, file_size, file_s.c_str(), mem_size, mem_s.c_str());

    m_bottominfo = title;

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

const std::string& cInfoBar::shortenFilename(const char* path)
{
    m_filename.clear();

    if (path != nullptr)
    {
        const char* n = ::strrchr(path, '/');
        if (n != nullptr)
        {
            path = n + 1;
        }

        m_filename = path;

        // ::printf("'%s' -> ", path);

        const uint8_t* s = (uint8_t*)path;
        const uint32_t count = countCodePoints(s);

        const uint32_t maxCount = m_config.fileMaxLength;
        if (count > maxCount)
        {
            uint32_t state = 0;
            uint32_t codepoint;

            m_filename.clear();
            for (uint32_t left = maxCount / 2; *s && left; s++)
            {
                m_filename += *s;
                if (!decode(&state, &codepoint, *s))
                {
                    left--;
                }
            }

            const char* delim = "~";

            m_filename += delim;

            for (uint32_t skip = count - (maxCount - ::strlen(delim)); *s && skip; s++)
            {
                if (!decode(&state, &codepoint, *s))
                {
                    skip--;
                }
            }

            m_filename += (const char*)s;
            // ::printf("'%s'\n", m_filename.c_str());
        }
    }

    return m_filename;
}
