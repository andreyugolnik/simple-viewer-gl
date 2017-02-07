/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "types/vector.h"

#include <vector>

class cQuad;

class cQuadImage final
{
public:
    cQuadImage();
    ~cQuadImage();

    void clear();
    void setBuffer(unsigned width, unsigned height, unsigned pitch, unsigned format, unsigned bytesPP, const unsigned char* image);
    bool upload(unsigned mipmapTextureSize);

    void stop();
    bool isUploading() const;
    float getProgress() const;

    void useFilter(bool filter);
    void render();

    unsigned getWidth() const
    {
        return m_width;
    }

    unsigned getHeight() const
    {
        return m_height;
    }

private:
    void moveToOld();
    void clearOld();
    cQuad* findAndRemoveOld(unsigned col, unsigned row);

    struct sChunk
    {
        unsigned col;
        unsigned row;
        cQuad* quad;
    };

    bool isInsideViewport(const sChunk& chunk, const Vectorf& pos) const;

private:
    bool m_started = false;
    bool m_filter = false;

    unsigned m_texWidth = 0;
    unsigned m_texHeight = 0;
    unsigned m_texPitch = 0;
    unsigned m_cols = 0;
    unsigned m_rows = 0;

    unsigned m_width = 0;
    unsigned m_height = 0;
    unsigned m_pitch = 0;
    unsigned m_format = 0;
    unsigned m_bytesPP = 0;
    const unsigned char* m_image = nullptr;

    std::vector<sChunk> m_chunks;
    std::vector<sChunk> m_chunksOld;

    std::vector<unsigned char> m_buffer;
};
