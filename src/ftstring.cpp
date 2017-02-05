/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ftstring.h"
#include "common/unicode.h"
#include "DroidSans.hpp"

#include <vector>

namespace
{
    uint32_t LinefeedCodepoint = (uint32_t)'\n';
}

cFTString::cFTString(int size)
    : m_height(size)
    , m_ft(nullptr)
    , m_texWidth(256)
    , m_texHeight(256)
{
    if (FT_Init_FreeType(&m_ft) != 0)
    {
        ::printf("(EE) Error initiation FreeType2.\n");
    }

    setColor({ 255, 255, 255, 255 });
}

cFTString::~cFTString()
{
    clearSymbols();
    cRenderer::deleteTexture(m_quad.tex);
    if (m_ft != nullptr)
    {
        FT_Done_FreeType(m_ft);
    }
}

void cFTString::setColor(const cColor& color)
{
    cRenderer::setColor(&m_quad, color);
}

void cFTString::draw(int x, int y, const char* utf8)
{
    auto xStart = x;

    uint32_t codepoint;
    auto s = (const uint8_t*)utf8;
    auto begin = utf8;

    for (uint32_t prev = 0, state = 0; *s; prev = state)
    {
        const uint32_t result = decode(&state, &codepoint, *s);
        if (result == UTF8_ACCEPT)
        {
            if (codepoint == LinefeedCodepoint)
            {
                x = xStart;
                y += m_height;
            }
            else
            {
                const auto& it = m_mapSymbol.find(codepoint);
                if (it == m_mapSymbol.end())
                {
                    generateNewSymbol(begin);
                    continue;
                }
                if (it->second.p != nullptr)
                {
                    it->second.p->render(x + it->second.l, y - it->second.t);
                    x += it->second.ax;
                }
            }
            begin = (const char*)s + 1;
        }
        else if (result == UTF8_REJECT)
        {
            state = UTF8_ACCEPT;
            if (prev != UTF8_ACCEPT)
            {
                continue;
            }
        }

        s++;
    }
}

uint32_t cFTString::getStringWidth(const char* utf8)
{
    uint32_t width = 0;

    uint32_t codepoint;
    auto s = (const uint8_t*)utf8;
    auto begin = utf8;

    for (uint32_t prev = 0, state = 0; *s; prev = state)
    {
        const uint32_t result = decode(&state, &codepoint, *s);
        if (result == UTF8_ACCEPT)
        {
            if (codepoint == LinefeedCodepoint)
            {
                break;
            }

            const auto& it = m_mapSymbol.find(codepoint);
            if (it == m_mapSymbol.end())
            {
                generateNewSymbol(begin);
                continue;
            }
            if (it->second.p != nullptr)
            {
                width += it->second.ax;
            }
            begin = (const char*)s + 1;
        }
        else if (result == UTF8_REJECT)
        {
            state = UTF8_ACCEPT;
            if (prev != UTF8_ACCEPT)
            {
                continue;
            }
        }

        s++;
    }

    return width;
}

void cFTString::generateNewSymbol(const char* utf8)
{
    if (m_ft != nullptr)
    {
        m_symbols.reserve(m_symbols.size() + ::strlen(utf8));

        uint32_t codepoint;
        auto s = (const uint8_t*)utf8;

        for (uint32_t prev = 0, state = 0; *s; prev = state)
        {
            const uint32_t result = decode(&state, &codepoint, *s);
            if (result == UTF8_ACCEPT)
            {
                bool dup = false;
                for (size_t i = 0, size = m_symbols.size(); i < size; i++)
                {
                    if (m_symbols[i] == codepoint)
                    {
                        dup = true;
                        break;
                    }
                }

                if (dup == false)
                {
                    m_symbols.push_back(codepoint);
                }
            }
            else if (result == UTF8_REJECT)
            {
                state = UTF8_ACCEPT;
                if (prev != UTF8_ACCEPT)
                {
                    continue;
                }
            }

            s++;
        }

        generate();
    }
}

void cFTString::generate()
{
    FT_Face face;

    FT_Error err = FT_New_Memory_Face(m_ft, droidsans_ttf, droidsans_ttf_size, 0, &face);
    if (err != 0)
    {
        ::printf("(EE) Error creating face.\n");
        return;
    }

    if (FT_Set_Pixel_Sizes(face, 0, m_height) != 0)
    {
        ::printf("(EE) Error set font size.\n");
        FT_Done_Face(face);
        return;
    }

    clearSymbols();

    FT_GlyphSlot slot = face->glyph;
    for (const auto& charcode : m_symbols)
    {
        err = FT_Load_Char(face, (FT_ULong)charcode, FT_LOAD_RENDER);
        Symbol str;
        ::memset(&str, 0, sizeof(str));
        if (err == 0)
        {
            FT_Bitmap bmp = slot->bitmap;
            const uint32_t size = bmp.pitch * bmp.rows;

            //str.p = 0;
            str.bmp = new uint8_t[size];
            str.w = bmp.width;
            str.h = bmp.rows;
            str.pitch = bmp.pitch;
            str.l = slot->bitmap_left;
            str.t = slot->bitmap_top;
            str.ax = slot->advance.x >> 6;
            //str.px    = 0;
            //str.py    = 0;
            ::memcpy(str.bmp, bmp.buffer, size);
        }
        m_mapSymbol[charcode] = str;
    }

    while (placeSymbols() == false)
    {
        if (m_texWidth <= m_texHeight)
        {
            m_texWidth <<= 1;
        }
        else
        {
            m_texHeight <<= 1;
        }
    }
    // ::printf("(II) Font texture size: %u x %u\n", m_texWidth, m_texHeight);

    std::vector<uint8_t> buffer(m_texWidth * m_texHeight);
    ::memset(buffer.data(), 0, buffer.size());

    // regenerate texture
    for (auto& it : m_mapSymbol)
    {
        const auto dx = it.second.px;
        const auto dy = it.second.py;
        const auto w = it.second.w;
        const auto h = it.second.h;
        const auto pitch = it.second.pitch;
        const auto in = it.second.bmp;
        if (in != nullptr)
        {
            for (uint32_t y = 0; y < h; y++)
            {
                size_t pos = dx + m_texWidth + (dy + y) * m_texWidth;
                for (uint32_t x = 0; x < w; x++)
                {
                    uint8_t pixel = in[x + y * pitch];
                    buffer[pos++] = pixel;
                }
            }
        }
    }

    if (m_quad.tex == 0)
    {
        m_quad.tex = cRenderer::createTexture();
    }
    cRenderer::setData(m_quad.tex, buffer.data(), m_texWidth, m_texHeight, GL_ALPHA);

    for (auto& it : m_mapSymbol)
    {
        if (it.second.bmp != nullptr)
        {
            it.second.p = new cFTSymbol(m_quad, m_texWidth, m_texHeight, it.second.px, it.second.py, it.second.w, it.second.h + 1);
            delete[] it.second.bmp;
            it.second.bmp = nullptr;
        }
    }

    FT_Done_Face(face);

    // ::printf("(II) Texture (%u x %u) with %u / %u symbols has been created.\n", m_texWidth, m_texHeight, (uint32_t)m_mapSymbol.size(), (uint32_t)len);
}

bool cFTString::placeSymbols()
{
    uint32_t maxRowHeight = 0;

    uint32_t x = 0;
    uint32_t y = 0;
    for (auto& it : m_mapSymbol)
    {
        maxRowHeight = std::max<uint32_t>(maxRowHeight, it.second.h);

        if (y + maxRowHeight > m_texHeight)
        {
            return false;
        }

        if (x + it.second.w > m_texWidth)
        {
            x = 0;
            y += maxRowHeight + 1;
            if (y + maxRowHeight > m_texHeight)
            {
                return false;
            }
        }

        it.second.px = x;
        it.second.py = y;
        x += it.second.w + 1;
    }

    return true;
}

void cFTString::clearSymbols()
{
    for (auto& it : m_mapSymbol)
    {
        delete it.second.p;
    }
    m_mapSymbol.clear();
}
