/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATICO_H
#define FORMATICO_H

#include "format.h"
#include <png.h>

namespace FORMAT_ICO
{
#pragma pack(push, 1)
    typedef struct ICO_HEADER {
        uint16_t reserved;	// Reserved. Should always be 0.
        uint16_t type;		// Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
        uint16_t count;		// Specifies number of images in the file.
    } IcoHeader;

    // List of icons.
    // Size = IcoHeader.ount * 16
    typedef struct ICO_DIRENTRY
    {
        uint8_t width;	// Specifies image width in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image width is 256 pixels.
        uint8_t height;	// Specifies image height in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image height is 256 pixels.
        uint8_t colors;	// Specifies number of colors in the color palette. Should be 0 if the image is truecolor.
        uint8_t reserved;	// Reserved. Should be 0.[Notes 1]
        uint16_t planes;	// In .ICO format: Specifies color planes. Should be 0 or 1.
        // In .CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left.
        uint16_t bits;	// In .ICO format: Specifies bits per pixel. (1, 4, 8)
        // In .CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top.
        uint32_t size;	// Specifies the size of the bitmap data in bytes. Size of (InfoHeader + ANDbitmap + XORbitmap)
        uint32_t offset;	// Specifies the offset of bitmap data address in the file
    } IcoDirentry;

    // Variant of BMP InfoHeader.
    // Size = 40 bytes.
    typedef struct ICO_BMP_INFOHEADER
    {
        uint32_t size;      // Size of InfoHeader structure = 40
        uint32_t width;     // Icon Width
        uint32_t height;    // Icon Height (added height of XOR-Bitmap and AND-Bitmap)
        uint16_t planes;    // number of planes = 1
        uint16_t bits;      // bits per pixel = 1, 2, 4, 8, 16, 24, 32
        uint32_t reserved0; // Type of Compression = 0
        uint32_t imagesize; // Size of Image in Bytes = 0 (uncompressed)
        uint32_t reserved1; // XpixelsPerM
        uint32_t reserved2; // YpixelsPerM
        uint32_t reserved3; // ColorsUsed
        uint32_t reserved4; // ColorsImportant
    } IcoBmpInfoHeader;

    // Color Map for XOR-Bitmap.
    // Size = NumberOfColors * 4 bytes.
    typedef struct ICO_COLORS
    {
        uint8_t red;		// red component
        uint8_t green;	// green component
        uint8_t blue;		// blue component
        uint8_t reserved;	// = 0
    } IcoColors;

    //		uint8_t xormask;	// DIB bits for XOR mask
    //		uint8_t andmask;	// DIB bits for AND mask
#pragma pack(pop)

    typedef struct PNG_RAW
    {
        uint8_t* data;
        size_t size;	// size of raw data
        size_t pos;	// current pos
    } PngRaw;
};

class CFormatIco : public CFormat
{
public:
    CFormatIco(Callback callback);
    virtual ~CFormatIco();

    virtual bool Load(const char* filename, int subImage = 0);

private:
    static FORMAT_ICO::PngRaw m_pngRaw;

private:
    bool loadOrdinaryFrame(const FORMAT_ICO::IcoDirentry* image);
    static void readPngData(png_structp png, png_bytep out, png_size_t count);
    bool loadPngFrame(const FORMAT_ICO::IcoDirentry* image);
    int calcIcoPitch();
    int getBit(const uint8_t* data, int bit);
    int getNibble(const uint8_t* data, int nibble);
    int getByte(const uint8_t* data, int byte);
};

#endif // FORMATICO_H

