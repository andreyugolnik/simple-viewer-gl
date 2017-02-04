/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/rect.h"
#include "types/types.h"

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

    enum class eMouseMode
    {
        None,
        Select,
        Move,
        Resize
    };
    eMouseMode m_mode = eMouseMode::None;

    enum class eCorner : uint32_t
    {
        None = 0,
        UP   = 1 << 0,
        RT   = 1 << 1,
        DN   = 1 << 2,
        LT   = 1 << 3,
        CR   = 1 << 4,
    };
    uint32_t m_corner = (uint32_t)eCorner::None;

    std::unique_ptr<cQuad> m_selection;
    CRect<float> m_rc;
    CRect<float> m_rc_test;
};
