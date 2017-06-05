/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "popup.h"

void cPopup::setScale(float scale)
{
    m_scale = scale;
}

void cPopup::createBackground(const cColor& color)
{
    m_bg.reset(new cQuad(0, 0));
    m_bg->setColor(color);
}

void cPopup::createFont(int fontSize, const cColor& color)
{
    if (m_fontSize != fontSize)
    {
        m_fontSize = fontSize;

        m_ft.reset(new cFTString(fontSize));
        m_ft->setColor(color);
    }
}
