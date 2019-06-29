/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatsvg.h"
#include "common/bitmap_description.h"
#include "common/file.h"
#include "common/config.h"

#include <cfloat>
#include <cmath>
#include <cstring>

#define NANOSVG_IMPLEMENTATION
#include "formats/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "formats/nanosvgrast.h"

cFormatSvg::cFormatSvg(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatSvg::~cFormatSvg()
{
}

bool cFormatSvg::isSupported(cFile& file, Buffer& buffer) const
{
    auto len = std::min<uint32_t>(file.getSize(), 4096);

    if (!readBuffer(file, buffer, len))
    {
        return false;
    }

    auto magic = (char*)buffer.data();
    magic[len] = '\0';

    return ::strstr(magic, "<svg") != nullptr;
}

bool cFormatSvg::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (file.open(filename) == false)
    {
        return false;
    }

    std::vector<char> data(file.getSize());
    if (file.read(data.data(), file.getSize()) != file.getSize())
    {
        ::printf("(EE) Couldn't read file.\n");
        return false;
    }

    auto image = nsvgParse(data.data(), "px", 96.0f);
    if (image == nullptr)
    {
        ::printf("(EE) Couldn't parse SVG image.\n");
        return false;
    }

    auto rasterizer = nsvgCreateRasterizer();
    if (!rasterizer)
    {
        ::printf("(EE) Couldn't create SVG rasterizer.\n");
        nsvgDelete(image);
        return false;
    }

    const auto mw = m_config->minSvgSize.x;
    const auto sw = image->width < mw ? (mw / image->width) : 1.0f;

    const auto mh = m_config->minSvgSize.y;
    const auto sh = image->height < mh ? (mh / image->height) : 1.0f;

    const auto scale = std::min(sw, sh);

    desc.size = file.getSize();
    desc.images = 1;
    desc.format = GL_RGBA;
    desc.bpp = 32;
    desc.bppImage = 32;
    desc.width = image->width * scale;
    desc.height = image->height * scale;
    desc.pitch = desc.width * 4;
    desc.bitmap.resize(desc.pitch * desc.height);
    auto pix = desc.bitmap.data();
    // std::fill(desc.bitmap.begin(), desc.bitmap.end(), 0);

    m_formatName = "svg";

    nsvgRasterize(rasterizer, image, 0.0f, 0.0f, scale, pix, desc.width, desc.height, desc.pitch);
    nsvgDeleteRasterizer(rasterizer);
    nsvgDelete(image);

    return true;
}
