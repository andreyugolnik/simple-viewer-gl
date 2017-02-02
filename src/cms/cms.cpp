/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "cms.h"

#include <cassert>
#include <lcms2.h>

cCMS::cCMS()
{
    m_outProfile = cmsCreate_sRGBProfile();
}

cCMS::~cCMS()
{
    destroyTransform();

    cmsCloseProfile(m_outProfile);
}

void cCMS::createTransform(const void* iccProfile, unsigned iccProfileSize, Pixel format)
{
    destroyTransform();
    if (iccProfile != nullptr)
    {
        auto inProfile = cmsOpenProfileFromMem(iccProfile, iccProfileSize);
        createTransform(inProfile, format);
    }
}

void cCMS::createTransform(const float* chr, const float* wp
                           , const unsigned short* gmr
                           , const unsigned short* gmg
                           , const unsigned short* gmb
                           , Pixel format)
{
    cmsCIExyYTRIPLE Primaries;
    Primaries.Red.x   =  chr[0];
    Primaries.Red.y   =  chr[1];
    Primaries.Green.x =  chr[2];
    Primaries.Green.y =  chr[3];
    Primaries.Blue.x  =  chr[4];
    Primaries.Blue.y  =  chr[5];
    Primaries.Red.Y = Primaries.Green.Y = Primaries.Blue.Y = 1.0;

    cmsCIExyY WhitePoint;
    WhitePoint.x = wp[0];
    WhitePoint.y = wp[1];
    WhitePoint.Y = 1.0;

    cmsToneCurve* Curve[3];
    Curve[0] = cmsBuildTabulatedToneCurve16(nullptr, 256, gmr);
    Curve[1] = cmsBuildTabulatedToneCurve16(nullptr, 256, gmg);
    Curve[2] = cmsBuildTabulatedToneCurve16(nullptr, 256, gmb);

    auto inProfile = cmsCreateRGBProfileTHR(nullptr, &WhitePoint, &Primaries, Curve);

    for (unsigned i = 0; i < 3; i++)
    {
        cmsFreeToneCurve(Curve[i]);
    }

    createTransform(inProfile, format);
}

void cCMS::createTransform(void* inProfile, Pixel format)
{
    if (inProfile != nullptr)
    {
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
