/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "helppopup.h"

#include <cstring>

namespace
{
    const int AlphaColor = 200;
    const cColor keyColor{ 255, 255, 150, AlphaColor };
    const cColor descriptionColor{ 255, 255, 255, AlphaColor };

    const float Border = 10.0f;
    const float RowHeight = 30.0f;

    struct KeyBinding
    {
        const char* key;
        const char* description;
    };

    const KeyBinding KeyBindingsList[] = {
        { "<esc> / <q>", "exit" },
        { "<space>", "next image" },
        { "<backspace>", "previous image" },
        { "<+> / <->", "scale image" },
        { "<1>...<0>", "set scale from 100% to 1000%" },
        { "<enter>", "switch fullscreen / windowed mode" },
        { "<del>", "toggle deletion mark" },
        { "<ctrl>+<del>", "delete marked images from disk" },
        { "<r>", "rotate clockwise" },
        { "<shift>+<r>", "rotate counterclockwise" },
        { "<pgup> / <bgdn>", "previous /next subimage" },
        { "<s>", "fit image to window" },
        { "<shift>+<s>", "toggle 'keep scale' on image load" },
        { "<c>", "hide / show chequerboard" },
        { "<i>", "hide / show on-screen info" },
        { "<e>", "hide / show exif" },
        { "<p>", "hide / show pixel info" },
        { "<b>", "hide / show border around image" },
    };
}

void cHelpPopup::init()
{
    createBackground();

    const int DesiredFontSize = 30;
    createFont(DesiredFontSize);

    m_dirty = true;
}

void cHelpPopup::setScale(float scale)
{
    cPopup::setScale(scale);

    m_dirty = true;
}

void cHelpPopup::calculate()
{
    float width = 0.0f;
    const float space = m_ft->getBounds(" ").x;
    m_descriptionOffset = 0.0f;

    for (const auto& s : KeyBindingsList)
    {
        auto keyBounds = m_ft->getBounds(s.key);
        m_descriptionOffset = std::max<float>(m_descriptionOffset, keyBounds.x + space);
    }

    for (const auto& s : KeyBindingsList)
    {
        auto descriptionBounds = m_ft->getBounds(s.description);
        width = std::max<float>(width, m_descriptionOffset + descriptionBounds.x);
    }

    const auto rows = sizeof(KeyBindingsList) / sizeof(KeyBindingsList[0]);
    m_bgSize = {
        width + 2.0f * Border,
        RowHeight * rows + 2.0f * Border
    };
    m_bgSize *= m_scale;
}

void cHelpPopup::render()
{
    if (m_isVisible)
    {
        if (m_dirty)
        {
            m_dirty = false;
            calculate();
        }

        int width;
        int height;
        glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

        Vectorf pos{ width - (m_bgSize.x + 5.0f), 5.0f };

        m_bg->setSpriteSize(m_bgSize);
        m_bg->render(pos);

        const auto scale = m_scale;
        const auto offset = Vectorf{ m_descriptionOffset, 0.0f } * scale;

        pos += Vectorf{ Border, Border } * scale;
        for (const auto& s : KeyBindingsList)
        {
            m_ft->setColor(keyColor);
            m_ft->draw(pos, s.key, scale);

            m_ft->setColor(descriptionColor);
            m_ft->draw(pos + offset, s.description, scale);

            pos.y += RowHeight;
        };
    }
}
