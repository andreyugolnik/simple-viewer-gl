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
    void setImageDimension(int w, int h);
    void mouseButton(const Vectorf& pos, bool pressed);
    void mouseMove(const Vectorf& pos);
    void render(const Vectorf& offset);
    const Recti& getRect() const;
    int getCursor() const;

private:
    void updateTestRect();
    void updateCorner(const Vectori& pos);
    void renderHorizontal(float x, float y, float w, float thickness);
    void renderVertical(float x, float y, float h, float thickness);
    void setImagePos(Recti& rc, const Vectori& offset);
    void setColor(bool selected);
    void clampShiftDelta(Vectori& delta);

private:
    bool m_enabled = true;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
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
    Recti m_rc;
    Recti m_rcTest;
};
