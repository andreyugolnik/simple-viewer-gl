/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatwebp.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <webp/decode.h>

cFormatWebP::cFormatWebP(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
}

cFormatWebP::~cFormatWebP()
{
}

bool cFormatWebP::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    Buffer buffer;
    buffer.resize(file.getSize());
    file.read(buffer.data(), file.getSize());

    WebPBitstreamFeatures features;
    auto error = WebPGetFeatures(buffer.data(), buffer.size(), &features);
    if (error != VP8_STATUS_OK)
    {
        ::printf("(EE) Error loading WebP: %d.\n", error);
        return false;
    }

    desc.size = file.getSize();

    desc.images = 1;
    desc.current = 0;
    desc.width = features.width;
    desc.height = features.height;
  
    // ::printf("alpha: %d, animations: %d, format: %d\n"
          // , features.has_alpha, features.has_animation
          // , features.format);

    if (features.has_alpha)
    {
        desc.pitch = features.width * 4;
        desc.bpp = 32;
        desc.bppImage = 32;
        desc.format = GL_RGBA;
        desc.bitmap.resize(desc.pitch * desc.height);

        if (WebPDecodeRGBAInto(buffer.data(), buffer.size(), desc.bitmap.data(), desc.bitmap.size(), desc.pitch) == nullptr)
        {
            ::printf("(EE) Error decoding WebP.\n");
            return false;
        }
    }
    else
    {
        desc.pitch = features.width * 3;
        desc.bpp = 24;
        desc.bppImage = 24;
        desc.format = GL_RGB;
        desc.bitmap.resize(desc.pitch * desc.height);

        if (WebPDecodeRGBInto(buffer.data(), buffer.size(), desc.bitmap.data(), desc.bitmap.size(), desc.pitch) == nullptr)
        {
            ::printf("(EE) Error decoding WebP.\n");
            return false;
        }
    }

    m_formatName = "webp";

    return true;
}
