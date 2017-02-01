/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

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
    void createTransform(const void* iccProfile, unsigned iccProfileSize, Pixel format);
    void createTransform(const float* chr, const float* wp
                         , const unsigned short* gmr
                         , const unsigned short* gmg
                         , const unsigned short* gmb
                         , Pixel format);
    void destroyTransform();

    void doTransform(void* input, void* output, unsigned sizeInPixels) const;

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
