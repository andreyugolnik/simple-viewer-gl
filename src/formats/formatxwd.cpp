/**********************************************\
*
*  Andrey A. Ugolnik
*  Tiny Orange
*  http://www.tinyorange.com
*  andrey@ugolnik.info
*
*  created: 21.09.2012
*
\**********************************************/

#include "formatxwd.h"
#include <string.h>

struct sXwdCommon
{
    uint32_t HeaderSize;        /* Header size in bytes */
    uint32_t FileVersion;       /* X10 XWD file version (always 06h) */
                                /* X11 XWD file version (always 07h) */
};

struct X10WindowDump : sXwdCommon
{
    uint32_t DisplayType;       /* Display type */
    uint32_t DisplayPlanes;     /* Number of display planes */
    uint32_t PixmapFormat;      /* Pixmap format */
    uint32_t PixmapWidth;       /* Pixmap width */
    uint32_t PixmapHeight;      /* Pixmap height */
    uint16_t WindowWidth;       /* Window width */
    uint16_t WindowHeight;      /* Window height */
    uint16_t WindowX;           /* Window upper left X coordinate */
    uint16_t WindowY;           /* Window upper left Y coordinate */
    uint16_t WindowBorderWidth; /* Window border width */
    uint16_t WindowNumColors;   /* Number of color entries in window */
};

struct X11WindowDump : sXwdCommon
{
    uint32_t PixmapFormat;      /* Pixmap format */
    uint32_t PixmapDepth;       /* Pixmap depth in pixels */
    uint32_t PixmapWidth;       /* Pixmap width in pixels */
    uint32_t PixmapHeight;      /* Pixmap height in pixels */
    uint32_t XOffset;           /* Bitmap X offset */
    uint32_t ByteOrder;         /* Byte order of image data */
    uint32_t BitmapUnit;        /* Bitmap base data size */
    uint32_t BitmapBitOrder;    /* Bit-order of image data */
    uint32_t BitmapPad;         /* Bitmap scan-line pad*/
    uint32_t BitsPerPixel;      /* Bits per pixel */
    uint32_t BytesPerLine;      /* Bytes per scan-line */
    uint32_t VisualClass;       /* Class of the image */
    uint32_t RedMask;           /* Red mask */
    uint32_t GreenMask;         /* Green mask */
    uint32_t BlueMask;          /* Blue mask */
    uint32_t BitsPerRgb;        /* Size of each color mask in bits */
    uint32_t NumberOfColors;    /* Number of colors in image */
    uint32_t ColorMapEntries;   /* Number of entries in color map */
    uint32_t WindowWidth;       /* Window width */
    uint32_t WindowHeight;      /* Window height */
    uint32_t WindowX;           /* Window upper left X coordinate */
    uint32_t WindowY;           /* Window upper left Y coordinate */
    uint32_t WindowBorderWidth; /* Window border width */
};

struct X10ColorMap
{
    uint16_t EntryNumber; /* Number of the color-map entry */
    uint16_t Red;         /* Red-channel value */
    uint16_t Green;       /* Green-channel value */
    uint16_t Blue;        /* Blue-channel value */
};

struct X11ColorMap
{
    uint32_t EntryNumber; /* Number of the color map entry */
    uint16_t Red;         /* Red-channel value */
    uint16_t Green;       /* Green-channel value */
    uint16_t Blue;        /* Blue-channel value */
    uint8_t Flags;        /* Flag for this entry */
    uint8_t Padding;      /* WORD-align padding */
};


CFormatXwd::CFormatXwd(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

CFormatXwd::~CFormatXwd()
{
}

bool CFormatXwd::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    sXwdCommon common;
    if(sizeof(common) != file.read(&common, sizeof(common)))
    {
        printf("Can't read XWD header\n");
        return false;
    }

    swap_uint32s((uint8_t*)&common, sizeof(common));
    printf("\n");
    printf(" HeaderSize: %u\n" , common.HeaderSize);
    printf(" FileVersion: %u\n", common.FileVersion);
    printf("\n");

    file.seek(0, SEEK_SET);

    if(common.HeaderSize == sizeof(X10WindowDump) && common.FileVersion == 0x06)
    {
        X10WindowDump header;
        if(sizeof(header) != file.read(&header, sizeof(header)))
        {
            printf("Can't read XWD header\n");
            return false;
        }
        swap_uint32s((uint8_t*)&header, sizeof(header));

        printf(" PixmapFormat: %u\n"      , header.PixmapFormat);
        printf(" DisplayType: %u\n"       , header.DisplayType);
        printf(" DisplayPlanes: %u\n"     , header.DisplayPlanes);
        printf(" PixmapFormat: %u\n"      , header.PixmapFormat);
        printf(" PixmapWidth: %u\n"       , header.PixmapWidth);
        printf(" PixmapHeight: %u\n"      , header.PixmapHeight);
        printf(" WindowWidth: %u\n"       , header.WindowWidth);
        printf(" WindowHeight: %u\n"      , header.WindowHeight);
        printf(" WindowX: %u\n"           , header.WindowX);
        printf(" WindowY: %u\n"           , header.WindowY);
        printf(" WindowBorderWidth: %u\n" , header.WindowBorderWidth);
        printf(" WindowNumColors: %u\n"   , header.WindowNumColors);

        return loadX10(header, file);
    }
    else if(common.HeaderSize == sizeof(X11WindowDump) && common.FileVersion == 0x07)
    {
        X11WindowDump header;
        if(sizeof(header) != file.read(&header, sizeof(header)))
        {
            printf("Can't read XWD header\n");
            return false;
        }
        swap_uint32s((uint8_t*)&header, sizeof(header));

        printf(" PixmapFormat: %u\n"      , header.PixmapFormat);
        printf(" PixmapDepth: %u\n"       , header.PixmapDepth);
        printf(" PixmapWidth: %u\n"       , header.PixmapWidth);
        printf(" PixmapHeight: %u\n"      , header.PixmapHeight);
        printf(" XOffset: %u\n"           , header.XOffset);
        printf(" ByteOrder: %u\n"         , header.ByteOrder);
        printf(" BitmapUnit: %u\n"        , header.BitmapUnit);
        printf(" BitmapBitOrder: %u\n"    , header.BitmapBitOrder);
        printf(" BitmapPad: %u\n"         , header.BitmapPad);
        printf(" BitsPerPixel: %u\n"      , header.BitsPerPixel);
        printf(" BytesPerLine: %u\n"      , header.BytesPerLine);
        printf(" VisualClass: %u\n"       , header.VisualClass);
        printf(" RedMask: %u\n"           , header.RedMask);
        printf(" GreenMask: %u\n"         , header.GreenMask);
        printf(" BlueMask: %u\n"          , header.BlueMask);
        printf(" BitsPerRgb: %u\n"        , header.BitsPerRgb);
        printf(" NumberOfColors: %u\n"    , header.NumberOfColors);
        printf(" ColorMapEntries: %u\n"   , header.ColorMapEntries);
        printf(" WindowWidth: %u\n"       , header.WindowWidth);
        printf(" WindowHeight: %u\n"      , header.WindowHeight);
        printf(" WindowX: %u\n"           , header.WindowX);
        printf(" WindowY: %u\n"           , header.WindowY);
        printf(" WindowBorderWidth: %u\n" , header.WindowBorderWidth);

        return loadX11(header, file);
    }

    printf("Invalid XWD file version\n");
    return false;
}

bool CFormatXwd::loadX10(const X10WindowDump& header, cFile& file)
{
    return false;
}

bool CFormatXwd::loadX11(const X11WindowDump& header, cFile& file)
{
    std::vector<X11ColorMap> colors(header.ColorMapEntries);
    const unsigned color_map_size = sizeof(X11ColorMap) * header.ColorMapEntries;
    if(color_map_size != file.read(&colors[0], color_map_size))
    {
        printf("Can't read colormap\n");
        return false;
    }

    m_size     = file.getSize();
    m_width    = header.PixmapWidth;
    m_height   = header.PixmapHeight;
    m_bpp      = header.BitsPerPixel;
    m_bppImage = header.BitsPerPixel;
    m_pitch    = header.BytesPerLine;
    m_format   = GL_RGB;
    m_bitmap.resize(m_pitch * m_height);

    if(m_bitmap.size() != file.read(&m_bitmap[0], m_bitmap.size()))
    {
        printf("Can't read pixmap\n");
        return false;
    }

    return true;
}

