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
    uint32_t getStringWidth(const char* utf8);
    void draw(int x, int y, const char* utf8);

private:
    void generateNewSymbol(const char* utf8);
    void generate();
    bool placeSymbols();
    void clearSymbols();

private:
    int m_height;   // desired font size
    FT_Library m_ft;
    std::vector<uint32_t> m_symbols; // all symbols placed on texture
    uint32_t m_texWidth;
    uint32_t m_texHeight;
    sQuad m_quad;

    struct Symbol
    {
        cFTSymbol* p;
        uint8_t* bmp; // bitmap data
        uint32_t w, h, pitch; // bitmap width, height, pitch
        int l, t, ax;
        uint32_t px, py; // texture symbol position
    };

    std::map<wchar_t, Symbol> m_mapSymbol;
};
