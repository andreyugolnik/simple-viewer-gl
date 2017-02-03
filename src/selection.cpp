/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "selection.h"
#include "math/vector.h"
#include "quad.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace
{

    const float delta = 20.0f;
    const float delta2 = delta * 0.5f;

    const unsigned m_cellSize = 8;
    const unsigned m_texSize = m_cellSize * 2;

}

void cSelection::Init()
{
    std::vector<unsigned char> buffer(m_texSize * m_texSize);
    auto p = buffer.data();

    unsigned idx = 0;
    const unsigned char colors[2] = { 0x20, 0xff };

    for (unsigned y = 0; y < m_texSize; y++)
    {
        if (y % m_cellSize == 0)
        {
            idx = (idx + 1) % 2;
        }

        for (unsigned x = 0; x < m_texSize; x++)
        {
            if (x % m_cellSize == 0)
            {
                idx = (idx + 1) % 2;
            }

            const auto color = colors[idx];
            *p++ = color;
        }
    }

    m_selection.reset(new cQuad(m_texSize, m_texSize, buffer.data(), GL_LUMINANCE));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    m_selection->useFilter(false);
}

void cSelection::SetImageDimension(float w, float h)
{
    m_imageWidth = w;
    m_imageHeight = h;
    m_rc.Clear();
    m_rc_test.Clear();
}

void cSelection::setScale(float scale)
{
    m_scale = scale;

    if (m_rc.IsSet())
    {
        m_rc.Normalize();
        const float d = delta2 / scale;
        m_rc_test.Set(m_rc.x1 - d, m_rc.y1 - d, m_rc.x2 + d, m_rc.y2 + d);
    }
}

void cSelection::MouseButton(float x, float y, bool pressed)
{
    if (pressed)
    {
        m_rc.Normalize();
        const bool inside = m_rc_test.TestPoint(x, y);

        m_mouseX = x;
        m_mouseY = y;

        if (m_mode == MODE_NONE)
        {
            if (inside)
            {
                m_mode = (m_corner == CORNER_CR ? MODE_MOVE : MODE_RESIZE);
            }
            else
            {
                m_mode = MODE_SELECT;
                clampPoint(x, y);
                m_rc.SetLeftTop(x, y);
                m_rc.Clear();
                const float d = delta2 / m_scale;
                m_rc_test.SetLeftTop(x - d, y - d);
                m_rc_test.Clear();
            }
        }
        else if (inside == false)
        {
            m_mode = MODE_NONE;
            m_rc.Clear();
            m_rc_test.Clear();
        }
    }
    else
    {
        m_mode = MODE_NONE;
        setScale(m_scale);
    }
}

void cSelection::MouseMove(float x, float y)
{
    updateCorner(x, y);

    if (m_mode != MODE_NONE)
    {
        float dx = x - m_mouseX;
        float dy = y - m_mouseY;

        switch (m_mode)
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
            if ((m_corner & CORNER_UP))
            {
                m_rc.y1 += dy;
            }
            if ((m_corner & CORNER_RT))
            {
                m_rc.x2 += dx;
            }
            if ((m_corner & CORNER_DN))
            {
                m_rc.y2 += dy;
            }
            if ((m_corner & CORNER_LT))
            {
                m_rc.x1 += dx;
            }
            break;
        }

        m_rc.x1 = m_rc.x1 >= 0.0f ? m_rc.x1 : 0.0f;
        m_rc.y1 = m_rc.y1 >= 0.0f ? m_rc.y1 : 0.0f;

        m_rc.x2 = m_rc.x2 < m_imageWidth  ? m_rc.x2 : m_imageWidth  - 1.0f;
        m_rc.y2 = m_rc.y2 < m_imageHeight ? m_rc.y2 : m_imageHeight - 1.0f;

        m_mouseX += dx;
        m_mouseY += dy;
    }
}

void cSelection::clampShiftDelta(float& dx, float& dy)
{
    if (m_rc.x1 + dx < 0.0f)
    {
        dx = -m_rc.x1;
    }
    else if (m_rc.x2 + dx >= m_imageWidth)
    {
        dx = m_imageWidth - 1.0f - m_rc.x2;
    }
    if (m_rc.y1 + dy < 0.0f)
    {
        dy = -m_rc.y1;
    }
    else if (m_rc.y2 + dy >= m_imageHeight)
    {
        dy = m_imageHeight - 1.0f - m_rc.y2;
    }
}

void cSelection::Render(float dx, float dy)
{
    if (m_enabled && m_rc.IsSet())
    {
        CRect<float> rc;
        setImagePos(rc, dx, dy);

        const float d = 1.0f / m_scale;
        const float x = std::min<float>(rc.x1, rc.x2);
        const float y = std::min<float>(rc.y1, rc.y2);
        const float w = rc.GetWidth();
        const float h = rc.GetHeight();
        // top line
        setColor(m_corner & CORNER_UP);
        renderHorizontal(x - d, y - d, w + 2.0f * d, d);
        // bottom line
        setColor(m_corner & CORNER_DN);
        renderHorizontal(x - d, y + h + d, w + 2.0f * d, d);
        // left line
        setColor(m_corner & CORNER_LT);
        renderVertical(x - d, y, h, d);
        // right line
        setColor(m_corner & CORNER_RT);
        renderVertical(x + w + d, y, h, d);
    }
}

const CRect<float>& cSelection::GetRect() const
{
    return m_rc;
}

int cSelection::GetCursor() const
{
    static const int cursor[16 + 1] =
    {
        //                                 1  1  1  1  1
        //0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
        0, 3, 2, 5, 3, 0, 4, 2, 2, 4, 0, 3, 5, 2, 3, 1, 1
    };
    //printf("corner: %u , cursor: %d\n", m_corner, cursor[m_corner]);
    return cursor[m_corner];
}

void cSelection::updateCorner(float x, float y)
{
    if (m_mode != MODE_NONE)
    {
        return;
    }

    const CRect<float>& rc = m_rc_test;
    if (rc.TestPoint(x, y))
    {
        m_corner = 0;
        const float d = delta / m_scale;
        CRect<float> rcLt(rc.x1, rc.y1, rc.x1 + d, rc.y2);
        if (rcLt.TestPoint(x, y))
        {
            m_corner |= CORNER_LT;
        }

        CRect<float> rcRt(rc.x2 - d, rc.y1, rc.x2, rc.y2);
        if (rcRt.TestPoint(x, y))
        {
            m_corner |= CORNER_RT;
        }

        CRect<float> rcUp(rc.x1, rc.y1, rc.x2, rc.y1 + d);
        if (rcUp.TestPoint(x, y))
        {
            m_corner |= CORNER_UP;
        }

        CRect<float> rcDn(rc.x1, rc.y2 - d, rc.x2, rc.y2);
        if (rcDn.TestPoint(x, y))
        {
            m_corner |= CORNER_DN;
        }

        if (!m_corner)
        {
            m_corner = CORNER_CR;
        }
    }
    else
    {
        m_corner = CORNER_NONE;
    }
}

void cSelection::renderHorizontal(float x, float y, float w, float scale)
{
    const auto offset = glfwGetTime() * 10.0f;
    m_selection->SetTextureRect(offset, 0.0f, w, scale);
    m_selection->Render(x, y);
}

void cSelection::renderVertical(float x, float y, float h, float scale)
{
    const auto offset = glfwGetTime() * 10.0f;
    m_selection->SetTextureRect(0.0f, offset, scale, h);
    m_selection->Render(x, y);
}

void cSelection::setImagePos(CRect<float>& rc, float dx, float dy)
{
    rc.x1 = m_rc.x1 + dx;
    rc.x2 = m_rc.x2 + dx;
    rc.y1 = m_rc.y1 + dy;
    rc.y2 = m_rc.y2 + dy;
}

void cSelection::clampPoint(float& x, float& y)
{
    x = std::max<float>(x, 0.0f);
    x = std::min<float>(x, m_imageWidth - 1.0f);
    y = std::max<float>(y, 0.0f);
    y = std::min<float>(y, m_imageHeight - 1.0f);
}

void cSelection::setColor(bool selected)
{
    if (selected == false)
    {
        m_selection->SetColor(255, 255, 255, 255);
    }
    else
    {
        m_selection->SetColor(0, 255, 0, 255);
    }
}
