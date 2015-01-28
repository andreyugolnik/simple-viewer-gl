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

    for(int i = 0; i < m_selectionTexCount; i++)
    {
        m_selection[i].reset(new CQuad(m_selectionTexSize, m_selectionTexSize, buffer, GL_RGB));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        m_selection[i]->useFilter(false);

        // shift texture
        for(int y = 0; y < m_selectionTexSize; y++)
        {
            for(int x = 0; x < m_selectionTexSize; x++)
            {
            }
        }
    }

    delete[] buffer;
}

void CSelection::SetImageDimension(float _w, float _h)
{
    m_imageWidth = _w;
    m_imageHeight = _h;
    m_rc.Clear();
    getTime();
}

void CSelection::MouseButton(int x, int y, bool pressed)
{
    if(pressed == true)
    {
        m_rc.Normalize();
        bool inside = m_rc.TestPoint(x, y);

        m_mouseX = x;
        m_mouseY = y;

        if(m_mode == MODE_NONE)
        {
            if(inside == true)
            {
                m_mode = (m_corner == CORNER_CENTER ? MODE_MOVE : MODE_RESIZE);
            }
            else
            {
                m_mode = MODE_SELECT;
                clampPoint(x, y);
                m_rc.SetLeftTop(x, y);
                m_rc.Clear();
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
    }
}

void CSelection::MouseMove(int x, int y)
{
    updateCorner(x, y);

    if(m_mode != MODE_NONE)
    {
        int dx = x - m_mouseX;
        int dy = y - m_mouseY;

        // correct selection position
        dx = std::max<int>(dx, 0 - m_rc.x1);
        dx = std::min<int>(dx, m_imageWidth - (m_rc.x1 + m_rc.GetWidth()) - 1);
        dy = std::max<int>(dy, 0 - m_rc.y1);
        dy = std::min<int>(dy, m_imageHeight - (m_rc.y1 + m_rc.GetHeight()) - 1);

        switch(m_mode)
        {
        case MODE_NONE:   // do nothing here
            break;

        case MODE_SELECT:
            clampPoint(x, y);
            m_rc.SetRightBottom(x, y);
            break;

        case MODE_MOVE:
            m_rc.ShiftRect(dx, dy);
            break;

        case MODE_RESIZE:
            switch(m_corner)
            {
            case CORNER_NONE:   // do nothing here
            case CORNER_CENTER:
                break;

            case CORNER_LEFT:
                m_rc.x1 += dx;
                break;
            case CORNER_RIGHT:
                m_rc.x2 += dx;
                break;
            case CORNER_UP:
                m_rc.y1 += dy;
                break;
            case CORNER_DOWN:
                m_rc.y2 += dy;
                break;
            case CORNER_LEUP:
                m_rc.x1 += dx;
                m_rc.y1 += dy;
                break;
            case CORNER_RIUP:
                m_rc.x2 += dx;
                m_rc.y1 += dy;
                break;
            case CORNER_LEDN:
                m_rc.x1 += dx;
                m_rc.y2 += dy;
                break;
            case CORNER_RIDN:
                m_rc.x2 += dx;
                m_rc.y2 += dy;
                break;
            }
        }

        m_mouseX = x;
        m_mouseY = y;
    }
}

void CSelection::Render(const cVector& _delta, float _scale)
{
    float dt = getTime();

    m_timeDelta += dt * 10;
    int frame = (int)m_timeDelta;

    if(m_enabled == true && m_rc.IsSet() == true)
    {
        CRect<int> rc;
        setImagePos(rc, _delta.x * _scale, _delta.y * _scale);

        setColor(frame, m_corner != CORNER_UP);
        renderLine(rc.x1, rc.y1, rc.x2, rc.y1, frame);	// top line
        setColor(frame, m_corner != CORNER_DOWN);
        renderLine(rc.x1, rc.y2, rc.x2, rc.y2, frame);	// bottom line
        setColor(frame, m_corner != CORNER_LEFT);
        renderLine(rc.x1, rc.y1, rc.x1, rc.y2, frame);	// left line
        setColor(frame, m_corner != CORNER_RIGHT);
        renderLine(rc.x2, rc.y1, rc.x2, rc.y2, frame);	// right line
    }
}

const CRect<float>& CSelection::GetRect() const
{
    return m_rc;
}

int CSelection::GetCursor() const
{
    int cursor[] = { 0, 1, 2, 2, 3, 3, 4, 5, 5, 4 };
    return cursor[m_corner];
}

void CSelection::updateCorner(int x, int y)
{
    if(m_mode != MODE_NONE)
    {
        return;
    }

    m_rc.Normalize();
    if(m_rc.TestPoint(x, y) == true)
    {
        const int delta = 10;

        CRect<int> rcLe(m_rc.x1, m_rc.y1, m_rc.x1 + delta, m_rc.y2);
        CRect<int> rcRi(m_rc.x2 - delta, m_rc.y1, m_rc.x2, m_rc.y2);
        CRect<int> rcUp(m_rc.x1, m_rc.y1, m_rc.x2, m_rc.y1 + delta);
        CRect<int> rcDn(m_rc.x1, m_rc.y2 - delta, m_rc.x2, m_rc.y2);

        if(rcLe.TestPoint(x, y) == true)
        {
            if(rcUp.TestPoint(x, y) == true)
            {
                m_corner = CORNER_LEUP;
            }
            else if(rcDn.TestPoint(x, y) == true)
            {
                m_corner = CORNER_LEDN;
            }
            else
            {
                m_corner = CORNER_LEFT;
            }
        }
        else if(rcRi.TestPoint(x, y) == true)
        {
            if(rcUp.TestPoint(x, y) == true)
            {
                m_corner = CORNER_RIUP;
            }
            else if(rcDn.TestPoint(x, y) == true)
            {
                m_corner = CORNER_RIDN;
            }
            else
            {
                m_corner = CORNER_RIGHT;
            }
        }
        else if(rcUp.TestPoint(x, y) == true)
        {
            m_corner = CORNER_UP;
        }
        else if(rcDn.TestPoint(x, y) == true)
        {
            m_corner = CORNER_DOWN;
        }
        else
        {
            m_corner = CORNER_CENTER;
        }
    }
    else
    {
        m_corner = CORNER_NONE;
    }
}

void CSelection::renderLine(int x1, int y1, int x2, int y2, int frame)
{
    frame %= m_selectionTexCount;

    int x = std::min(x1, x2);
    int y = std::min(y1, y2);
    int w = (x1 == x2 ? 1 : m_rc.GetWidth());
    int h = (y1 == y2 ? 1 : m_rc.GetHeight());

    m_selection[frame]->SetSpriteSize(w, h);
    m_selection[frame]->Render(x, y);
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

void CSelection::setColor(int frame, bool std)
{
    frame %= m_selectionTexCount;

    if(std == true)
    {
        m_selection[frame]->SetColor(200, 255, 200, 150);
    }
    else
    {
        m_selection[frame]->SetColor(255, 255, 0, 255);
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

