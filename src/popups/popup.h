/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../ftstring.h"

#include <memory>

class cPopup
{
public:
    virtual ~cPopup() = default;

    virtual void init() = 0;
    virtual void setScale(float scale);

    virtual void render() = 0;

protected:
    void createBackground(const cColor& color = { 0, 0, 0, 200 });
    virtual void createFont(int fontSize, const cColor& color = cColor::White);

protected:
    int m_fontSize = -1;

    float m_scale = 0.0f;
    std::unique_ptr<cQuad> m_bg;
    std::unique_ptr<cFTString> m_ft;
};
