/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FTSTRING_H
#define FTSTRING_H

#include "quad.h"
#include "ftsymbol.h"

#include <string>
#include <memory>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class CFTString
{
public:
    CFTString(int size);
    CFTString(const char* ttf, int size);
    virtual ~CFTString();

    void SetColor(int r, int g, int b, int a);
    void Update(const char* utf8);
    void Render(int x, int y);
    int GetStringWidth();

private:
    std::string m_ttf;
    int m_height;	// desired font size
    FT_Library m_ft;
    wchar_t* m_symbols;	// all symbols placed on texture
    wchar_t* m_unicode;
    int m_unicodeSize;
    int m_texW, m_texH;
    sQuad m_quad;

    typedef struct SYMBOL
    {
        CFTSymbol* p;
        unsigned char* bmp; // bitmap data
        int w, h, pitch;    // bitmap width, height, pitch
        int l, t, ax;
        int px, py;         // texture symbol position
    } Symbol;

    typedef std::map<wchar_t, Symbol> Symbols;
    typedef Symbols::iterator SymbolsIt;
    typedef Symbols::const_iterator SymbolsIc;
    Symbols m_mapSymbol;

private:
    void generateNewSymbol(const wchar_t* string);
    void generate();
    bool placeSymbols();
    void clearSymbols();
};

#endif // FTSTRING_H

