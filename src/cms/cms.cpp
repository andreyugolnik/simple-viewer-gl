/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "cms.h"

#include <lcms.h>

cCMS::cCMS()
{
    m_outProfile = cmsCreate_sRGBProfile();
}

cCMS::~cCMS()
{
    destroyTransform();

    cmsCloseProfile(m_outProfile);
}

void cCMS::createTransform(void* iccProfile, unsigned iccProfileSize, Pixel format)
{
    destroyTransform();
    if (iccProfile != nullptr)
    {
        auto inProfile = cmsOpenProfileFromMem(iccProfile, iccProfileSize);

        auto pixel = format == Pixel::Rgb ? TYPE_RGB_8 : TYPE_RGBA_8;
        m_transform = cmsCreateTransform(inProfile, pixel
                                         , m_outProfile, pixel
                                         , INTENT_PERCEPTUAL, 0);
        cmsCloseProfile(inProfile);
    }
}

void cCMS::destroyTransform()
{
    if (m_transform != nullptr)
    {
        cmsDeleteTransform(m_transform);
        m_transform = nullptr;
    }
}

void cCMS::doTransform(void* input, void* output, unsigned sizeInPixels) const
{
    assert(m_transform != nullptr);
    cmsDoTransform(m_transform, input, output, sizeInPixels);
}
