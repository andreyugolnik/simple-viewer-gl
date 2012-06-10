/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

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
    int format = (imgNa.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
    m_na.reset(new CQuad(imgNa.width, imgNa.height, imgNa.pixel_data, format));
}

bool CNotAvailable::Render()
{
    if(m_enabled == true)
    {
        //float w = (float)glutGet(GLUT_WINDOW_WIDTH);
        //float h = (float)glutGet(GLUT_WINDOW_HEIGHT);
        //float x = ceil((w - imgNa.width) / 2);
        //float y = ceil((h - imgNa.height) / 2);

        cRenderer::setGlobals();
        m_na->Render(-imgNa.width / 2, -imgNa.height / 2);
    }

    return m_enabled;
}

