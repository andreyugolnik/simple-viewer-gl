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
    void createTransform(void* iccProfile, unsigned iccProfileSize, Pixel format);
    void destroyTransform();

    void doTransform(void* input, void* output, unsigned sizeInPixels) const;

    bool hasTransform() const
    {
        return m_transform != nullptr;
    }
    
private:
    void* m_outProfile = nullptr;
    void* m_transform = nullptr;
};
