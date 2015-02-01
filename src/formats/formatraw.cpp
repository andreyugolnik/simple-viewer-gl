 /**********************************************
 *
 *  Andrey A. Ugolnik
 *  http://www.ugolnik.info
 *  andrey@ugolnik.info
 *
 ***********************************************/

#include "formatraw.h"
#include "rle.h"

#include <string.h>

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

cFormatRaw::cFormatRaw(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

cFormatRaw::~cFormatRaw()
{
}

bool cFormatRaw::isRawFormat(const char* name)
{
    cFile file;
    if(!file.open(name))
    {
        return false;
    }

    sHeader header;
    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        return false;
    }

    return isValidFormat(header, file.getSize());
}

bool cFormatRaw::isValidFormat(const sHeader& header, unsigned file_size)
{
    if(header.data_size + sizeof(sHeader) == file_size)
    {
        const char* id = (const char*)&header.id;
        return (id[0] == Id[0] && id[1] == Id[1] && id[2] == Id[2] && id[3] == Id[3]);
    }
    return false;
}

bool cFormatRaw::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    sHeader header;
    if(sizeof(header) != file.read(&header, sizeof(header)))
    {
        std::cout << "not valid RAW format" << std::endl;
        return false;
    }

    if(!isValidFormat(header, m_size))
    {
        return false;
    }

    bool rle = true;
    unsigned bytespp = 0;
    switch(header.format)
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
        std::cout << "unknown RAW format" << std::endl;
        return false;
    }
    m_bpp = m_bppImage = bytespp * 8;
    m_format = (bytespp == 3 ? GL_RGB : GL_RGBA);
    m_width = header.w;
    m_height = header.h;
    m_pitch = m_width * bytespp;
    m_bitmap.resize(m_pitch * m_height);

    m_info = "'WE' Group RAW format";

    if(rle)
    {
        std::vector<unsigned char> rle(header.data_size);
        if(header.data_size != file.read(&rle[0], header.data_size))
        {
            return false;
        }

        progress(50);

        cRLE decoder;
        unsigned decoded = 0;
        if(header.format == FORMAT_RGB_RLE4 || header.format == FORMAT_RGBA_RLE4)
        {
            decoded = decoder.decodeBy4((unsigned*)&rle[0], rle.size() / 4, (unsigned*)&m_bitmap[0], m_bitmap.size() / 4);
        }
        else
        {
            decoded = decoder.decode(&rle[0], rle.size(), &m_bitmap[0], m_bitmap.size());
        }
        if(!decoded)
        {
            std::cout << "error decode RLE" << std::endl;
            return false;
        }

        progress(100);
    }
    else
    {
        for(unsigned y = 0; y < m_height; y++)
        {
            if(m_pitch != file.read(&m_bitmap[y * m_pitch], m_pitch))
            {
                return false;
            }
            int percent = (int)(100.0f * y / m_height);
            progress(percent);
        }
    }

    return true;
}

