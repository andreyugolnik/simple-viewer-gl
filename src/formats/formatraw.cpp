/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatraw.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"
#include "../common/helpers.h"
#include "rle.h"

#include <cstring>

static const char Id[] = { 'R', 'A', 'W', 'I' };

enum eFormat
{
    FORMAT_UNKNOWN,
    FORMAT_RGB,
    FORMAT_RGBA,
    FORMAT_RGB_RLE,
    FORMAT_RGBA_RLE,
    FORMAT_RGB_RLE4,
    FORMAT_RGBA_RLE4
};

struct sHeader
{
    unsigned id;
    unsigned w;
    unsigned h;
    unsigned format;
    unsigned data_size;
};

static bool isValidFormat(const sHeader& header, unsigned file_size)
{
    if (header.data_size + sizeof(sHeader) == file_size)
    {
        const char* id = (const char*)&header.id;
        return (id[0] == Id[0] && id[1] == Id[1] && id[2] == Id[2] && id[3] == Id[3]);
    }
    return false;
}



cFormatRaw::cFormatRaw(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatRaw::~cFormatRaw()
{
}

bool cFormatRaw::isSupported(cFile& file, Buffer& buffer) const
{
    if (!helpers::readBuffer(file, buffer, sizeof(sHeader)))
    {
        return false;
    }

    const sHeader& header = *(sHeader*)&buffer[0];
    return isValidFormat(header, file.getSize());
}

//bool cFormatRaw::isRawFormat(const char* name)
//{
//cFile file;
//if(!file.open(name))
//{
//return false;
//}

//sHeader header;
//if(sizeof(header) != file.read(&header, sizeof(header)))
//{
//return false;
//}

//return isValidFormat(header, file.getSize());
//}

bool cFormatRaw::Load(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    sHeader header;
    if (sizeof(header) != file.read(&header, sizeof(header)))
    {
        printf("not valid RAW format\n");
        return false;
    }

    if (!isValidFormat(header, desc.size))
    {
        return false;
    }

    bool rle = true;
    unsigned bytespp = 0;
    switch (header.format)
    {
    case FORMAT_RGB:
        bytespp = 3;
        rle = false;
        break;
    case FORMAT_RGBA:
        bytespp = 4;
        rle = false;
        break;
    case FORMAT_RGB_RLE:
    case FORMAT_RGB_RLE4:
        bytespp = 3;
        break;
    case FORMAT_RGBA_RLE:
    case FORMAT_RGBA_RLE4:
        bytespp = 4;
        break;
    default:
        printf("unknown RAW format\n");
        return false;
    }
    desc.bpp = desc.bppImage = bytespp * 8;
    desc.format = (bytespp == 3 ? GL_RGB : GL_RGBA);
    desc.width = header.w;
    desc.height = header.h;
    desc.pitch = desc.width * bytespp;
    desc.bitmap.resize(desc.pitch * desc.height);

    desc.info = "AGE RAW format";

    if (rle)
    {
        std::vector<unsigned char> rle(header.data_size);
        if (header.data_size != file.read(&rle[0], header.data_size))
        {
            return false;
        }

        updateProgress(0.5f);

        cRLE decoder;
        unsigned decoded = 0;
        if (header.format == FORMAT_RGB_RLE4 || header.format == FORMAT_RGBA_RLE4)
        {
            decoded = decoder.decodeBy4((unsigned*)&rle[0], rle.size() / 4, (unsigned*)&desc.bitmap[0], desc.bitmap.size() / 4);
        }
        else
        {
            decoded = decoder.decode(&rle[0], rle.size(), &desc.bitmap[0], desc.bitmap.size());
        }
        if (!decoded)
        {
            printf("error decode RLE\n");
            return false;
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
