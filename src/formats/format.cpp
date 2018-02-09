/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "format.h"
#include "common/bitmap_description.h"
#include "common/callbacks.h"
#include "common/file.h"

#include <cassert>
#include <cstdio>

cFormat::cFormat(iCallbacks* callbacks)
    : m_callbacks(callbacks)
{
}

cFormat::~cFormat()
{
}

bool cFormat::Load(const char* filename, sBitmapDescription& desc)
{
    m_stop = false;
    return LoadImpl(filename, desc);
}

bool cFormat::LoadSubImage(unsigned subImage, sBitmapDescription& desc)
{
    m_stop = false;
    return LoadSubImageImpl(subImage, desc);
}

void cFormat::dump(sBitmapDescription& desc) const
{
    // ::printf("(II) %s\n", getFormatName(format));
    ::printf("(II) bits per pixel: %u\n", desc.bpp);
    ::printf("(II) image bpp:      %u\n", desc.bppImage);
    ::printf("(II) width:          %u\n", desc.width);
    ::printf("(II) height:         %u\n", desc.height);
    ::printf("(II) pitch:          %u\n", desc.pitch);
    ::printf("(II) size:           %ld\n", desc.size);
    ::printf("(II) frames count:   %u\n", desc.images);
    ::printf("(II) current frame:  %u\n", desc.current);
    ::printf("(II) animation:      %s\n", desc.isAnimation ? "true" : "false");
    ::printf("(II) frame duration: %u\n", desc.delay);
}

void cFormat::updateProgress(float percent) const
{
    m_callbacks->doProgress(percent);
}

bool cFormat::readBuffer(cFile& file, Buffer& buffer, unsigned minSize) const
{
    const unsigned size = buffer.size();
    if (size < minSize)
    {
        buffer.resize(minSize);
        const unsigned length = minSize - size;
        if (length != file.read(&buffer[size], length))
        {
            return false;
        }
    }

    return minSize <= buffer.size();
}

bool cFormat::applyIccProfile(sBitmapDescription& desc, const void* iccProfile, uint32_t iccProfileSize, cCMS::Pixel type) const
{
    m_cms.createTransform(iccProfile, iccProfileSize, type);
    return applyIccProfile(desc);
}

bool cFormat::applyIccProfile(sBitmapDescription& desc, const float* chr, const float* wp, const uint16_t* gmr, const uint16_t* gmg, const uint16_t* gmb, cCMS::Pixel format) const
{
    m_cms.createTransform(chr, wp, gmr, gmg, gmb, format);
    return applyIccProfile(desc);
}

bool cFormat::applyIccProfile(sBitmapDescription& desc) const
{
    bool hasIccProfile = m_cms.hasTransform();

    if (hasIccProfile)
    {
        auto bitmap = desc.bitmap.data();

        for (uint32_t y = 0; y < desc.height; y++)
        {
            m_cms.doTransform(bitmap, bitmap, desc.width);
            bitmap += desc.pitch;

            updateProgress((float)y / desc.height);
        }
    }

    m_cms.destroyTransform();

    return hasIccProfile;
}
