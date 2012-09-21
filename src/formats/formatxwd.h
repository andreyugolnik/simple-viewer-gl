////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// 'WE' Group
// http://www.ugolnik.info
// andrey@ugolnik.info
//
////////////////////////////////////////////////

#ifndef FORMATXWD_H
#define FORMATXWD_H

#include "format.h"

typedef struct _X11WindowDump
{
    uint32_t HeaderSize;     /* Size of the header in bytes */
    uint32_t FileVersion;    /* X11WD file version (always 07h) */
    uint32_t PixmapFormat;   /* Pixmap format */
    uint32_t PixmapDepth;    /* Pixmap depth in pixels */
    uint32_t PixmapWidth;    /* Pixmap width in pixels */
    uint32_t PixmapHeight;   /* Pixmap height in pixels */
    uint32_t XOffset;        /* Bitmap X offset */
    uint32_t ByteOrder;      /* Byte order of image data */
    uint32_t BitmapUnit;     /* Bitmap base data size */
    uint32_t BitmapBitOrder; /* Bit-order of image data */
    uint32_t BitmapPad;      /* Bitmap scan-line pad*/
    uint32_t BitsPerPixel;   /* Bits per pixel */
    uint32_t BytesPerLine;   /* Bytes per scan-line */
    uint32_t VisualClass;    /* Class of the image */
    uint32_t RedMask;        /* Red mask */
    uint32_t GreenMask;      /* Green mask */
    uint32_t BlueMask;       /* Blue mask */
    uint32_t BitsPerRgb;     /* Size of each color mask in bits */
    uint32_t NumberOfColors;  /* Number of colors in image */
    uint32_t ColorMapEntries; /* Number of entries in color map */
    uint32_t WindowWidth;    /* Window width */
    uint32_t WindowHeight;   /* Window height */
    uint32_t WindowX;        /* Window upper left X coordinate */
    uint32_t WindowY;        /* Window upper left Y coordinate */
    uint32_t WindowBorderWidth; /* Window border width */
} X11WINDOWDUMP;

typedef struct _X11ColorMap
{
    uint32_t EntryNumber;    /* Number of the color map entry */
    uint16_t Red;            /* Red-channel value */
    uint16_t Green;          /* Green-channel value */
    uint16_t Blue;           /* Blue-channel value */
    uint8_t Flags;          /* Flag for this entry */
    uint8_t Padding;        /* WORD-align padding */
} X11COLORMAP;


class CFormatXwd : public CFormat
{
public:
	CFormatXwd(Callback callback, const char* _lib, const char* _name);
	virtual ~CFormatXwd();

	virtual bool Load(const char* filename, int subImage = 0);

private:
};

#endif // FORMATXWD_H

