/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatgif.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <gif_lib.h>

#include <cstdio>
#include <cstring>
#include <memory>

namespace
{

    GifFileType* OpenFile(const char* path)
    {
#if GIFLIB_MAJOR >= 5
        int errorCode = 0;
        auto gifFile = DGifOpenFileName(path, &errorCode);
        (void)errorCode;
#else
        auto gifFile = DGifOpenFileName(path);
#endif

        return gifFile;
    }

    int CloseFile(GifFileType* gifFile)
    {
#if GIFLIB_MAJOR >= 5
        int errorCode = 0;
        auto result = DGifCloseFile(gifFile, &errorCode);
        (void)errorCode;
#else
        auto result = DGifCloseFile(gifFile);
#endif

        return result;
    }

    void putPixel(sBitmapDescription& desc, uint32_t pos, const GifColorType* color, bool transparent)
    {
        if (!desc.current || !transparent)
        {
            desc.bitmap[pos + 0] = color->Red;
            desc.bitmap[pos + 1] = color->Green;
            desc.bitmap[pos + 2] = color->Blue;
            desc.bitmap[pos + 3] = transparent ? 0 : 255;
        }
    }

}

cFormatGif::cFormatGif(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatGif::~cFormatGif()
{
}

bool cFormatGif::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, 6))
    {
        return false;
    }

    const auto h = buffer.data();
    return (h[0] == 'G' && h[1] == 'I' && h[2] == 'F' && h[3] == '8' && h[5] == 'a')
           && (h[4] == '7' || h[4] == '9');
}

bool cFormatGif::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    m_filename = filename;
    return load(0, desc);
}

bool cFormatGif::LoadSubImageImpl(uint32_t current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool cFormatGif::load(uint32_t current, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(m_filename.c_str()))
    {
        return false;
    }

    desc.size = file.getSize();
    file.close();

    std::unique_ptr<GifFileType, int (*)(GifFileType*)> gif(OpenFile(m_filename.c_str()), CloseFile);
    if (gif.get() == nullptr)
    {
        ::printf("(EE) Error Opening GIF image.\n");
        return false;
    }

    int res = DGifSlurp(gif.get());
    if (res != GIF_OK || gif->ImageCount < 1)
    {
        ::printf("(EE) Error Opening GIF image.\n");
        return false;
    }

    desc.images = gif->ImageCount;
    desc.isAnimation = desc.images > 1;
    desc.current = std::max<uint32_t>(current, 0);
    desc.current = std::min<uint32_t>(desc.current, desc.images - 1);

    const auto& image = gif->SavedImages[desc.current];

    auto cmap = image.ImageDesc.ColorMap;
    if (cmap == nullptr)
    {
        cmap = gif->SColorMap;
    }
    if (cmap == nullptr)
    {
        ::printf("(EE) Invalid GIF colormap.\n");
        return false;
    }

    const uint32_t width = image.ImageDesc.Width;
    const uint32_t height = image.ImageDesc.Height;

    // place next frame abov previous
    if (!desc.current || desc.bitmap.empty())
    {
        desc.width = desc.images > 1 ? gif->SWidth : width;
        desc.height = desc.images > 1 ? gif->SHeight : height;
        desc.pitch = desc.width * 4;
        desc.bpp = 32;
        desc.bppImage = cmap->BitsPerPixel;
        desc.bitmap.resize(desc.pitch * desc.height);
        desc.format = GL_RGBA;
    }

    // look for the transparent color extension
    uint32_t transparent = (uint32_t) - 1U;
    for (int i = 0; i < image.ExtensionBlockCount; i++)
    {
        const auto& eb = image.ExtensionBlocks[i];
        if (eb.Function == 0xF9 && eb.ByteCount == 4)
        {
            bool has_transparency = ((eb.Bytes[0] & 1) == 1);
            if (has_transparency)
            {
                transparent = eb.Bytes[3];
            }

            const uint32_t disposalMode = (eb.Bytes[0] >> 2) & 0x07;
            // ::printf("Disposal: %u at frame %u\n", disposalMode, desc.current);
            // DISPOSAL_UNSPECIFIED 0 // No disposal specified.
            // DISPOSE_DO_NOT       1 // Leave image in place
            // DISPOSE_BACKGROUND   2 // Set area too background color
            // DISPOSE_PREVIOUS     3 // Restore to previous content
            if (disposalMode == 2)
            {
                ::memset(desc.bitmap.data(), 0, desc.bitmap.size());
            }

            desc.delay = eb.Bytes[1] * 10 + eb.Bytes[2];
            // ::printf(" %d ms\n", desc.delay);
        }
    }

    if (image.ImageDesc.Interlace)
    {
        // Need to perform 4 passes on the images:
        const uint32_t interlacedOffset[] = { 0, 4, 2, 1 };  // The way Interlaced image should
        const uint32_t interlacedJumps[] = { 8, 8, 4, 2 };   // be read - offsets and jumps...
        uint32_t interlaced_y = 0;

        for (uint32_t i = 0; i < 4; i++)
        {
            for (uint32_t y = interlacedOffset[i]; y < height; y += interlacedJumps[i])
            {
                for (uint32_t x = 0; x < width; x++)
                {
                    const uint32_t idx = image.RasterBits[interlaced_y * width + x];
                    const uint32_t pos = (y + image.ImageDesc.Top) * desc.pitch + (x + image.ImageDesc.Left) * 4;
                    putPixel(desc, pos, &cmap->Colors[idx], transparent == idx);
                }

                updateProgress((float)interlaced_y / height);

                interlaced_y++;
            }
        }
    }
    else
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                const uint32_t idx = image.RasterBits[y * width + x];
                const uint32_t pos = (y + image.ImageDesc.Top) * desc.pitch + (x + image.ImageDesc.Left) * 4;
                putPixel(desc, pos, &cmap->Colors[idx], transparent == idx);
            }

            updateProgress((float)y / height);
        }
    }

    m_formatName = "gif";

    return true;
}
