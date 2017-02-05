/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "selection.h"
#include "quad.h"
#include "types/math.h"
#include "types/vector.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace
{

    const float delta = 20.0f;
    const float delta2 = delta * 0.5f;

    const uint32_t m_cellSize = 8;
    const uint32_t m_texSize = m_cellSize * 2;

}

void cSelection::init()
{
    std::vector<uint8_t> buffer(m_texSize * m_texSize);
    auto p = buffer.data();

    uint32_t idx = 0;
    const uint8_t colors[2] = { 0x20, 0xff };

    for (uint32_t y = 0; y < m_texSize; y++)
    {
        if (y % m_cellSize == 0)
        {
            idx = (idx + 1) % 2;
        }

        for (uint32_t x = 0; x < m_texSize; x++)
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

void cSelection::setImageDimension(float w, float h)
{
    m_imageWidth = w;
    m_imageHeight = h;
    m_rc.clear();
    m_rc_test.clear();
}

void cSelection::setScale(float scale)
{
    m_scale = scale;

    if (m_rc.isSet())
    {
        auto rc = m_rc;
        rc.normalize();
        const float d = delta2 / scale;
        m_rc_test.set(rc.x1 - d, rc.y1 - d, rc.x2 + d, rc.y2 + d);
    }
}

void cSelection::mouseButton(float x, float y, bool pressed)
{
    if (pressed)
    {
        m_rc.normalize();
        const bool inside = m_rc_test.testPoint(x, y);

        m_mouseX = x;
        m_mouseY = y;

        if (m_mode == eMouseMode::None)
        {
            if (inside)
            {
                m_mode = (m_corner == (uint32_t)eCorner::CR ? eMouseMode::Move : eMouseMode::Resize);
            }
            else
            {
                m_mode = eMouseMode::Select;
                clampPoint(x, y);
                m_rc.setLeftTop(x, y);
                m_rc.clear();
                const float d = delta2 / m_scale;
                m_rc_test.setLeftTop(x - d, y - d);
                m_rc_test.clear();
            }
        }
        else if (inside == false)
        {
            m_mode = eMouseMode::None;
            m_rc.clear();
            m_rc_test.clear();
        }
    }
    else
    {
        m_mode = eMouseMode::None;
        setScale(m_scale);
    }
}

void cSelection::mouseMove(float x, float y)
{
    updateCorner(x, y);

    if (m_mode != eMouseMode::None)
    {
        float dx = x - m_mouseX;
        float dy = y - m_mouseY;

        switch (m_mode)
        {
        case eMouseMode::None:   // do nothing here
            break;

        case eMouseMode::Select:
            clampPoint(x, y);
            m_rc.setRightBottom(x, y);
            break;

        case eMouseMode::Move:
            clampShiftDelta(dx, dy);
            m_rc.shiftRect(dx, dy);
            break;

        case eMouseMode::Resize:
            if ((m_corner & (uint32_t)eCorner::UP))
            {
                m_rc.y1 += dy;
            }
            if ((m_corner & (uint32_t)eCorner::RT))
            {
                m_rc.x2 += dx;
            }
            if ((m_corner & (uint32_t)eCorner::DN))
            {
                m_rc.y2 += dy;
            }
            if ((m_corner & (uint32_t)eCorner::LT))
            {
                m_rc.x1 += dx;
            }
            break;
        }

        m_rc.x1 = clamp<float>(0.0f, m_imageWidth - 1.0f, m_rc.x1);
        m_rc.y1 = clamp<float>(0.0f, m_imageHeight - 1.0f, m_rc.y1);
        m_rc.x2 = clamp<float>(0.0f, m_imageWidth - 1.0f, m_rc.x2);
        m_rc.y2 = clamp<float>(0.0f, m_imageHeight - 1.0f, m_rc.y2);

        m_mouseX += dx;
        m_mouseY += dy;
    }
}

void cSelection::clampShiftDelta(float& dx, float& dy)
{
    dx = clamp<float>(-m_rc.x1, m_imageWidth - m_rc.x2 - 1.0f, dx);
    dy = clamp<float>(-m_rc.y1, m_imageHeight - m_rc.y2 - 1.0f, dy);
}

void cSelection::render(float dx, float dy)
{
    if (m_enabled && m_rc.isSet())
    {
        Rectf rc;
        setImagePos(rc, dx, dy);

        const float d = 1.0f / m_scale;
        const float x = std::min<float>(rc.x1, rc.x2);
        const float y = std::min<float>(rc.y1, rc.y2);
        const float w = rc.width();
        const float h = rc.height();
        // top line
        setColor(m_corner & (uint32_t)eCorner::UP);
        renderHorizontal(x - d, y - d, w + 2.0f * d, d);
        // bottom line
        setColor(m_corner & (uint32_t)eCorner::DN);
        renderHorizontal(x - d, y + h + d, w + 2.0f * d, d);
        // left line
        setColor(m_corner & (uint32_t)eCorner::LT);
        renderVertical(x - d, y, h, d);
        // right line
        setColor(m_corner & (uint32_t)eCorner::RT);
        renderVertical(x + w + d, y, h, d);
    }
}

const Rectf& cSelection::getRect() const
{
    return m_rc;
}

int cSelection::getCursor() const
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
    if (m_mode != eMouseMode::None)
    {
        return;
    }

    const Rectf& rc = m_rc_test;
    if (rc.testPoint(x, y))
    {
        m_corner = 0;
        const float d = delta / m_scale;
        Rectf rcLt(rc.x1, rc.y1, rc.x1 + d, rc.y2);
        if (rcLt.testPoint(x, y))
        {
            m_corner |= (uint32_t)eCorner::LT;
        }

        Rectf rcRt(rc.x2 - d, rc.y1, rc.x2, rc.y2);
        if (rcRt.testPoint(x, y))
        {
            m_corner |= (uint32_t)eCorner::RT;
        }

        Rectf rcUp(rc.x1, rc.y1, rc.x2, rc.y1 + d);
        if (rcUp.testPoint(x, y))
        {
            m_corner |= (uint32_t)eCorner::UP;
        }

        Rectf rcDn(rc.x1, rc.y2 - d, rc.x2, rc.y2);
        if (rcDn.testPoint(x, y))
        {
            m_corner |= (uint32_t)eCorner::DN;
        }

        if (!m_corner)
        {
            m_corner = (uint32_t)eCorner::CR;
        }
    }
    else
    {
        m_corner = (uint32_t)eCorner::None;
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

void cSelection::setImagePos(Rectf& rc, float dx, float dy)
{
    rc.x1 = m_rc.x1 + dx;
    rc.x2 = m_rc.x2 + dx;
    rc.y1 = m_rc.y1 + dy;
    rc.y2 = m_rc.y2 + dy;
}

void cSelection::clampPoint(float& x, float& y)
{
    x = clamp<float>(0.0f, m_imageWidth - 1.0f, x);
    y = clamp<float>(0.0f, m_imageHeight - 1.0f, y);
}

void cSelection::setColor(bool selected)
{
    if (selected == false)
    {
        m_selection->setColor({ 255, 255, 255, 255 });
    }
    else
    {
        m_selection->setColor({ 0, 255, 0, 255 });
    }
}
