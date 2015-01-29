/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef SELECTION_H
#define SELECTION_H

#include "math/rect.h"
#include "quad.h"

#include <memory>

const int m_selectionTexSize = 16;

class CSelection
{
public:
    CSelection();
    virtual ~CSelection();

    void Init();
    void SetImageDimension(float w, float h);
    void MouseButton(int x, int y, bool pressed);
    void MouseMove(int x, int y);
    void Render(float dx, float dy);
    const CRect<int>& GetRect() const;
    int GetCursor() const;

private:
    void updateCorner(int x, int y);
    void renderLine(int x1, int y1, int x2, int y2);
    void setImagePos(CRect<int>& rc, int dx, int dy);
    void clampPoint(int& x, int& y);
    void setColor(bool selected);
    float getTime();
    void clampShiftDelta(int& dx, int& dy);

private:
    bool m_enabled;
    float m_imageWidth, m_imageHeight;
    float m_mouseX, m_mouseY;
    float m_timeDelta;
    enum eMouseMode
    {
        MODE_NONE,
        MODE_SELECT,
        MODE_MOVE,
        MODE_RESIZE
    };
    eMouseMode m_mode;
    enum eCorner
    {
        CORNER_NONE = 0,
        CORNER_UP   = 1 << 0,
        CORNER_RT   = 1 << 1,
        CORNER_DN   = 1 << 2,
        CORNER_LT   = 1 << 3,
        CORNER_CR   = 1 << 4,
    };
    unsigned m_corner;

    std::auto_ptr<CQuad> m_selection;
    CRect<int> m_rc;
    CRect<int> m_rc_test;
};

#endif // SELECTION_H

