/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#if defined(OPENEXR_SUPPORT)

#include "formatexr.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"
#include "../common/helpers.h"

#include <cstring>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfPreviewImage.h>
#include <OpenEXR/ImfStandardAttributes.h>

namespace
{

    struct sRgba8888
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct sRgb888
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    inline uint8_t halfToUint8(const half& h)
    {
        return (uint8_t)helpers::clamp<uint32_t>(0, 255, h * 255.0f);
    }

    const char* getFormat(uint32_t format)
    {
        const char* Formats[] =
        {
            "exr",         // no compression
            "exr/rle",     // run length encoding
            "exr/zips",    // zlib compression, one scan line at a time
            "exr/zip",     // zlib compression, in blocks of 16 scan lines
            "exr/piz",     // piz-based wavelet compression
            "exr/pxr24",   // lossy 24-bit float compression
            "exr/b44",     // lossy 4-by-4 pixel block compression, fixed compression rate
            "exr/b44a",    // lossy 4-by-4 pixel block compression, flat fields are compressed more
            "exr/dwaa",    // lossy DCT based compression, in blocks of 32 scanlines. More efficient for partial buffer access.

            "exr/dwab",    // lossy DCT based compression, in blocks
                           // of 256 scanlines. More efficient space
                           // wise and faster to decode full frames
                           // than DWAA_COMPRESSION.
        };

        return format < helpers::countof(Formats) ? Formats[format] : "unknown";
    }

    void readStringField(const Imf::Header& header, const char* field, const char* title, sBitmapDescription::ExifList& exifList)
    {
        auto value = header.findTypedAttribute<Imf::StringAttribute>(field);
        if (value != nullptr)
        {
            exifList.push_back({ title, value->value() });
        }
    }

#if 0
    bool loadPreview(Imf::RgbaInputFile& in, sBitmapDescription& desc)
    {
        auto& header = in.header();
        const bool hasPreview = header.hasPreviewImage();
        if (hasPreview)
        {
            const auto& preview = header.previewImage();

            const uint32_t width = preview.width();
            const uint32_t height = preview.height();

            desc.width = width;
            desc.height = height;
            desc.bppImage = 0;

            desc.pitch = desc.width * 4;
            desc.bitmap.resize(desc.pitch * desc.height);

            desc.bpp = 32;
            desc.format = GL_RGBA;

            auto bitmap = reinterpret_cast<sRgba8888*>(desc.bitmap.data());

            for (uint32_t y = 0; y < height; ++y)
            {
                size_t idx = y * width;
                for (uint32_t x = 0; x < width; ++x)
                {
                    const auto& p = preview.pixel(x, y);

                    bitmap[idx].r = halfToUint8(p.r);
                    bitmap[idx].g = halfToUint8(p.g);
                    bitmap[idx].b = halfToUint8(p.b);
                    bitmap[idx].a = halfToUint8(p.a);
                    idx++;
                }
            }
            return true;
        }

        return false;
    }
#endif

}

cFormatExr::cFormatExr(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatExr::~cFormatExr()
{
}

bool cFormatExr::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, 4))
    {
        return false;
    }

    const auto h = buffer.data();
    return h[0] == 0x76 && h[1] == 0x2f && h[2] == 0x31 && h[3] == 0x01;
}

bool cFormatExr::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    m_filename = filename;
    return load(0, desc);
}

bool cFormatExr::LoadSubImageImpl(unsigned current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool cFormatExr::load(unsigned current, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(m_filename.c_str()))
    {
        return false;
    }

    desc.size = file.getSize();

    file.close();

    try
    {
        Imf::RgbaInputFile in(m_filename.c_str());
        bool result = in.isComplete();

        if (result)
        {
            desc.images = 1;
            desc.current = 0;

            auto& dw = in.dataWindow();
            const uint32_t width = dw.max.x - dw.min.x + 1;
            const uint32_t height = dw.max.y - dw.min.y + 1;

            desc.width = width;
            desc.height = height;

            // WRITE_R    = 0x01, // Red
            // WRITE_G    = 0x02, // Green
            // WRITE_B    = 0x04, // Blue
            // WRITE_A    = 0x08, // Alpha
            // WRITE_Y    = 0x10, // Luminance, for black-and-white images, or in combination with chroma
            // WRITE_C    = 0x20, // Chroma (two subsampled channels, RY and BY, supported only for scanline-based files)
            //
            // WRITE_RGB  = 0x07, // Red, green, blue
            // WRITE_RGBA = 0x0f, // Red, green, blue, alpha
            // WRITE_YC   = 0x30, // Luminance, chroma
            // WRITE_YA   = 0x18, // Luminance, alpha
            // WRITE_YCA  = 0x38  // Luminance, chroma, alpha
            const auto channels = in.channels();
            // const bool hasR = (channels & Imf::WRITE_R) != 0;
            // const bool hasG = (channels & Imf::WRITE_G) != 0;
            // const bool hasB = (channels & Imf::WRITE_B) != 0;
            // const bool hasA = (channels & Imf::WRITE_A) != 0;

            uint32_t chCount = 0;

            chCount += (channels & Imf::WRITE_R) != 0;
            chCount += (channels & Imf::WRITE_G) != 0;
            chCount += (channels & Imf::WRITE_B) != 0;
            chCount += (channels & Imf::WRITE_A) != 0;

            chCount += (channels & Imf::WRITE_Y) != 0;
            chCount += (channels & Imf::WRITE_C) != 0;

            desc.bppImage = chCount * 8;

            const bool hasA = (channels & Imf::WRITE_A) != 0;
            const uint32_t bytes = hasA ? 4 : 3;
            desc.pitch = helpers::calculatePitch(desc.width, bytes * 8);
            desc.bitmap.resize(desc.pitch * desc.height);

            desc.bpp = bytes * 8;
            desc.format = hasA ? GL_RGBA : GL_RGB;

            const auto compression = in.compression();
            m_formatName = getFormat(compression);

            std::vector<Imf::Rgba> input(width * height);
            try
            {
                const auto dx = dw.min.x;
                const auto dy = dw.min.y;
                in.setFrameBuffer(input.data() - dx - dy * width, 1, width);
                in.readPixels(dw.min.y, dw.max.y);
            }
            catch (const IEX_NAMESPACE::InputExc&)
            {
                result = false;
                ::printf("(EE) readPixel: scanLinesMissing\n");
            }
            catch (const IEX_NAMESPACE::IoExc&)
            {
                result = false;
                ::printf("(EE) readPixel: scanLinesBroken\n");
            }
            catch (...)
            {
                ::printf("(EE) readPixel: error.\n");
            }

            if (result)
            {
                if (hasA)
                {
                    auto src = input.data();
                    for (uint32_t y = 0; y < height; y++)
                    {
                        auto dst = reinterpret_cast<sRgba8888*>(desc.bitmap.data() + y * desc.pitch);
                        for (uint32_t x = 0; x < width; x++)
                        {
                            const auto& i = *src++;
                            dst[x].r = halfToUint8(i.r);
                            dst[x].g = halfToUint8(i.g);
                            dst[x].b = halfToUint8(i.b);
                            dst[x].a = halfToUint8(i.a);
                        }
                    }
                }
                else
                {
                    auto src = input.data();
                    for (uint32_t y = 0; y < height; y++)
                    {
                        auto dst = reinterpret_cast<sRgb888*>(desc.bitmap.data() + y * desc.pitch);
                        for (uint32_t x = 0; x < width; x++)
                        {
                            const auto& i = *src++;
                            dst[x].r = halfToUint8(i.r);
                            dst[x].g = halfToUint8(i.g);
                            dst[x].b = halfToUint8(i.b);
                        }
                    }
                }

                auto& header = in.header();
                auto& exifList = desc.exifList;
                readStringField(header, "owner", "Owner:", exifList);
                readStringField(header, "capDate", "Date:", exifList);
                readStringField(header, "comments", "Comments:", exifList);
                readStringField(header, "type", "Type:", exifList);

#if 0
                for (auto it = header.begin(), itEnd = header.end(); it != itEnd; ++it)
                {
                    auto& attr = it.attribute();
                    exifList.push_back({ it.name(), attr.typeName() });
                }
#endif
            }

            return result;
        }
    }
    catch (...)
    {
        ::printf("(EE) OpenEXR open file error.\n");
    }

    return false;
}

#endif
