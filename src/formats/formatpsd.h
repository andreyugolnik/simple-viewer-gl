/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATPSD_H
#define FORMATPSD_H

#include "format.h"

namespace FORMAT_PSD
{
#pragma pack(push, 1)
    typedef struct PSD_HEADER
    {
        uint8_t signature[4];   /* file ID, always "8BPS" */
        uint16_t version;       /* version number, always 1 */
        uint8_t resetved[6];
        uint16_t channels;      /* number of color channels (1-24) */
        uint32_t rows;          /* height of image in pixels (1-30000) */
        uint32_t columns;       /* width of image in pixels (1-30000) */
        uint16_t depth;         /* number of bits per channel (1, 8, and 16) */
        uint16_t color_mode;    /* color mode as defined below */
    } PsdHeader;
#pragma pack(pop)

    typedef enum COLOR_MODE
    {
        PSD_MODE_MONO			= 0,
        PSD_MODE_GRAYSCALE		= 1,
        PSD_MODE_INDEXED		= 2,
        PSD_MODE_RGB			= 3,
        PSD_MODE_CMYK			= 4,
        PSD_MODE_MULTICHANNEL	= 7,
        PSD_MODE_DUOTONE		= 8,
        PSD_MODE_LAB			= 9,
    } ColorMode;
};

class CFormatPsd : public CFormat
{
public:
    CFormatPsd(Callback callback);
    virtual ~CFormatPsd();

    virtual bool Load(const char* filename, int subImage = 0);

private:
    uint8_t* m_buffer;
    uint16_t* m_linesLengths;
#define MAX_CHANNELS	24
    uint8_t* m_chBufs[MAX_CHANNELS];

private:
    bool skipNextBlock();
    void decompressLine(const uint8_t* src, uint32_t line_length, uint8_t* dest);
    void cleanup();
};

#endif // FORMATPSD_H

