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
#include "types/vector.h"

#include <memory>

class cQuad;

class cSelection final
{
public:
    void init();
    void setImageDimension(float w, float h);
    void mouseButton(const Vectorf& pos, bool pressed);
    void mouseMove(const Vectorf& pos);
    void render(const Vectorf& offset);
    const Rectf& getRect() const;
    int getCursor() const;

private:
    void updateTestRect();
    void updateCorner(const Vectorf& pos);
    void renderHorizontal(float x, float y, float w, float scale);
    void renderVertical(float x, float y, float h, float scale);
    void setImagePos(Rectf& rc, const Vectorf& offset);
    void clampPoint(Vectorf& pos);
    void setColor(bool selected);
    void clampShiftDelta(Vectorf& delta);

private:
    bool m_enabled = true;
    float m_imageWidth = 0.0f;
    float m_imageHeight = 0.0f;
    Vectorf m_mousePos = { 0.0f, 0.0f };

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
    Rectf m_rcTest;
};
