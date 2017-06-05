/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "popup.h"

class cHelpPopup final : public cPopup
{
public:
    void init() override;
    void render() override;
    void setScale(float scale) override;

    void show(bool show)
    {
        m_isVisible = show;
    }

    bool isVisible() const
    {
        return m_isVisible;
    }

private:
    void calculate();

private:
    bool m_isVisible = false;
    bool m_dirty = true;
    float m_descriptionOffset = 0.0f;
    Vectorf m_bgSize;
};
