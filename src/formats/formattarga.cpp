/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formattarga.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

cFormatTarga::cFormatTarga(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatTarga::~cFormatTarga()
{
}

namespace
{

#pragma pack(push, 1)
    struct sTARGAHeader
    {
        unsigned char idLength;
        unsigned char colorMapType;
        unsigned char imageType;

        unsigned short firstEntryIndex;
        unsigned short colorMapLength;
        unsigned char colorMapEntrySize;

        unsigned short xOrigin;
        unsigned short yOrigin;
        unsigned short width;
        unsigned short height;
        unsigned char pixelDepth;
        unsigned char imageDescriptor;
    };
#pragma pack(pop)

    bool colormapped(const sTARGAHeader& header, const unsigned char* tga, sBitmapDescription& desc)
    {
        // Uncompressed Color-mapped Image
        if (header.imageType == 1)
        {
            if (header.pixelDepth == 8)
            {
                // printf("  INFO: uncompressed 8 bit\n");

                if (header.colorMapEntrySize == 24)
                {
                    desc.bppImage = 8;
                    desc.bpp = 24;
                    desc.pitch = desc.width * 3;
                    desc.bitmap.resize(desc.pitch * desc.height);

                    unsigned tgaPitch = header.width;
                    auto cmdData = tga + sizeof(sTARGAHeader) + header.idLength;
                    unsigned cmtWidth  = header.colorMapEntrySize / 8;
                    tga += header.colorMapLength * cmtWidth;

                    for (unsigned y = 0; y < header.height; y++)
                    {
                        unsigned dp = (header.height - y - 1) * desc.pitch;
                        unsigned sp = (header.height - y - 1) * tgaPitch;
                        for (unsigned x = 0; x < header.width; x++)
                        {
                            desc.bitmap[dp + 0] = cmdData[tga[sp] * cmtWidth + 0];
                            desc.bitmap[dp + 1] = cmdData[tga[sp] * cmtWidth + 1];
                            desc.bitmap[dp + 2] = cmdData[tga[sp] * cmtWidth + 2];
                            dp += 3;
                            sp++;
                        }
                    }

                    return true;
                }
                else
                {
                    printf("(EE) uncompressed 8 bit with non 24 bit color map entry size currently not supported\n");
                }
            }
            else
            {
                printf("(EE) non 8 bit color-mapped format\n");
            }

            return false;
        }
        // Compressed Color-mapped Image
        else if (header.imageType == 9)
        {
            printf("(EE) compressed 8 bit currently not supported\n");
            return false;
        }
        else
        {
            printf("(EE) unknown color-mapped format\n");
            return false;
        }
    }

    bool rgbUncompressed(const sTARGAHeader& header, const unsigned char* tga, sBitmapDescription& desc)
    {
        if (header.pixelDepth == 16)
        {
            // printf("  INFO: uncompressed 16 bit\n");
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);

            unsigned tgaPitch = header.width * 2;

            for (unsigned y = 0; y < header.height; y++)
            {
                unsigned dp = (header.height - y - 1) * desc.pitch;
                unsigned sp = (header.height - y - 1) * tgaPitch;
                for (unsigned x = 0; x < header.width; x++)
                {
                    auto c = *(unsigned short*)&tga[sp];
                    desc.bitmap[dp + 0] = (((c >>  0) & 31) * 255) / 31;
                    desc.bitmap[dp + 1] = (((c >>  5) & 31) * 255) / 31;
                    desc.bitmap[dp + 2] = (((c >> 10) & 31) * 255) / 31;
                    dp += 3;
                    sp += 2;
                }
            }
        }
        else if (header.pixelDepth == 24)
        {
            // printf("  INFO: uncompressed 24 bit\n");
            unsigned pitch = header.width * 3;

            desc.bppImage = 24;
            desc.bpp = 24;
            desc.pitch = pitch;
            desc.bitmap.resize(desc.pitch * desc.height);

            for (unsigned y = 0; y < header.height; y++)
            {
                unsigned idx = (header.height - y - 1) * pitch;
                for (unsigned x = 0; x < header.width; x++)
                {
                    desc.bitmap[idx + 0] = tga[idx + 0];
                    desc.bitmap[idx + 1] = tga[idx + 1];
                    desc.bitmap[idx + 2] = tga[idx + 2];
                    idx += 3;
                }
            }
        }
        else if (header.pixelDepth == 32)
        {
            // printf("  INFO: uncompressed 32 bit\n");
            unsigned pitch = header.width * 4;

            desc.bpp = 32;
            desc.bppImage = 32;
            desc.pitch = pitch;
            desc.bitmap.resize(desc.pitch * desc.height);

            for (unsigned y = 0; y < header.height; y++)
            {
                unsigned idx = (header.height - y - 1) * pitch;
                for (unsigned x = 0; x < header.width; x++)
                {
                    desc.bitmap[idx + 0] = tga[idx + 0];
                    desc.bitmap[idx + 1] = tga[idx + 1];
                    desc.bitmap[idx + 2] = tga[idx + 2];
                    desc.bitmap[idx + 3] = tga[idx + 3];
                    idx += 4;
                }
            }
        }
        else
        {
            printf("(EE) unsupported uncompressed RGB format\n");
            return false;
        }

        return true;
    }

    bool rgbCompressed(const sTARGAHeader& header, const unsigned char* tga, sBitmapDescription& desc)
    {
        unsigned dwWidth  = 0;
        unsigned dwHeight = 0;
        unsigned dp       = 0;
        unsigned sp       = 0;

        if (header.pixelDepth == 16)
        {
            // printf("  INFO: compressed 16 bit\n");
            desc.bppImage = 16;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);

            while (dwHeight < header.height)
            {
                unsigned char cunkHead = tga[sp++];
                unsigned char isPacked = cunkHead & 128;
                unsigned char count    = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        auto c = *(unsigned short*)&tga[sp];
                        desc.bitmap[dp + 0] = (unsigned char)(((c >>  0) & 31) * 255) / 31;
                        desc.bitmap[dp + 1] = (unsigned char)(((c >>  5) & 31) * 255) / 31;
                        desc.bitmap[dp + 2] = (unsigned char)(((c >> 10) & 31) * 255) / 31;
                        dp += 3;
                        sp += 2;
                        dwWidth++;
                    }
                }
                else
                {
                    auto c = *(unsigned short*)&tga[sp];
                    auto r = (unsigned char)(((c >>  0) & 31) * 255) / 31;
                    auto g = (unsigned char)(((c >>  5) & 31) * 255) / 31;
                    auto b = (unsigned char)(((c >> 10) & 31) * 255) / 31;
                    sp += 2;

                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        desc.bitmap[dp + 0] = r;
                        desc.bitmap[dp + 1] = g;
                        desc.bitmap[dp + 2] = b;
                        dp += 3;
                        dwWidth++;
                    }
                }
            }
        }
        else if (header.pixelDepth == 24)
        {
            // printf("  INFO: compressed 24 bit\n");
            desc.bppImage = 24;
            desc.bpp = 24;
            desc.pitch = desc.width * 3;
            desc.bitmap.resize(desc.pitch * desc.height);

            while (dwHeight < header.height)
            {
                unsigned char cunkHead = tga[sp++];
                unsigned char isPacked  = cunkHead & 128;
                unsigned char count     = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp  += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        desc.bitmap[dp + 0] = tga[sp + 0];
                        desc.bitmap[dp + 1] = tga[sp + 1];
                        desc.bitmap[dp + 2] = tga[sp + 2];
                        dp += 3;
                        sp += 3;
                        dwWidth++;
                    }
                }
                else
                {
                    unsigned char r = tga[sp + 0];
                    unsigned char g = tga[sp + 1];
                    unsigned char b = tga[sp + 2];
                    sp += 3;
                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        desc.bitmap[dp + 0] = r;
                        desc.bitmap[dp + 1] = g;
                        desc.bitmap[dp + 2] = b;
                        dp += 3;
                        dwWidth++;
                    }
                }
            }
        }
        else if (header.pixelDepth == 32)
        {
            // printf("  INFO: compressed 32 bit\n");
            desc.bppImage = 32;
            desc.bpp = 32;
            desc.pitch = desc.width * 4;
            desc.bitmap.resize(desc.pitch * desc.height);

            while (dwHeight < header.height)
            {
                unsigned char cunkHead = tga[sp++];
                unsigned char isPacked  = cunkHead & 128;
                unsigned char count     = (cunkHead & 127) + 1;

                if (isPacked == 0)
                {
                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        desc.bitmap[dp + 0] = tga[sp + 0];
                        desc.bitmap[dp + 1] = tga[sp + 1];
                        desc.bitmap[dp + 2] = tga[sp + 2];
                        desc.bitmap[dp + 3] = tga[sp + 3];
                        dp += 4;
                        sp += 4;
                        dwWidth++;
                    }
                }
                else
                {
                    unsigned char r = tga[sp + 0];
                    unsigned char g = tga[sp + 1];
                    unsigned char b = tga[sp + 2];
                    unsigned char a = tga[sp + 3];
                    sp += 4;

                    for (unsigned x = 0; x < count; x++)
                    {
                        if (dwWidth == header.width)
                        {
                            dwWidth = 0;
                            dwHeight++;
                            dp  += ((4 - ((header.width % 4) == 0 ? 4 : header.width % 4)) * 3);
                            if (dwHeight == header.height)
                            {
                                break;
                            }
                        }
                        desc.bitmap[dp + 0] = r;
                        desc.bitmap[dp + 1] = g;
                        desc.bitmap[dp + 2] = b;
                        desc.bitmap[dp + 3] = a;
                        dp += 4;
                        dwWidth++;
                    }
                }
            }
        }
        else
        {
            printf("(EE) unsupported compressed RGB format\n");
            return false;
        }

        return true;
    }

}

bool cFormatTarga::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    std::vector<unsigned char> tga(desc.size);
    if (tga.size() != file.read(tga.data(), tga.size()))
    {
        printf("(EE) Can't read TARGA data.\n");
        return false;
    }

    auto& header = *reinterpret_cast<const sTARGAHeader*>(tga.data());

    desc.width = header.width;
    desc.height = header.height;

    if (header.colorMapType == 1)
    {
        colormapped(header, tga.data(), desc);
    }
    else if (header.colorMapType == 0)
    {
        // RGB - uncompressed
        if (header.imageType == 2)
        {
            rgbUncompressed(header, tga.data(), desc);
        }
        // RGB - compressed
        else if (header.imageType == 10)
        {
            rgbCompressed(header, tga.data(), desc);
        }
        else
        {
            printf("(EE) unknown image type, may be it black and white\n");
            return false;
        }
    }
    else
    {
        printf("(EE) unknown color map type\n");
        return false;
    }

    desc.format = desc.bpp == 32 ? GL_RGBA : GL_RGB;

    return true;
}
