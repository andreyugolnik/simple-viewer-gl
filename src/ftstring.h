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

    void SetColor(int r, int g, int b, int a);
    void Update(const char* utf8);
    void Render(int x, int y);
    unsigned GetStringWidth();

private:
    void generateNewSymbol(const wchar_t* string);
    void generate();
    bool placeSymbols();
    void clearSymbols();

private:
    int m_height;   // desired font size
    FT_Library m_ft;
    std::vector<wchar_t> m_symbols; // all symbols placed on texture
    std::vector<wchar_t> m_unicode;
    unsigned m_texWidth;
    unsigned m_texHeight;
    sQuad m_quad;

    struct Symbol
    {
        cFTSymbol* p;
        unsigned char* bmp; // bitmap data
        unsigned w, h, pitch; // bitmap width, height, pitch
        int l, t, ax;
        unsigned px, py; // texture symbol position
    };

    std::map<wchar_t, Symbol> m_mapSymbol;
};
