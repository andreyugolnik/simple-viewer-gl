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
    void mouseButton(const Vectorf& pos, float scale, bool pressed);
    void mouseMove(const Vectorf& pos, float scale);
    void render(const Vectorf& offset);
    const Rectf& getRect() const;
    int getCursor() const;

private:
    void updateTestRect(float scale);
    void updateCorner(const Vectorf& pos, float scale);
    void renderHorizontal(const Vectorf& pos, float w, float thickness);
    void renderVertical(const Vectorf& pos, float h, float thickness);
#if 0
    void renderRect(const Vectorf& tl, const Vectorf& br, float thickness);
#endif
    void setImagePos(Rectf& rc, const Vectorf& offset);
    void setColor(bool selected);
    void clampShiftDelta(Vectorf& delta);

private:
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

    enum class Edge : uint32_t
    {
        None   = 0,
        Top    = 1 << 0,
        Right  = 1 << 1,
        Bottom = 1 << 2,
        Left   = 1 << 3,
        Center = 1 << 4,
    };
    uint32_t m_corner = (uint32_t)Edge::None;

    std::unique_ptr<cQuad> m_hori;
    std::unique_ptr<cQuad> m_vert;
    Rectf m_rc;
    Rectf m_rcTest;
};
