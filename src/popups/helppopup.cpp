/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "helppopup.h"
#include "imgui/imgui.h"
#include "types/vector.h"

namespace
{
    const ImVec4 keyColor{ 1.0f, 1.0f, 0.5, 1.0f };
    const ImVec4 descriptionColor{ 1.0f, 1.0f, 1.0f, 1.0f };

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
        const int flags = ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings;

        ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_Appearing, { 0.5f, 0.5f });
        if (ImGui::Begin("Help", nullptr, flags))
        {
            // ImGui::Columns(2);

            // ImGui::TextColored(keyColor, "Key");
            // ImGui::NextColumn();
            // ImGui::TextColored(descriptionColor, "Description");
            // ImGui::Separator();
            // ImGui::NextColumn();

            for (const auto& s : KeyBindingsList)
            {
                ImGui::TextColored(keyColor, "%s", s.key);
                // ImGui::NextColumn();
                ImGui::SameLine(120.0f);
                ImGui::Bullet();
                ImGui::TextColored(descriptionColor, "%s", s.description);
                // ImGui::NextColumn();
            }
        }
        ImGui::End();
    }
}
