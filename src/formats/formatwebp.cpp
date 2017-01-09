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

cFormatWebP::cFormatWebP(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
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

    desc.size = file.getSize();

    Buffer buffer;
    buffer.resize(file.getSize());
    file.read(buffer.data(), file.getSize());

    WebPBitstreamFeatures features;
    if (WebPGetFeatures(buffer.data(), buffer.size(), &features) != VP8_STATUS_OK)
    {
    printf("(EE) error\n");
        return false;
    }

    desc.images = 1;
    desc.current = 0;
    desc.width = features.width;
    desc.height = features.height;
  
    // printf("alpha: %d, animations: %d, format: %d\n"
          // , features.has_alpha, features.has_animation
          // , features.format);

    uint8_t* decoded = nullptr;
    if (features.has_alpha)
    {
        decoded = WebPDecodeRGBA(buffer.data(), buffer.size(), nullptr, nullptr);
        desc.pitch = features.width * 4;
        desc.bpp = 32;
        desc.bppImage = 32;
        desc.format = GL_RGBA;
    }
    else
    {
        decoded = WebPDecodeRGB(buffer.data(), buffer.size(), nullptr, nullptr);
        desc.pitch = features.width * 3;
        desc.bpp = 24;
        desc.bppImage = 24;
        desc.format = GL_RGB;
    }

    if (decoded == nullptr)
    {
        return false;
    }

    desc.bitmap.resize(desc.pitch * desc.height);
    ::memcpy(desc.bitmap.data(), decoded, desc.bitmap.size());

    WebPFree(decoded);

    return true;
}
