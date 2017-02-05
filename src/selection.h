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
    void init();
    void setImageDimension(float w, float h);
    void mouseButton(float x, float y, bool pressed);
    void mouseMove(float x, float y);
    void render(float dx, float dy);
    const Rectf& getRect() const;
    int getCursor() const;
    void setScale(float scale);

private:
    void updateCorner(float x, float y);
    void renderHorizontal(float x, float y, float w, float scale);
    void renderVertical(float x, float y, float h, float scale);
    void setImagePos(Rectf& rc, float dx, float dy);
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
    Rectf m_rc;
    Rectf m_rc_test;
};
