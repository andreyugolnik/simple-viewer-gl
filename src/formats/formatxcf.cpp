/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatxcf.h"
#include "common/bitmap_description.h"
#include "common/file.h"

#define XCF

#if defined(XCF)
#include "formats/xcf.h"
#elif defined(SDL_XCF)
#include "sdl_xcf.h"
#elif defined(XCFTOOLS)
#include <xcftools/flatten.h>
#include <xcftools/palette.h>
#include <xcftools/xcftools.h>
#endif

#include <cstdio>
#include <cstring>

cFormatXcf::cFormatXcf(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatXcf::~cFormatXcf()
{
}

bool cFormatXcf::isSupported(cFile& file, Buffer& buffer) const
{
    const char header[8] = { 'g', 'i', 'm', 'p', ' ', 'x', 'c', 'f' };

    if (!readBuffer(file, buffer, sizeof(header)))
    {
        return false;
    }

    return ::memcmp(buffer.data(), header, sizeof(header)) == 0;
}

#if defined(XCFTOOLS)
namespace
{
    FlattenSpec flatspec;

    enum FlattenSpec::out_color_mode guessIndexed(FlattenSpec* spec, rgba* allPixels[])
    {
        if (allPixels == nullptr)
        {
            if (spec->gimpish_indexed && colormapLength)
            {
                init_palette_hash();
                for (uint32_t i = 0; i < colormapLength; i++)
                {
                    lookup_or_intern(NEWALPHA(colormap[i], 255));
                }
                if (lookup_or_intern(FULLALPHA(spec->default_pixel) ? spec->default_pixel : 0) >= 0)
                {
                    return FlattenSpec::COLOR_INDEXED;
                }
            }
        }
        else
        {
            init_palette_hash();
            if (palettify_rows(allPixels, spec->dim.width, spec->dim.height))
            {
                // Might grayscale sometimes be preferred? No, that is what -g is for!
                return FlattenSpec::COLOR_INDEXED;
            }
        }
        return FlattenSpec::COLOR_BY_CONTENTS;
    }

    rgba findUnusedColor(rgba* pixels[], uint32_t width, uint32_t height)
    {
        size_t freqtab[256];

        for (uint32_t i = 0; i < 256; i++)
        {
            freqtab[i] = 0;
        }

        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                if (pixels[y][x])
                {
                    freqtab[255 & (pixels[y][x] >> RED_SHIFT)]++;
                }
            }
        }

        uint32_t j = 0;
        for (uint32_t i = 0; i < 256; i++)
        {
            if (freqtab[i] == 0)
            {
                return ((rgba)i << RED_SHIFT) + ((rgba)i << GREEN_SHIFT) + ((rgba)i << BLUE_SHIFT) + ((rgba)255 << ALPHA_SHIFT);
            }
            if (freqtab[i] < freqtab[j])
            {
                j = i;
            }
        }

        rgba sofar = ((rgba)255 << ALPHA_SHIFT) + ((rgba)j << RED_SHIFT);

        for (uint32_t i = 0; i < 256; i++)
        {
            freqtab[i] = 0;
        }
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                if (((((rgba)255 << ALPHA_SHIFT) + ((rgba)255 << RED_SHIFT)) & pixels[y][x]) == sofar)
                {
                    freqtab[255 & (pixels[y][x] >> GREEN_SHIFT)]++;
                }
            }
        }

        j = 0;
        for (uint32_t i = 0; i < 256; i++)
        {
            if (freqtab[i] == 0)
            {
                return sofar + ((rgba)i << GREEN_SHIFT);
            }
            if (freqtab[i] < freqtab[j])
            {
                j = i;
            }
        }
        sofar += (rgba)j << GREEN_SHIFT;

        for (uint32_t i = 0; i < 256; i++)
        {
            freqtab[i] = 0;
        }
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                if (((((rgba)255 << ALPHA_SHIFT) + ((rgba)255 << RED_SHIFT) + ((rgba)255 << GREEN_SHIFT)) & pixels[y][x]) == sofar)
                {
                    freqtab[255 & (pixels[y][x] >> BLUE_SHIFT)]++;
                }
            }
        }

        for (uint32_t i = 0; i < 256; i++)
        {
            if (freqtab[i] == 0)
            {
                return sofar + ((rgba)i << BLUE_SHIFT);
            }
        }

        return 0;
    }

    uint32_t Offset = 0;
    uint8_t* Bitmap = nullptr;
    void raw_callback(uint32_t num, rgba* pixels)
    {
        struct RGBA
        {
            uint8_t r, g, b, a;
        };
        auto in = (RGBA*)pixels;
        auto out = (RGBA*)(Bitmap + Offset);
        for (uint32_t i = 0; i < num; i++)
        {
            out[i].r = in[i].g;
            out[i].g = in[i].b;
            out[i].b = in[i].a;
            out[i].a = in[i].r;
        }

        Offset += num * sizeof(uint32_t);

        xcffree(pixels);
    }

    void graying_callback(uint32_t num, rgba* pixels)
    {
        auto fillptr = (uint8_t*)pixels;
        for (uint32_t i = 0; i < num; i++)
        {
            rgba pixel = pixels[i];
            int g = degrayPixel(pixel);
            if (g == -1)
            {
                FatalGeneric(103, _("Grayscale output selected, but colored pixel(s) found"));
            }
            *fillptr++ = g;
            *fillptr++ = ALPHA(pixel);
        }

        raw_callback(num, pixels);
    }

    void optimistic_palette_callback(uint32_t num, rgba* pixels)
    {
        uint32_t prev_size = paletteSize;
        if (!palettify_row(pixels, num) || paletteSize != prev_size)
        {
            FatalUnexpected("Oops! Somehow the precomputed palette does not suffice after all...");
        }
        raw_callback(num, pixels);
    }

    lineCallback selectCallback(enum FlattenSpec::out_color_mode colorMode)
    {
        switch (colorMode)
        {
        case FlattenSpec::COLOR_GRAY:
            return &graying_callback;

        case FlattenSpec::COLOR_INDEXED:
            if (flatspec.process_in_memory)
            {
                return &raw_callback;
            }
            return &optimistic_palette_callback;

        case FlattenSpec::COLOR_RGB:
        default:
            return &raw_callback;
        }
    }

} // namespace
#endif

bool cFormatXcf::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (file.open(filename) == false)
    {
        return false;
    }

    m_formatName = "xcf";

#if defined(XCF)

    return import_xcf(file, desc);

#elif defined(SDL_XCF)

    return load(file, desc);

#elif defined(XCFTOOLS)
    Offset = 0;
    Bitmap = nullptr;

    init_flatspec(&flatspec);

    const char* unzipper = nullptr;
    read_or_mmap_xcf(filename, unzipper);

    if (getBasicXcfInfo() == false)
    {
        return false;
    }

    initColormap();

    complete_flatspec(&flatspec, guessIndexed);

    desc.size = file.getSize();

    desc.images = 1;
    desc.current = 0;
    desc.width = flatspec.dim.width;
    desc.height = flatspec.dim.height;

    desc.pitch = desc.width * 4;
    desc.bpp = 32;
    desc.bppImage = 32;
    desc.format = GL_RGBA;
    desc.bitmap.resize(desc.pitch * desc.height);
    Bitmap = desc.bitmap.data();

    m_formatName = "xcf";

    if (flatspec.process_in_memory)
    {
        rgba** allPixels = flattenAll(&flatspec);

        analyse_colormode(&flatspec, allPixels, guessIndexed);

        // See if we can do alpha compaction.
        if (flatspec.partial_transparency_mode != FlattenSpec::ALLOW_PARTIAL_TRANSPARENCY && !FULLALPHA(flatspec.default_pixel) && flatspec.out_color_mode != FlattenSpec::COLOR_INDEXED)
        {
            rgba unused = findUnusedColor(allPixels, flatspec.dim.width, flatspec.dim.height);
            if (unused && (flatspec.out_color_mode == FlattenSpec::COLOR_RGB || degrayPixel(unused) >= 0))
            {
                unused = NEWALPHA(unused, 0);
                for (uint32_t y = 0; y < flatspec.dim.height; y++)
                {
                    for (uint32_t x = 0; x < flatspec.dim.width; x++)
                    {
                        if (allPixels[y][x] == 0)
                        {
                            allPixels[y][x] = unused;
                        }
                    }
                }
                flatspec.default_pixel = unused;
            }
        }
        auto callback = selectCallback(flatspec.out_color_mode);
        shipoutWithCallback(&flatspec, allPixels, callback);
    }
    else
    {
        auto callback = selectCallback(flatspec.out_color_mode);
        flattenIncrementally(&flatspec, callback);
    }
    return true;

#endif
}
