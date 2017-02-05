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
    m_rcTest.clear();
}

void cSelection::updateTestRect()
{
    if (m_rc.isSet())
    {
        auto rc = m_rc;
        rc.normalize();
        const float d = delta2 / cRenderer::getZoom();
        m_rcTest.set({ rc.tl.x - d, rc.tl.y - d }, { rc.br.x + d, rc.br.y + d });
    }
}

void cSelection::mouseButton(const Vectorf& p, bool pressed)
{
    updateTestRect();

    if (pressed)
    {
        auto pos = p;

        m_rc.normalize();
        const bool inside = m_rcTest.testPoint(pos);

        m_mousePos = pos;

        if (m_mode == eMouseMode::None)
        {
            if (inside)
            {
                m_mode = (m_corner == (uint32_t)eCorner::CR ? eMouseMode::Move : eMouseMode::Resize);
            }
            else
            {
                m_mode = eMouseMode::Select;
                clampPoint(pos);
                m_rc.setLeftTop(pos);
                m_rc.clear();
                const float d = delta2 / cRenderer::getZoom();
                m_rcTest.setLeftTop(pos - d);
                m_rcTest.clear();
            }
        }
        else if (inside == false)
        {
            m_mode = eMouseMode::None;
            m_rc.clear();
            m_rcTest.clear();
        }
    }
    else
    {
        m_mode = eMouseMode::None;
    }
}

void cSelection::mouseMove(const Vectorf& p)
{
    auto pos = p;
    updateCorner(pos);

    if (m_mode != eMouseMode::None)
    {
        auto delta = pos - m_mousePos;

        switch (m_mode)
        {
        case eMouseMode::None:   // do nothing here
            break;

        case eMouseMode::Select:
            clampPoint(pos);
            m_rc.setRightBottom(pos);
            break;

        case eMouseMode::Move:
            clampShiftDelta(delta);
            m_rc.shiftRect(delta);
            break;

        case eMouseMode::Resize:
            if ((m_corner & (uint32_t)eCorner::UP))
            {
                m_rc.tl.y += delta.y;
            }
            if ((m_corner & (uint32_t)eCorner::RT))
            {
                m_rc.br.x += delta.x;
            }
            if ((m_corner & (uint32_t)eCorner::DN))
            {
                m_rc.br.y += delta.y;
            }
            if ((m_corner & (uint32_t)eCorner::LT))
            {
                m_rc.tl.x += delta.x;
            }
            break;
        }

        m_rc.tl.x = clamp<float>(0.0f, m_imageWidth - 1.0f, m_rc.tl.x);
        m_rc.tl.y = clamp<float>(0.0f, m_imageHeight - 1.0f, m_rc.tl.y);
        m_rc.br.x = clamp<float>(0.0f, m_imageWidth - 1.0f, m_rc.br.x);
        m_rc.br.y = clamp<float>(0.0f, m_imageHeight - 1.0f, m_rc.br.y);

        m_mousePos += delta;
    }
}

void cSelection::clampShiftDelta(Vectorf& delta)
{
    delta.x = clamp<float>(-m_rc.tl.x, m_imageWidth - m_rc.br.x - 1.0f, delta.x);
    delta.y = clamp<float>(-m_rc.tl.y, m_imageHeight - m_rc.br.y - 1.0f, delta.y);
}

void cSelection::render(const Vectorf& offset)
{
    if (m_enabled && m_rc.isSet())
    {
        Rectf rc;
        setImagePos(rc, offset);

        rc.normalize();
        const float x = rc.tl.x;
        const float y = rc.tl.y;
        const float w = rc.width() + 1.0f;
        const float h = rc.height() + 1.0f;

        const float thickness = 1.0f;
        const float d = thickness / cRenderer::getZoom();

        // top line
        setColor(m_corner & (uint32_t)eCorner::UP);
        renderHorizontal(x - d, y - d, w + d * 2.0f, d);
        // bottom line
        setColor(m_corner & (uint32_t)eCorner::DN);
        renderHorizontal(x - d, y + h + d, w + d * 2.0f, d);
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

void cSelection::updateCorner(const Vectorf& pos)
{
    if (m_mode != eMouseMode::None)
    {
        return;
    }

    updateTestRect();
    const auto& rc = m_rcTest;
    if (rc.testPoint(pos))
    {
        m_corner = 0;

        const float d = delta / cRenderer::getZoom();

        const Rectf rcLt(rc.tl, { rc.tl.x + d, rc.br.y });
        if (rcLt.testPoint(pos))
        {
            m_corner |= (uint32_t)eCorner::LT;
        }

        const Rectf rcRt({ rc.br.x - d, rc.tl.y }, rc.br);
        if (rcRt.testPoint(pos))
        {
            m_corner |= (uint32_t)eCorner::RT;
        }

        const Rectf rcUp(rc.tl, { rc.br.x, rc.tl.y + d });
        if (rcUp.testPoint(pos))
        {
            m_corner |= (uint32_t)eCorner::UP;
        }

        const Rectf rcDn({ rc.tl.x, rc.br.y - d }, rc.br);
        if (rcDn.testPoint(pos))
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

void cSelection::setImagePos(Rectf& rc, const Vectorf& offset)
{
    rc = m_rc;
    rc.shiftRect(offset);
}

void cSelection::clampPoint(Vectorf& pos)
{
    pos.x = clamp<float>(0.0f, m_imageWidth - 1.0f, pos.x);
    pos.y = clamp<float>(0.0f, m_imageHeight - 1.0f, pos.y);
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
