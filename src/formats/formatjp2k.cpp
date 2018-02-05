/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatjp2k.h"

#if defined(OPENJPEG_SUPPORT)

#include "common/bitmap_description.h"
#include "common/file.h"

#include <cstring>
#include <openjpeg.h>

namespace
{
    void j2k_info_callback(const char* msg, void* /*client_data*/)
    {
        // ::printf("(II) %s", msg);
        (void)msg;
    }

    void j2k_warning_callback(const char* msg, void* /*client_data*/)
    {
        // ::printf("(WW) %s", msg);
        (void)msg;
    }

    void j2k_error_callback(const char* msg, void* /*client_data*/)
    {
        ::printf("(EE) %s", msg);
    }

    // int int_ceildivpow2(int a, int b)
    // {
    // return (a + (1 << b) - 1) >> b;
    // }

    uint8_t* getScanLine(sBitmapDescription& desc, uint32_t y)
    {
        return desc.bitmap.data() + desc.pitch * y;
    }

    const uint32_t FI_RGBA_RED = 0;
    const uint32_t FI_RGBA_GREEN = 1;
    const uint32_t FI_RGBA_BLUE = 2;
    const uint32_t FI_RGBA_ALPHA = 3;
} // namespace

cFormatJp2k::cFormatJp2k(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatJp2k::~cFormatJp2k()
{
}

bool cFormatJp2k::isSupported(cFile& file, Buffer& buffer) const
{
    const uint8_t jp2_signature[] = { 0x00, 0x00, 0x00, 0x0C, 0x6A, 0x50, 0x20, 0x20, 0x0D, 0x0A, 0x87, 0x0A };

    if (!readBuffer(file, buffer, sizeof(jp2_signature)))
    {
        return false;
    }

    return ::memcmp(jp2_signature, buffer.data(), sizeof(jp2_signature)) == 0;
}

size_t streamRead(void* buffer, size_t size, void* user)
{
    // ::printf("read bytes %u\n", (uint32_t)size);
    auto file = static_cast<cFile*>(user);
    auto readed = file->read(buffer, size);
    return readed ? readed : (size_t)-1;
}

void streamClose(void* user)
{
    // ::printf("close\n");
    auto file = static_cast<cFile*>(user);
    file->close();
}

off_t streamSkip(off_t bytes, void* user)
{
    // ::printf("skip bytes %u\n", (uint32_t)bytes);
    auto file = static_cast<cFile*>(user);
    if (file->seek(bytes, SEEK_CUR) != 0)
    {
        return -1;
    }

    return bytes;
}

int streamSeek(off_t bytes, void* user)
{
    // ::printf("seek bytes %u\n", (uint32_t)bytes);
    auto file = static_cast<cFile*>(user);
    return file->seek(bytes, SEEK_SET) == 0;
}

bool cFormatJp2k::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    auto stream = opj_stream_default_create(true);

    opj_stream_set_user_data(stream, &file, streamClose);
    opj_stream_set_user_data_length(stream, desc.size);

    opj_stream_set_read_function(stream, streamRead);
    opj_stream_set_skip_function(stream, streamSkip);
    opj_stream_set_seek_function(stream, streamSeek);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);

    // decode the JPEG-2000 codestream

    // get a decoder handle
    auto d_codec = opj_create_decompress(OPJ_CODEC_JP2);

    // configure the event callbacks
    // catch events using our callbacks (no local context needed here)
    opj_set_info_handler(d_codec, j2k_info_callback, nullptr);
    opj_set_warning_handler(d_codec, j2k_warning_callback, nullptr);
    opj_set_error_handler(d_codec, j2k_error_callback, nullptr);

    // setup the decoder decoding parameters using user parameters
    if (!opj_setup_decoder(d_codec, &parameters))
    {
        ::printf("Failed to setup the decoder\n");
        return false;
    }

    opj_image_t* image = nullptr;

    // read the main header of the codestream and if necessary the JP2 boxes
    if (!opj_read_header(stream, d_codec, &image))
    {
        ::printf("(EE) Failed to read the header.\n");
        return false;
    }

    // decode the stream and fill the image structure
    if (!(opj_decode(d_codec, stream, image) && opj_end_decompress(d_codec, stream)))
    {
        ::printf("(EE) Failed to decode image.\n");
        return false;
    }

    // free the codec context
    opj_destroy_codec(d_codec);
    d_codec = nullptr;

    // create output image
    if (loadJp2k(image, desc) == false)
    {
        ::printf("(EE) Failed to import JPEG2000 image.\n");
        return false;
    }

    // free image data structure
    opj_image_destroy(image);

    opj_stream_destroy(stream);

    m_formatName = "jpeg2000";

    return true;
}

bool cFormatJp2k::loadJp2k(void* img, sBitmapDescription& desc) const
{
    auto image = static_cast<opj_image_t*>(img);

    desc.reset();

    // compute image width and height

    // //uint32_t w = int_ceildiv(image->x1 - image->x0, image->comps[0].dx);
    uint32_t wr = image->comps[0].w;
    // uint32_t wrr = int_ceildivpow2(image->comps[0].w, image->comps[0].factor);

    // //uint32_t h = int_ceildiv(image->y1 - image->y0, image->comps[0].dy);
    // //uint32_t hr = image->comps[0].h;
    // uint32_t hrr = int_ceildivpow2(image->comps[0].h, image->comps[0].factor);

    // check the number of components

    uint32_t numcomps = image->numcomps;

    bool bIsValid = true;
    for (uint32_t c = 0; c < numcomps - 1; c++)
    {
        if (image->comps[c].dx != image->comps[c + 1].dx
            || image->comps[c].dy != image->comps[c + 1].dy
            || image->comps[c].prec != image->comps[c + 1].prec)
        {
            bIsValid = false;
            break;
        }
    }

    bIsValid &= ((numcomps == 1) || (numcomps == 3) || (numcomps == 4));
    if (!bIsValid)
    {
        if (numcomps)
        {
            ::printf("(WW) image contains %u greyscale components. Only the first will be loaded.\n", numcomps);
            numcomps = 1;
        }
        else
        {
            // unknown type
            return false;
        }
    }

    desc.format = numcomps == 4 ? GL_RGBA : GL_RGB;
    desc.bpp = numcomps * 8;
    desc.width = image->comps[0].w;
    desc.height = image->comps[0].h;
    desc.pitch = numcomps * desc.width;

    desc.bppImage = numcomps * 8; // image->comps[0].bpp;

    desc.images = 1;

    if (image->comps[0].prec > 16)
    {
        return false; // ERROR_UNSUPPORTED_FORMAT;
    }

    desc.bitmap.resize(desc.pitch * desc.height);

    if (image->comps[0].prec <= 8)
    {
        if (numcomps == 1)
        {
            // 8-bit greyscale
            // ----------------------------------------------------------

            // build a greyscale palette

#if 0
            RGBQUAD* pal = FreeImage_GetPalette(dib);
            for (int i = 0; i < 256; i++)
            {
                pal[i].rgbRed = (uint8_t)i;
                pal[i].rgbGreen = (uint8_t)i;
                pal[i].rgbBlue = (uint8_t)i;
            }

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                uint8_t* bits = getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    int index = image->comps[0].data[pixel_pos];
                    index += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    bits[x] = (uint8_t)index;

                    pixel_count++;
                }
            }
#else
            ::printf("(EE) 8-bit grayscale not supported at this moment.\n");
            return false;
#endif
        }
        else if (numcomps == 3)
        {
            // 24-bit RGB
            // ----------------------------------------------------------

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                uint8_t* bits = getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    uint32_t r = image->comps[0].data[pixel_pos];
                    r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    uint32_t g = image->comps[1].data[pixel_pos];
                    g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);

                    uint32_t b = image->comps[2].data[pixel_pos];
                    b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

                    bits[FI_RGBA_RED] = (uint8_t)r;
                    bits[FI_RGBA_GREEN] = (uint8_t)g;
                    bits[FI_RGBA_BLUE] = (uint8_t)b;
                    bits += 3;

                    pixel_count++;
                }
            }
        }
        else if (numcomps == 4)
        {
            // 32-bit RGBA
            // ----------------------------------------------------------

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                uint8_t* bits = getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    uint32_t r = image->comps[0].data[pixel_pos];
                    r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    uint32_t g = image->comps[1].data[pixel_pos];
                    g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);

                    uint32_t b = image->comps[2].data[pixel_pos];
                    b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

                    uint32_t a = image->comps[3].data[pixel_pos];
                    a += (image->comps[3].sgnd ? 1 << (image->comps[3].prec - 1) : 0);

                    bits[FI_RGBA_RED] = (uint8_t)r;
                    bits[FI_RGBA_GREEN] = (uint8_t)g;
                    bits[FI_RGBA_BLUE] = (uint8_t)b;
                    bits[FI_RGBA_ALPHA] = (uint8_t)a;
                    bits += 4;

                    pixel_count++;
                }
            }
        }
    }
    else if (image->comps[0].prec <= 16)
    {
        if (numcomps == 1)
        {
            // 16-bit greyscale
            // ----------------------------------------------------------

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                auto bits = (uint16_t*)getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    uint32_t index = image->comps[0].data[pixel_pos];
                    index += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    bits[x] = (uint16_t)index;

                    pixel_count++;
                }
            }
        }
        else if (numcomps == 3)
        {
            // 48-bit RGB
            // ----------------------------------------------------------

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                auto bits = (uint8_t*)getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    uint32_t r = image->comps[0].data[pixel_pos];
                    r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    uint32_t g = image->comps[1].data[pixel_pos];
                    g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);

                    uint32_t b = image->comps[2].data[pixel_pos];
                    b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

                    bits[FI_RGBA_RED] = (uint8_t)r;
                    bits[FI_RGBA_GREEN] = (uint8_t)g;
                    bits[FI_RGBA_BLUE] = (uint8_t)b;

                    pixel_count++;
                }
            }
        }
        else if (numcomps == 4)
        {
            // 64-bit RGBA
            // ----------------------------------------------------------

            // load pixel data

            uint32_t pixel_count = 0;

            for (uint32_t y = 0; y < desc.height; y++)
            {
                auto bits = (uint8_t*)getScanLine(desc, y);//desc.height - 1 - y);

                for (uint32_t x = 0; x < desc.width; x++)
                {
                    const uint32_t pixel_pos = pixel_count / desc.width * wr + pixel_count % desc.width;

                    uint32_t r = image->comps[0].data[pixel_pos];
                    r += (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);

                    uint32_t g = image->comps[1].data[pixel_pos];
                    g += (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);

                    uint32_t b = image->comps[2].data[pixel_pos];
                    b += (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

                    uint32_t a = image->comps[3].data[pixel_pos];
                    a += (image->comps[3].sgnd ? 1 << (image->comps[3].prec - 1) : 0);

                    bits[FI_RGBA_RED] = (uint8_t)r;
                    bits[FI_RGBA_GREEN] = (uint8_t)g;
                    bits[FI_RGBA_BLUE] = (uint8_t)b;
                    bits[FI_RGBA_ALPHA] = (uint8_t)a;

                    pixel_count++;
                }
            }
        }
    }

    return true;
}

#endif
