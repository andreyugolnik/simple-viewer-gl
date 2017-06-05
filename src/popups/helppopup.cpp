/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "helppopup.h"
#include "types/vector.h"

#include "imgui/imgui.h"

namespace
{
    const int AlphaColor = 200;
    const ImVec4 keyColor{ 255, 255, 150, AlphaColor };
    const ImVec4 descriptionColor{ 255, 255, 255, AlphaColor };

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

void cHelpPopup::render()
{
    if (m_isVisible)
    {
        // int width;
        // int height;
        // glfwGetFramebufferSize(cRenderer::getWindow(), &width, &height);

        // Vectorf pos{ width - (m_bgSize.x + 5.0f), 5.0f };

        for (const auto& s : KeyBindingsList)
        {
            ImGui::TextColored(keyColor, "%s - %s", s.key, s.description);
        };
    }
}
