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
    , m_texId(0)
    , m_texWidth(256)
    , m_texHeight(256)
{
    if (FT_Init_FreeType(&m_ft) != 0)
    {
        ::printf("(EE) Error initiation FreeType2.\n");
    }

    setColor(cColor::White);
}

cFTString::~cFTString()
{
    clearSymbols();
    cRenderer::deleteTexture(m_texId);
    if (m_ft != nullptr)
    {
        FT_Done_FreeType(m_ft);
    }
}

void cFTString::setColor(const cColor& color)
{
    m_color = color;
}

void cFTString::draw(const Vectorf& pos, const char* utf8, float scale)
{
    auto p = pos;

    uint32_t codepoint = 0;
    auto s = (const uint8_t*)utf8;
    auto begin = utf8;

    for (uint32_t prev = 0, state = 0; *s; prev = state)
    {
        const uint32_t result = decode(&state, &codepoint, *s);
        if (result == UTF8_ACCEPT)
        {
            if (codepoint == LinefeedCodepoint)
            {
                p.x = pos.x * scale;
                p.y += m_height * scale;
            }
            else
            {
                const auto& it = m_mapSymbol.find(codepoint);
                if (it == m_mapSymbol.end())
                {
                    generateNewSymbol(begin);
                    continue;
                }
                const auto& symbol = it->second;
                if (symbol.p != nullptr)
                {
                    symbol.p->render(p + symbol.offset * scale, m_color, scale);
                    p.x += symbol.ax * scale;
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

Vectorf cFTString::getBounds(const char* utf8) const
{
    Vectorf bounds{ 0.0f, 0.0f };

    uint32_t codepoint = 0;
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

            const auto& symbol = it->second;
            if (symbol.p != nullptr)
            {
                bounds.x += symbol.ax;
                bounds.y = std::max<float>(bounds.y, symbol.offset.y + symbol.h);
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

    return bounds;
}

void cFTString::generateNewSymbol(const char* utf8) const
{
    if (m_ft != nullptr)
    {
        m_symbols.reserve(m_symbols.size() + ::strlen(utf8));

        uint32_t codepoint = 0;
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

void cFTString::generate() const
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
        Symbol str;
        if (FT_Load_Char(face, (FT_ULong)charcode, FT_LOAD_RENDER) == 0)
        {
            FT_Bitmap bmp = slot->bitmap;
            const uint32_t size = bmp.pitch * bmp.rows;

            str.bmp = new uint8_t[size];
            str.w = bmp.width;
            str.h = bmp.rows;
            str.pitch = bmp.pitch;
            str.offset = { (float)slot->bitmap_left, m_height - (float)slot->bitmap_top };
            str.ax = slot->advance.x / 64.0f;
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
    for (const auto& it : m_mapSymbol)
    {
        auto& symbol = it.second;
        const auto in = symbol.bmp;
        if (in != nullptr)
        {
            const auto x = symbol.x;
            const auto y = symbol.y;
            const auto w = symbol.w;
            const auto h = symbol.h;
            const auto p = symbol.pitch;
            const auto pitch = m_texWidth;
            for(uint32_t dy = 0; dy < h; dy++)
            {
                auto pos = &buffer[x + (y + dy) * pitch];
                for(uint32_t dx = 0; dx < w; dx++)
                {
                    *pos++ = in[dx + dy * p];
                }
            }
        }
    }

    if (m_texId == 0)
    {
        m_texId = cRenderer::createTexture();
    }
    cRenderer::setData(m_texId, buffer.data(), m_texWidth, m_texHeight, GL_ALPHA);

    for (auto& it : m_mapSymbol)
    {
        auto& symbol = it.second;
        if (symbol.bmp != nullptr)
        {
            symbol.p = new cFTSymbol(m_texId, m_texWidth, m_texHeight, symbol.x, symbol.y, symbol.w, symbol.h);
            delete[] symbol.bmp;
            symbol.bmp = nullptr;
        }
    }

    FT_Done_Face(face);

    // ::printf("(II) Texture (%u x %u) with %u / %u symbols has been created.\n", m_texWidth, m_texHeight, (uint32_t)m_mapSymbol.size(), (uint32_t)len);
}

bool cFTString::placeSymbols() const
{
    uint32_t maxRowHeight = 0;

    uint32_t x = 0;
    uint32_t y = 0;
    for (auto& it : m_mapSymbol)
    {
        auto& symbol = it.second;

        maxRowHeight = std::max<uint32_t>(maxRowHeight, symbol.h);

        if (y + maxRowHeight > m_texHeight)
        {
            return false;
        }

        if (x + symbol.w > m_texWidth)
        {
            x = 0;
            y += maxRowHeight + 1;
            maxRowHeight = symbol.h;
            if (y + maxRowHeight > m_texHeight)
            {
                return false;
            }
        }

        symbol.x = x;
        symbol.y = y;
        x += symbol.w + 1;
    }

    return true;
}

void cFTString::clearSymbols() const
{
    for (auto& it : m_mapSymbol)
    {
        delete it.second.p;
    }
    m_mapSymbol.clear();
}
