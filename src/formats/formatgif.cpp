/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatgif.h"
#include "common/bitmap_description.h"
#include "common/file.h"

#include <cstdio>
#include <cstring>
#include <memory>

void cFormatGif::GifDeleter::operator()(GifFileType* gifFile)
{
#if GIFLIB_MAJOR >= 5
    int errorCode = 0;
    auto result = DGifCloseFile(gifFile, &errorCode);
    (void)errorCode;
#else
    auto result = DGifCloseFile(gifFile);
#endif

    ::printf("(WW) GIF closed.\n");
    (void)result;
}

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

    void putRow(sBitmapDescription& desc, uint32_t row, uint32_t width, const SavedImage& image, const ColorMapObject* cmap, uint32_t transparentIdx)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            const uint32_t idx = image.RasterBits[row * width + x];
            const uint32_t pos = (row + image.ImageDesc.Top) * desc.pitch + (x + image.ImageDesc.Left) * 4;
            putPixel(desc, pos, &cmap->Colors[idx], transparentIdx == idx);
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

    cFile file;
    if (file.open(filename) == false)
    {
        ::printf("(EE) Error Opening GIF image.\n");
        return false;
    }

    desc.size = file.getSize();
    file.close();

    auto gif = OpenFile(filename);
    m_gif.reset(gif);

    if (m_gif.get() == nullptr)
    {
        ::printf("(EE) Error Opening GIF image.\n");
        return false;
    }

    int res = DGifSlurp(m_gif.get());
    if (res != GIF_OK || m_gif->ImageCount < 1)
    {
        ::printf("(EE) Error Opening GIF image.\n");
        return false;
    }

    return load(0, desc);
}

bool cFormatGif::LoadSubImageImpl(uint32_t current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool cFormatGif::load(uint32_t current, sBitmapDescription& desc)
{
    desc.images = m_gif->ImageCount;
    desc.isAnimation = desc.images > 1;
    desc.current = std::max<uint32_t>(current, 0);
    desc.current = std::min<uint32_t>(desc.current, desc.images - 1);

    const auto& image = m_gif->SavedImages[desc.current];

    auto cmap = image.ImageDesc.ColorMap;
    if (cmap == nullptr)
    {
        cmap = m_gif->SColorMap;
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
        desc.width = desc.images > 1 ? m_gif->SWidth : width;
        desc.height = desc.images > 1 ? m_gif->SHeight : height;
        desc.pitch = desc.width * 4;
        desc.bpp = 32;
        desc.bppImage = cmap->BitsPerPixel;
        desc.bitmap.resize(desc.pitch * desc.height);
        desc.format = GL_RGBA;
    }

    // look for the transparent color extension
    uint32_t transparentIdx = (uint32_t)-1U;
    for (int i = 0; i < image.ExtensionBlockCount; i++)
    {
        const auto& eb = image.ExtensionBlocks[i];
        if (eb.Function == 0xF9 && eb.ByteCount == 4)
        {
            const bool hasTransparency = (eb.Bytes[0] & 1) == 1;
            if (hasTransparency)
            {
                transparentIdx = eb.Bytes[3];
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
        struct Interlace
        {
            uint32_t offset;
            uint32_t jump;
        };
        const Interlace interlaced[] = {
            { 0, 8 },
            { 4, 8 },
            { 2, 4 },
            { 1, 2 },
        };

        uint32_t row = 0;

        for (const auto& interlace : interlaced)
        {
            for (uint32_t y = interlace.offset; y < height; y += interlace.jump)
            {
                putRow(desc, y, width, image, cmap, transparentIdx);

                updateProgress((float)row / height);
                row++;
            }
        }

        m_formatName = "gif/i";
    }
    else
    {
        for (uint32_t y = 0; y < height; y++)
        {
            putRow(desc, y, width, image, cmap, transparentIdx);

            updateProgress((float)y / height);
        }

        m_formatName = "gif/p";
    }

    return true;
}
