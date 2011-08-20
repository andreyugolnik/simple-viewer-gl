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
    CFTSymbol(sQuad& _quad, int _tw, int _th, float _tx, float _ty, int _w, int _h);
    virtual ~CFTSymbol();

    void Render(int _x, int _y);

private:
    int m_w;
    int m_h;
    sQuad m_quad;
};

#endif // FTSYMBOL_H

