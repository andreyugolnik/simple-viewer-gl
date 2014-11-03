/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatppm.h"
#include <string.h>
#include <iostream>

cFormatPpm::cFormatPpm(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

cFormatPpm::~cFormatPpm()
{
}

bool cFormatPpm::Load(const char* filename, unsigned /*subImage*/)
{
    if(openFile(filename) == false)
    {
        return false;
    }

    char* line = 0;
    size_t len = 0;
    ssize_t read;
    int format = 0;
    while((read = getline(&line, &len, m_file)) != -1)
    {
        if(read == 3 && line[0] == 'P')
        {
            // format
            format = line[1] - '0';
        }
        else if(line[0] == '#')
        {
            // comment
        }
        else
        {
            int w, h;
            sscanf(line, "%d %d\n", &w, &h);
            m_width = w;
            m_height = h;

            if(getline(&line, &len, m_file) != -1)
            {
                int max_val;
                sscanf(line, "%d\n", &max_val);
            }

            switch(format)
            {
            case 1: // 1-ascii
                readAscii(w, h, 1);
                break;
            case 4: // 1-raw
                readRaw(w, h, 1);
                break;

            case 2: // 8-ascii
                readAscii(w, h, 8);
                break;
            case 5: // 8-raw
                readRaw(w, h, 8);
                break;

            case 3: // 24-ascii
                readAscii(w, h, 24);
                break;
            case 6: // 24-raw
                readRaw(w, h, 24);
                break;
            }

            break;
        }
    }
    free(line);

    return true;
}

bool cFormatPpm::readAscii(int w, int h, int bpp)
{
    m_bpp = m_bppImage = bpp;
    m_pitch = w * bpp / 8;
    const size_t components = h * m_pitch;
    m_bitmap.resize(components);

    size_t idx = 0;
    char* line = 0;
    size_t len = 0;
    while(getline(&line, &len, m_file) != -1)
    {
        const int val = atoi(line);
        m_bitmap[idx++] = val;
    }
    return idx == m_bitmap.size();
}

bool cFormatPpm::readRaw(int w, int h, int bpp)
{
    m_bpp = m_bppImage = bpp;
    m_pitch = w * bpp / 8;
    const size_t components = h * m_pitch;
    m_bitmap.resize(components);

    return components == fread(&m_bitmap[0], 1, components, m_file);
}

