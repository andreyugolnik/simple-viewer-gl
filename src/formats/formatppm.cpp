/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatppm.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <iostream>

cFormatPpm::cFormatPpm(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

cFormatPpm::~cFormatPpm()
{
}

bool cFormatPpm::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

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
                    desc.width = w;
                    desc.height = h;
                }
                else
                {
                    sscanf(line, "%d\n", &w);
                    desc.width = w;

                    if(getline(&line, &len, (FILE*)file.getHandle()) != -1)
                    {
                        sscanf(line, "%d\n", &h);
                        desc.height = h;
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
                    result = readAscii1(file, w, h, desc);
                    break;
                case 4: // 1-raw
                    result = readRaw1(file, w, h, desc);
                    break;

                case 2: // 8-ascii
                    result = readAscii8(file, w, h, desc);
                    break;
                case 5: // 8-raw
                    result = readRaw8(file, w, h, desc);
                    break;

                case 3: // 24-ascii
                    result = readAscii24(file, w, h, desc);
                    break;
                case 6: // 24-raw
                    result = readRaw24(file, w, h, desc);
                    break;
                }

                break;
            }
        }
    }
    free(line);

    return result;
}

bool cFormatPpm::readAscii1(cFile& file, int w, int h, sBitmapDescription& desc)
{
    return false;
}

bool cFormatPpm::readRaw1(cFile& file, int w, int h, sBitmapDescription& desc)
{
    return false;
}

bool cFormatPpm::readAscii8(cFile& file, int w, int h, sBitmapDescription& desc)
{
    return false;
}

bool cFormatPpm::readRaw8(cFile& file, int w, int h, sBitmapDescription& desc)
{
    return false;
}

bool cFormatPpm::readAscii24(cFile& file, int w, int h, sBitmapDescription& desc)
{
    const int bpp = 24;
    desc.bpp = desc.bppImage = bpp;
    desc.pitch = w * bpp / 8;
    const size_t components = h * desc.pitch;
    desc.bitmap.resize(components);

    size_t idx = 0;
    char* line = 0;
    size_t len = 0;
    while(getline(&line, &len, (FILE*)file.getHandle()) != -1)
    {
        const int val = atoi(line);
        desc.bitmap[idx++] = val;
    }
    return idx == desc.bitmap.size();
}

bool cFormatPpm::readRaw24(cFile& file, int w, int h, sBitmapDescription& desc)
{
    const int bpp = 24;
    desc.bpp = desc.bppImage = bpp;
    desc.pitch = w * bpp / 8;
    const size_t components = h * desc.pitch;
    desc.bitmap.resize(components);

    return components == file.read(&desc.bitmap[0], components);
}

