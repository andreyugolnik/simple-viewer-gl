/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "selection.h"
#include "vector.h"

#include <algorithm>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

typedef struct timeval SystemTime;
static SystemTime m_timeLast;

CSelection::CSelection()
    : m_enabled(true)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_timeDelta(0)
    , m_mode(MODE_NONE)
    , m_corner(CORNER_NONE)
{
    ::gettimeofday(&m_timeLast, 0);
}

CSelection::~CSelection()
{
}

void CSelection::Init()
{
    unsigned char* buffer = new unsigned char[m_selectionTexSize * m_selectionTexSize * 3];
    unsigned char* p = buffer;
    bool checker_height_odd = true;
    for(int y = 0; y < m_selectionTexSize; y++)
    {
        if(y % 4 == 0)
        {
            checker_height_odd = !checker_height_odd;
        }

        bool checker_width_odd = checker_height_odd;
        for(int x = 0; x < m_selectionTexSize; x++)
        {
            if(x % 4 == 0)
            {
                checker_width_odd = !checker_width_odd;
            }

            const unsigned char color = (checker_width_odd == true ? 0x20 : 0xff);
            *p++ = color;
            *p++ = color;
            *p++ = color;
        }
    }

    m_selection.reset(new CQuad(m_selectionTexSize, m_selectionTexSize, buffer, GL_RGB));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    m_selection->useFilter(false);

    delete[] buffer;
}

void CSelection::SetImageDimension(float w, float h)
{
    m_imageWidth = w;
    m_imageHeight = h;
    m_rc.Clear();
    m_rc_test.Clear();
    getTime();
}

const int delta = 20;
const int delta2 = delta / 2;
void CSelection::MouseButton(int x, int y, bool pressed)
{
    if(pressed)
    {
        m_rc.Normalize();
        const bool inside = m_rc_test.TestPoint(x, y);

        m_mouseX = x;
        m_mouseY = y;

        if(m_mode == MODE_NONE)
        {
            if(inside)
            {
                m_mode = (m_corner == CORNER_CR ? MODE_MOVE : MODE_RESIZE);
            }
            else
            {
                m_mode = MODE_SELECT;
                clampPoint(x, y);
                m_rc.SetLeftTop(x, y);
                m_rc.Clear();
                m_rc_test.SetLeftTop(x - delta2, y - delta2);
                m_rc_test.Clear();
            }
        }
        else if(inside == false)
        {
            m_mode = MODE_NONE;
            m_rc.Clear();
        }
    }
    else
    {
        m_mode = MODE_NONE;

        m_rc.Normalize();
        m_rc_test.Set(m_rc.x1 - delta2, m_rc.y1 - delta2, m_rc.x2 + delta2, m_rc.y2 + delta2);
    }
}

void CSelection::MouseMove(int x, int y)
{
    updateCorner(x, y);

    if(m_mode != MODE_NONE)
    {
        int dx = x - m_mouseX;
        int dy = y - m_mouseY;

        switch(m_mode)
        {
        case MODE_NONE:   // do nothing here
            break;

        case MODE_SELECT:
            clampPoint(x, y);
            m_rc.SetRightBottom(x, y);
            break;

        case MODE_MOVE:
            clampShiftDelta(dx, dy);
            m_rc.ShiftRect(dx, dy);
            break;

        case MODE_RESIZE:
            //clampShiftDelta(dx, dy);
            if((m_corner & CORNER_UP))
            {
                m_rc.y1 += dy;
            }
            if((m_corner & CORNER_RT))
            {
                m_rc.x2 += dx;
            }
            if((m_corner & CORNER_DN))
            {
                m_rc.y2 += dy;
            }
            if((m_corner & CORNER_LT))
            {
                m_rc.x1 += dx;
            }
            break;
        }

        m_rc.x1 = m_rc.x1 >= 0 ? m_rc.x1 : 0;
        m_rc.y1 = m_rc.y1 >= 0 ? m_rc.y1 : 0;

        m_rc.x2 = m_rc.x2 < m_imageWidth ? m_rc.x2 : m_imageWidth - 1;
        m_rc.y2 = m_rc.y2 < m_imageHeight ? m_rc.y2 : m_imageHeight - 1;

        m_mouseX += dx;
        m_mouseY += dy;
    }
}

void CSelection::clampShiftDelta(int& dx, int& dy)
{
    if(m_rc.x1 + dx < 0)
    {
        dx = -m_rc.x1;
    }
    else if(m_rc.x2 + dx >= m_imageWidth)
    {
        dx = m_imageWidth - 1 - m_rc.x2;
    }
    if(m_rc.y1 + dy < 0)
    {
        dy = -m_rc.y1;
    }
    else if(m_rc.y2 + dy >= m_imageHeight)
    {
        dy = m_imageWidth - 1 - m_rc.y2;
    }
}

void CSelection::Render(float dx, float dy)
{
    if(m_enabled && m_rc.IsSet())
    {
        const float dt = getTime();

        m_timeDelta += dt * 10.0f;

        CRect<int> rc;
        setImagePos(rc, dx, dy);
        //setImagePos(rc, _delta.x * _scale, _delta.y * _scale);

        setColor(m_corner & CORNER_UP);
        renderLine(rc.x1, rc.y1, rc.x2, rc.y1); // top line
        setColor(m_corner & CORNER_DN);
        renderLine(rc.x1, rc.y2, rc.x2, rc.y2); // bottom line
        setColor(m_corner & CORNER_LT);
        renderLine(rc.x1, rc.y1, rc.x1, rc.y2); // left line
        setColor(m_corner & CORNER_RT);
        renderLine(rc.x2, rc.y1, rc.x2, rc.y2); // right line
    }
}

const CRect<int>& CSelection::GetRect() const
{
    return m_rc;
}

int CSelection::GetCursor() const
{
    static const int cursor[16] =
    {
        //                                 1  1  1  1  1
        //0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
          0, 3, 2, 5, 3, 0, 4, 0, 2, 4, 0, 0, 5, 0, 0, 0
    };
    return cursor[m_corner];
}

void CSelection::updateCorner(int x, int y)
{
    if(m_mode != MODE_NONE)
    {
        return;
    }

    const CRect<int>& rc = m_rc_test;
    if(rc.TestPoint(x, y))
    {
        m_corner = 0;
        CRect<int> rcLt(rc.x1, rc.y1, rc.x1 + delta, rc.y2);
        if(rcLt.TestPoint(x, y))
        {
            m_corner |= CORNER_LT;
        }

        CRect<int> rcRt(rc.x2 - delta, rc.y1, rc.x2, rc.y2);
        if(rcRt.TestPoint(x, y))
        {
            m_corner |= CORNER_RT;
        }

        CRect<int> rcUp(rc.x1, rc.y1, rc.x2, rc.y1 + delta);
        if(rcUp.TestPoint(x, y))
        {
            m_corner |= CORNER_UP;
        }

        CRect<int> rcDn(rc.x1, rc.y2 - delta, rc.x2, rc.y2);
        if(rcDn.TestPoint(x, y))
        {
            m_corner |= CORNER_DN;
        }

        if(!m_corner)
        {
            m_corner = CORNER_CR;
        }
    }
    else
    {
        m_corner = CORNER_NONE;
    }
}

void CSelection::renderLine(int x1, int y1, int x2, int y2)
{
    const int x = std::min<int>(x1, x2);
    const int y = std::min<int>(y1, y2);
    const int w = (x1 == x2 ? 1 : m_rc.GetWidth());
    const int h = (y1 == y2 ? 1 : m_rc.GetHeight());

    m_selection->SetSpriteSize(w, h);
    m_selection->Render(x, y);
}

void CSelection::setImagePos(CRect<int>& rc, int dx, int dy)
{
    rc.x1 = m_rc.x1 + dx;
    rc.x2 = m_rc.x2 + dx;
    rc.y1 = m_rc.y1 + dy;
    rc.y2 = m_rc.y2 + dy;
}

void CSelection::clampPoint(int& x, int& y)
{
    x = std::max<int>(x, 0);
    x = std::min<int>(x, m_imageWidth - 1);
    y = std::max<int>(y, 0);
    y = std::min<int>(y, m_imageHeight - 1);
}

void CSelection::setColor(bool selected)
{
    if(!selected)
    {
        m_selection->SetColor(255, 255, 255, 255);
    }
    else
    {
        m_selection->SetColor(0, 255, 0, 255);
    }
}

float CSelection::getTime()
{
    SystemTime now;
    ::gettimeofday(&now, 0);

    const unsigned delta = (unsigned)((now.tv_sec - m_timeLast.tv_sec) * 1000000 + (now.tv_usec - m_timeLast.tv_usec));

    m_timeLast = now;

    return delta * 0.000001f;
}

