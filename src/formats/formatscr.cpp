/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatscr.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <cstdint>

cFormatScr::cFormatScr(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatScr::~cFormatScr()
{
}

struct sPixelRGB
{
    struct sColor
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    void set(bool isSet, unsigned char attribute)
    {
        static sColor Colors[] =
        {
            // normal
            { 0, 0, 0 },
            { 0, 0, 192 },
            { 192, 0, 0 },
            { 192, 0, 192 },
            { 0, 192, 0 },
            { 0, 192, 192 },
            { 192, 192, 0 },
            { 192, 192, 192 },

            // bright
            { 0, 0, 0 },
            { 0, 0, 255 },
            { 255, 0, 0 },
            { 255, 0, 255 },
            { 0, 255, 0 },
            { 0, 255, 255 },
            { 255, 255, 0 },
            { 255, 255, 255 },
        };

        const unsigned bright = (attribute & 0x40) >> 3;
        if (!isSet)
        {
            attribute >>= 3;
        }
        color = Colors[bright + (attribute & 0x7)];
    }

    sColor color;
};

void putEightPixels(sPixelRGB* out, const unsigned char pixels, const unsigned char attribute)
{
    for (unsigned i = 0; i < 8; i++)
    {
        const unsigned bit = 0x80 >> i;
        const bool isSet = pixels & bit;
        out->set(isSet, attribute);
        out++;
    }
}

void fillThird(const unsigned char* zxPixels, const unsigned char* zxColors, sPixelRGB* out)
{
    for (unsigned y = 0; y < 64; y++)
    {
        const unsigned line = (y * 8) % 64 + (y * 8) / 64;
        sPixelRGB* startLine = &out[line * 256];
        for (unsigned x = 0; x < 256 / 8; x++)
        {
            const unsigned char pixels = *zxPixels;
            const unsigned char attribute = zxColors[(y % 8) * 32 + x];
            putEightPixels(&startLine[x * 8], pixels, attribute);
            zxPixels++;
        }
    }
}

bool cFormatScr::Load(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    std::vector<unsigned char> scr(6912);
    if (scr.size() != file.read(&scr[0], scr.size()))
    {
        printf("invalid size of ZX-Spectrum screen\n");
        return false;
    }

    desc.width  = 256;
    desc.height = 192;

    desc.pitch = desc.width * 3;
    desc.bitmap.resize(desc.pitch * desc.height);

    for (unsigned i = 0; i < 3; i++)
    {
        const unsigned char* zxPixels = &scr[2048 * i];
        const unsigned char* zxColors = &scr[6144 + 256 * i];
        sPixelRGB* out = (sPixelRGB*)&desc.bitmap[desc.pitch * 64 * i];
        fillThird(zxPixels, zxColors, out);
    }

    desc.bppImage = 1;

    desc.bpp = 24;
    desc.format = GL_RGB;

    return true;
}
