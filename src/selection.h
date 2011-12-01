////////////////////////////////////////////////
//
// Simple Viewer GL edition
// Andrey A. Ugolnik
// http://www.wegroup.org
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#ifndef SELECTION_H
#define SELECTION_H

#include "rect.h"
#include "quad.h"

#include <memory>

const int m_selectionTexSize = 16;
const int m_selectionTexCount = 4;

class CSelection
{
public:
    CSelection();
    virtual ~CSelection();

    void Init();
    void SetImageDimension(float _w, float _h);
    void MouseButton(int x, int y, bool pressed);
    void MouseMove(int x, int y);
    void Render(float _dx, float _dy, float _scale);
    CRect<float> GetRect() const;
    int GetCursor() const;

private:
    bool m_enabled;
    float m_imageWidth, m_imageHeight;
    float m_mouseX, m_mouseY;
    struct timespec m_timeLast;
    float m_timeDelta;
    typedef enum { MODE_NONE, MODE_SELECT, MODE_MOVE, MODE_RESIZE } MouseMode;
    MouseMode m_mode;
    typedef enum { CORNER_NONE, CORNER_CENTER, CORNER_LEFT, CORNER_RIGHT, CORNER_UP, CORNER_DOWN, CORNER_LEUP, CORNER_RIUP, CORNER_LEDN, CORNER_RIDN } CornerType;
    CornerType m_corner;

    std::auto_ptr<CQuad> m_selection[m_selectionTexCount];
    CRect<float> m_rc;

private:
    void updateCorner(int x, int y);
    void renderLine(int x1, int y1, int x2, int y2, int frame);
    void setImagePos(CRect<int>& rc, int dx, int dy);
    void clampPoint(int& x, int& y);
    void setColor(int idx, bool std = true);
    float getTime();
};

#endif // SELECTION_H

