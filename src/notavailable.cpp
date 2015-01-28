/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "notavailable.h"
#include "img-na.c"
#include <math.h>

CNotAvailable::CNotAvailable()
    : m_enabled(false)
{
}

CNotAvailable::~CNotAvailable()
{
}

void CNotAvailable::Init()
{
    const int format = (imgNa.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
    m_na.reset(new CQuad(imgNa.width, imgNa.height, imgNa.pixel_data, format));
}

bool CNotAvailable::Render()
{
    if(m_enabled == true)
    {
        cRenderer::resetGlobals();
        m_na->Render(-imgNa.width * 0.5f, -imgNa.height * 0.5f);
    }

    return m_enabled;
}

