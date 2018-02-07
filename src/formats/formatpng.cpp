/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatpng.h"
#include "common/file.h"
#include "formats/PngReader.h"

#include <cstring>

cFormatPng::cFormatPng(iCallbacks* callbacks)
    : cFormat(callbacks)
{
}

cFormatPng::~cFormatPng()
{
}

bool cFormatPng::isSupported(cFile& file, Buffer& buffer) const
{
    if (!readBuffer(file, buffer, cPngReader::MinBytesToTest))
    {
        return false;
    }

    return cPngReader::isValid(buffer.data(), file.getSize());
}

bool cFormatPng::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    cPngReader reader(m_cms);
    reader.setProgressCallback([this](float progress) {
        updateProgress(progress);
    });

    auto result = reader.loadPng(desc, file);
    if (result)
    {
        m_formatName = m_cms.hasTransform() ? "png/icc" : "png";
    }

    return result;
}
