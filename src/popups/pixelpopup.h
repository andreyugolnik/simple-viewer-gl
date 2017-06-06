/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "popup.h"

#include "imgui/imgui.h"
#include "quadseries.h"
#include "types/color.h"
#include "types/rect.h"
#include "types/vector.h"

#include <memory>
#include <string>
#include <vector>

struct sPixelInfo
{
    sPixelInfo()
    {
        reset();
    }

    void reset()
    {
        mouse = { 0.0f, 0.0f };
        point = { 0.0f, 0.0f };
        imgWidth = 0;
        imgHeight = 0;
        bpp = 0;
        color = { 0, 0, 0, 0 };
        rc.clear();
    }

    Vectorf mouse;
    Vectorf point;
    uint32_t imgWidth;
    uint32_t imgHeight;
    uint32_t bpp;
    cColor color;
    Rectf rc;
};

class cPixelPopup final : public cPopup
{
public:
    void render() override;

    void init();
    void setPixelInfo(const sPixelInfo& pi);
    void setCursor(int cursor);

private:
    bool isInsideImage(const Vectorf& pos) const;
    void renderCursor();
    void renderInfo();

private:
    sPixelInfo m_pixelInfo;

    struct Info
    {
        enum class Icon
        {
            Position,
            Color,
            Size,
            Rect,
        };
        Icon icon;
        ImVec4 color;
        std::string text;
        Vectorf offset;
    };
    std::vector<Info> m_info;

    std::unique_ptr<cQuadSeries> m_pointer;
    std::unique_ptr<cQuadSeries> m_icons;

    ImVec2 m_size;
};
