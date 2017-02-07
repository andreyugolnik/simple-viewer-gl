/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "quad.h"
#include "ftsymbol.h"
#include "types/color.h"

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class cFTString final
{
public:
    explicit cFTString(int size);
    ~cFTString();

    void setColor(const cColor& color);
    Vectorf getBounds(const char* utf8) const;

    void draw(const Vectorf& pos, const char* utf8);

private:
    void generateNewSymbol(const char* utf8) const;
    void generate() const;
    bool placeSymbols() const;
    void clearSymbols() const;

private:
    int m_height;   // desired font size
    FT_Library m_ft;

    cColor m_color;

    struct Symbol
    {
        cFTSymbol* p = nullptr;

        uint8_t* bmp = nullptr; // bitmap data
        uint32_t w = 0;
        uint32_t h = 0;
        uint32_t pitch = 0;

        Vectorf offset{ 0.0f, 0.0f };
        float ax = 0.0f;

        uint32_t x = 0;
        uint32_t y = 0; // texture symbol position
    };

    mutable GLuint m_texId;
    mutable uint32_t m_texWidth;
    mutable uint32_t m_texHeight;
    mutable std::vector<uint16_t> m_symbols; // all symbols placed on texture
    mutable std::map<uint16_t, Symbol> m_mapSymbol;
};
