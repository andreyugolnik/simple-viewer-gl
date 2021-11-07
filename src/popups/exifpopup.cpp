/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "exifpopup.h"
#include "imgui/imgui.h"

namespace
{
    const ImVec4 tagColor{ 1.0f, 1.0f, 0.5f, 1.0f };
    const ImVec4 valueColor{ 1.0f, 1.0f, 1.0f, 1.0f };
}

void cExifPopup::setExifList(const sBitmapDescription::ExifList& exifList)
{
    m_exif = exifList;
}

void cExifPopup::render()
{
    if (m_exif.size())
    {
        const int flags = ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing;

        ImGui::SetNextWindowPos({ 5.0f, 5.0f }, ImGuiCond_Once);
        if (ImGui::Begin("EXIF", nullptr, flags))
        {
            ImGui::Columns(2);

            ImGui::TextColored(tagColor, "Tag");
            ImGui::NextColumn();
            ImGui::TextColored(valueColor, "Value");
            ImGui::NextColumn();
            ImGui::Separator();

            for (const auto& s : m_exif)
            {
                ImGui::TextColored(tagColor, "%s", s.tag.c_str());
                ImGui::NextColumn();
                ImGui::TextColored(valueColor, "%s", s.value.c_str());
                ImGui::NextColumn();
            }
        }
        ImGui::End();
    }
}
