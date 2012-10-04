 /**********************************************
 *
 *  Andrey A. Ugolnik
 *  'WE' Group
 *  http://www.ugolnik.info
 *  andrey@ugolnik.info
 *
 *  created: 04.10.2012
 *  changed: 05.09.2012
 *  version: 0.0.0.54
 *
 ***********************************************/

#include "formatraw.h"

#include <string.h>

static const char Id[] = { 'R', 'A', 'W', 'I' };

cFormatRaw::cFormatRaw(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

cFormatRaw::~cFormatRaw()
{
}

bool cFormatRaw::IsValidFormat(const char* _name)
{
    if(openFile(_name) == false)
    {
        return false;
    }

    sHeader header;
    size_t size = fread(&header, sizeof(header), 1, m_file);
    bool valid = isValidFormat(&header);
    reset();
    return (size == 1 && valid);
}

bool cFormatRaw::isValidFormat(const sHeader* _header)
{
    if(_header->w * _header->h * _header->format + sizeof(sHeader) == m_size)
    {
        return (memcmp(&_header->id, Id, sizeof(Id)) == 0);
    }
    return false;
}

bool cFormatRaw::Load(const char* filename, int subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }

    sHeader header;
    if(1 != fread(&header, sizeof(header), 1, m_file))
    {
        std::cout << "not valid RAW format" << std::endl;
        reset();
        return false;
    }

    if(!isValidFormat(&header))
    {
        reset();
        return false;
    }

    m_bpp = (header.format == 3 ? 24 : 32);
    m_bppImage = m_bpp;
    m_format = (header.format == 3 ? GL_RGB : GL_RGBA);
    m_width = header.w;
    m_height = header.h;
    m_pitch = m_width * m_bpp / 8;
    m_bitmap.resize(m_pitch * m_height);

    m_info = "'WE' Group RAW format";

    for(unsigned y = 0; y < m_height; y++)
    {
        if(1 != fread(&m_bitmap[y * m_pitch], m_pitch, 1, m_file))
        {
            reset();
            return false;
        }
        int percent = (int)(100.0f * y / m_height);
        progress(percent);
    }

    fclose(m_file);

    return true;
}

