/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../types/types.h"

class cCMS final
{
public:
    cCMS();
    ~cCMS();

    enum class Pixel
    {
        Rgba,
        Rgb,
    };
    void createTransform(const void* iccProfile, uint32_t iccProfileSize, Pixel format);
    void createTransform(const float* chr, const float* wp
                         , const uint16_t* gmr
                         , const uint16_t* gmg
                         , const uint16_t* gmb
                         , Pixel format);
    void destroyTransform();

    void doTransform(void* input, void* output, uint32_t sizeInPixels) const;

    bool hasTransform() const
    {
        return m_transform != nullptr;
    }

private:
    void createTransform(void* inProfile, Pixel format);

private:
    void* m_outProfile = nullptr;
    void* m_transform = nullptr;
};
