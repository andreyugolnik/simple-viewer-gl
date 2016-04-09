/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatppm.h"
#include "file.h"

#include <cstring>
#include <iostream>

cFormatPpm::cFormatPpm(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

cFormatPpm::~cFormatPpm()
{
}

bool cFormatPpm::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    bool result = false;
    char* line = 0;
    size_t len = 0;
    ssize_t read;
    int format = 0;
    while((read = getline(&line, &len, (FILE*)file.getHandle())) != -1)
    {
        if(line[0] != '#')
        {
            if(read == 3 && line[0] == 'P')
            {
                // format
                format = line[1] - '0';
            }
            else
            {
                int w = 0;
                int h = 0;
                if(2 == sscanf(line, "%d %d\n", &w, &h))
                {
                    m_width = w;
                    m_height = h;
                }
                else
                {
                    sscanf(line, "%d\n", &w);
                    m_width = w;

                    if(getline(&line, &len, (FILE*)file.getHandle()) != -1)
                    {
                        sscanf(line, "%d\n", &h);
                        m_height = h;
                    }
                }

                if(getline(&line, &len, (FILE*)file.getHandle()) != -1)
                {
                    int max_val;
                    sscanf(line, "%d\n", &max_val);
                }

                switch(format)
                {
                case 1: // 1-ascii
                    result = readAscii1(file, w, h);
                    break;
                case 4: // 1-raw
                    result = readRaw1(file, w, h);
                    break;

                case 2: // 8-ascii
                    result = readAscii8(file, w, h);
                    break;
                case 5: // 8-raw
                    result = readRaw8(file, w, h);
                    break;

                case 3: // 24-ascii
                    result = readAscii24(file, w, h);
                    break;
                case 6: // 24-raw
                    result = readRaw24(file, w, h);
                    break;
                }

                break;
            }
        }
    }
    free(line);

    return result;
}

bool cFormatPpm::readAscii1(cFile& file, int w, int h)
{
    return false;
}

bool cFormatPpm::readRaw1(cFile& file, int w, int h)
{
    return false;
}

bool cFormatPpm::readAscii8(cFile& file, int w, int h)
{
    return false;
}

bool cFormatPpm::readRaw8(cFile& file, int w, int h)
{
    return false;
}

bool cFormatPpm::readAscii24(cFile& file, int w, int h)
{
    const int bpp = 24;
    m_bpp = m_bppImage = bpp;
    m_pitch = w * bpp / 8;
    const size_t components = h * m_pitch;
    m_bitmap.resize(components);

    size_t idx = 0;
    char* line = 0;
    size_t len = 0;
    while(getline(&line, &len, (FILE*)file.getHandle()) != -1)
    {
        const int val = atoi(line);
        m_bitmap[idx++] = val;
    }
    return idx == m_bitmap.size();
}

bool cFormatPpm::readRaw24(cFile& file, int w, int h)
{
    const int bpp = 24;
    m_bpp = m_bppImage = bpp;
    m_pitch = w * bpp / 8;
    const size_t components = h * m_pitch;
    m_bitmap.resize(components);

    return components == file.read(&m_bitmap[0], components);
}

