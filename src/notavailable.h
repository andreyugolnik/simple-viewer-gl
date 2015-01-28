/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef NOTAVAILABLE_H
#define NOTAVAILABLE_H

#include "quad.h"
#include <memory>

class CNotAvailable
{
public:
    CNotAvailable();
    virtual ~CNotAvailable();

    void Init();
    bool Render();
    void Enable(bool enable) { m_enabled = enable; }

private:
    bool m_enabled;
    std::auto_ptr<CQuad> m_na;
};

#endif // NOTAVAILABLE_H

