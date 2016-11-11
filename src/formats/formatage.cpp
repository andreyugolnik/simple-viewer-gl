/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatage.h"
#include "../common/ZlibDecoder.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"
#include "../common/helpers.h"
#include "AGEheader.h"
#include "rle.h"

#include <cstring>

static bool isValidFormat(const AGE::Header& header, unsigned file_size)
{
    if (header.data_size + sizeof(AGE::Header) == file_size)
    {
        return AGE::isRawHeader(header);
    }
    return false;
}



cFormatAge::cFormatAge(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatAge::~cFormatAge()
{
}

bool cFormatAge::isSupported(cFile& file, Buffer& buffer) const
{
    if (!helpers::readBuffer(file, buffer, sizeof(AGE::Header)))
    {
        return false;
    }

    auto header = (const AGE::Header*)&buffer[0];
    return isValidFormat(*header, file.getSize());
}

//bool cFormatAge::isRawFormat(const char* name)
//{
//cFile file;
//if(!file.open(name))
//{
//return false;
//}

//Header header;
//if(sizeof(header) != file.read(&header, sizeof(header)))
//{
//return false;
//}

//return isValidFormat(header, file.getSize());
//}

bool cFormatAge::Load(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    AGE::Header header;
    if (sizeof(header) != file.read(&header, sizeof(header)))
    {
        printf("not valid AGE image format\n");
        return false;
    }

    if (!isValidFormat(header, desc.size))
    {
        return false;
    }

    unsigned bytespp = 0;
    switch (header.format)
    {
    case AGE::Format::ALPHA:
        bytespp = 1;
        desc.format = GL_ALPHA;
        break;
    case AGE::Format::RGB:
        bytespp = 3;
        desc.format = GL_RGB;
        break;
    case AGE::Format::RGBA:
        bytespp = 4;
        desc.format = GL_RGBA;
        break;
    default:
        printf("unknown AGE format\n");
        return false;
    }

    desc.bpp = desc.bppImage = bytespp * 8;
    desc.width = header.w;
    desc.height = header.h;
    desc.pitch = desc.width * bytespp;
    desc.bitmap.resize(desc.pitch * desc.height);

    desc.info = "AGE image format";

    if (header.compression != AGE::Compression::NONE)
    {
        std::vector<unsigned char> in(header.data_size);
        if (header.data_size != file.read(&in[0], header.data_size))
        {
            return false;
        }

        updateProgress(0.5f);

        unsigned decoded = 0;

        if (header.compression == AGE::Compression::ZLIB)
        {
            cZlibDecoder decoder;
            decoded = decoder.decode(&in[0], in.size(), &desc.bitmap[0], desc.bitmap.size());
            if (!decoded)
            {
                printf("error decode ZLIB\n");
                return false;
            }
        }
        else
        {
            cRLE decoder;
            if (header.compression == AGE::Compression::RLE4)
            {
                decoded = decoder.decodeBy4((unsigned*)&in[0], in.size() / 4, (unsigned*)&desc.bitmap[0], desc.bitmap.size() / 4);
            }
            else
            {
                decoded = decoder.decode(&in[0], in.size(), &desc.bitmap[0], desc.bitmap.size());
            }
            if (!decoded)
            {
                printf("error decode RLE\n");
                return false;
            }
        }

        updateProgress(1.0f);
    }
    else
    {
        for (unsigned y = 0; y < desc.height; y++)
        {
            if (desc.pitch != file.read(&desc.bitmap[y * desc.pitch], desc.pitch))
            {
                return false;
            }
            updateProgress((float)y / desc.height);
        }
    }

    return true;
}
