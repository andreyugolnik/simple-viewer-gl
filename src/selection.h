/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "math/rect.h"

#include <memory>

class cQuad;

class cSelection final
{
public:
    void Init();
    void SetImageDimension(float w, float h);
    void MouseButton(float x, float y, bool pressed);
    void MouseMove(float x, float y);
    void Render(float dx, float dy);
    const CRect<float>& GetRect() const;
    int GetCursor() const;
    void setScale(float scale);

private:
    void updateCorner(float x, float y);
    void renderHorizontal(float x, float y, float w, float scale);
    void renderVertical(float x, float y, float h, float scale);
    void setImagePos(CRect<float>& rc, float dx, float dy);
    void clampPoint(float& x, float& y);
    void setColor(bool selected);
    void clampShiftDelta(float& dx, float& dy);

private:
    bool m_enabled = true;
    float m_imageWidth = 0.0f;
    float m_imageHeight = 0.0f;
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    float m_scale = 1.0f;

    enum eMouseMode
    {
        MODE_NONE,
        MODE_SELECT,
        MODE_MOVE,
        MODE_RESIZE
    };
    eMouseMode m_mode = eMouseMode::MODE_NONE;

    enum eCorner
    {
        CORNER_NONE = 0,
        CORNER_UP   = 1 << 0,
        CORNER_RT   = 1 << 1,
        CORNER_DN   = 1 << 2,
        CORNER_LT   = 1 << 3,
        CORNER_CR   = 1 << 4,
    };
    unsigned m_corner = eCorner::CORNER_NONE;

    std::unique_ptr<cQuad> m_selection;
    CRect<float> m_rc;
    CRect<float> m_rc_test;
};
