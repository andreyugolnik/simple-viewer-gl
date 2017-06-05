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
    const ImVec4 tagColor{ 255, 255, 150, 200 };
    const ImVec4 valueColor{ 255, 255, 255, 200 };
}

void cExifPopup::setExifList(const sBitmapDescription::ExifList& exifList)
{
    m_exif = exifList;
}

void cExifPopup::render()
{
    for (const auto& s : m_exif)
    {
        ImGui::TextColored(tagColor, "%s - %s", s.tag.c_str(), s.value.c_str());
    };
}
