/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FTSYMBOL_H
#define FTSYMBOL_H

#include "renderer.h"

class CFTSymbol
{
public:
    CFTSymbol(sQuad& _quad, int tw, int th, float tx, float ty, int w, int h);
    virtual ~CFTSymbol();

    void Render(int x, int y);

private:
    int m_w;
    int m_h;
    sQuad m_quad;
};

#endif // FTSYMBOL_H

