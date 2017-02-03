/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "quadimage.h"
#include "quad.h"

#include <cassert>
#include <cmath>
#include <cstring>

cQuadImage::cQuadImage()
{
}

cQuadImage::~cQuadImage()
{
    clear();
}

void cQuadImage::clear()
{
    m_texWidth = 0;
    m_texHeight = 0;
    m_texPitch = 0;
    m_cols = 0;
    m_rows = 0;

    m_width = 0;
    m_height = 0;
    m_pitch = 0;
    m_format = 0;
    m_bytesPP = 0;
    m_image = nullptr;

    clearOld();

    for (auto& chunk : m_chunks)
    {
        delete chunk.quad;
    }
    m_chunks.clear();

    m_buffer.resize(0);
}

void cQuadImage::clearOld()
{
    for (auto& chunk : m_chunksOld)
    {
        delete chunk.quad;
    }
    m_chunksOld.clear();
}

void cQuadImage::setBuffer(unsigned width, unsigned height, unsigned pitch, unsigned format, unsigned bytesPP, const unsigned char* image)
{
    m_texWidth = cRenderer::calculateTextureSize(width);
    m_texHeight = cRenderer::calculateTextureSize(height);

    // texture pitch should be multiple by 4
    m_texPitch = (unsigned)ceilf(m_texWidth * bytesPP / 4.0f) * 4;
    //const unsigned line = texWidth * bytesPP;
    //const unsigned texPitch = line + (line % 4) * 4;

    m_cols = (unsigned)ceilf((float)width / m_texWidth);
    m_rows = (unsigned)ceilf((float)height / m_texHeight);
    // printf("textures: %d (%d x %d) required\n", m_cols * m_rows, m_cols, m_rows);

    moveToOld();

    m_width = width;
    m_height = height;
    m_pitch = pitch;
    m_format = format;
    m_bytesPP = bytesPP;
    m_image = image;

    // printf(" %d x %d, ", width, height);

    m_buffer.resize(m_texPitch * m_texHeight);
}

bool cQuadImage::upload(unsigned mipmapTextureSize)
{
    const auto size = m_chunks.size();
    assert(size < m_rows * m_cols);

    const unsigned col = size % m_cols;
    const unsigned row = size / m_cols;

    const unsigned w = col < (m_cols - 1) ? m_texWidth : (m_width - m_texWidth * (m_cols - 1));
    const unsigned h = row < (m_rows - 1) ? m_texHeight : (m_height - m_texHeight * (m_rows - 1));
    // printf("cols %u : col %u : w %u\n", m_cols, col, w);
    // printf("rows %u : row %u : h %u\n", m_rows, row, h);

    unsigned dx = col * m_texPitch;
    unsigned dy = row * m_texHeight;
    unsigned count = w * m_bytesPP;
    for (unsigned line = 0; line < h; line++)
    {
        const unsigned src = dx + (dy + line) * m_pitch;
        const unsigned dst = line * m_texPitch;
        memcpy(&m_buffer[dst], &m_image[src], count);
    }

    cRenderer::enableMipmap(m_width >= mipmapTextureSize || m_height >= mipmapTextureSize);

    cQuad* quad = findAndRemoveOld(col, row);
    if (quad == nullptr
        || quad->GetTexWidth() != m_texWidth || quad->GetTexHeight() != m_texHeight
        || quad->getFormat() != m_format)
    {
        delete quad;
        quad = new cQuad(m_texWidth, m_texHeight, &m_buffer[0], m_format);
    }
    else
    {
        quad->setData(&m_buffer[0]);
    }

    quad->SetSpriteSize(w, h);
    quad->useFilter(false);

    m_chunks.push_back({ col, row, quad });

    const bool isDone = isUploading() == false;
    if (isDone)
    {
        clearOld();
        m_buffer.resize(0);
    }

    return isDone;
}

bool cQuadImage::isUploading() const
{
    return m_chunks.size() < m_rows * m_cols;
}

float cQuadImage::getProgress() const
{
    return m_chunks.size() / (float)(m_rows * m_cols);
}

void cQuadImage::useFilter(bool filter)
{
    for (auto& chunk : m_chunks)
    {
        chunk.quad->useFilter(filter);
    }
}

void cQuadImage::render()
{
    const float halfWidth = ceilf(m_width * 0.5f);
    const float halfHeight = ceilf(m_height * 0.5f);
    const unsigned texWidth = m_texWidth;
    const unsigned texHeight = m_texHeight;

    for (const auto& chunk : m_chunksOld)
    {
        const float x = chunk.col * texWidth - halfWidth;
        const float y = chunk.row * texHeight - halfHeight;

        chunk.quad->Render(x, y);
    }

    for (const auto& chunk : m_chunks)
    {
        const float x = chunk.col * texWidth - halfWidth;
        const float y = chunk.row * texHeight - halfHeight;

        chunk.quad->Render(x, y);
    }
}

void cQuadImage::moveToOld()
{
    clearOld();

    for (size_t i = 0, size = m_chunks.size(); i < size; )
    {
        const auto& chunk = m_chunks[i];
        if (chunk.col >= m_cols || chunk.row >= m_rows)
        {
            // printf("removed: %u x %u\n", chunk.col, chunk.row);
            delete chunk.quad;
            m_chunks[i] = m_chunks[--size];
            m_chunks.pop_back();
        }
        else
        {
            i++;
        }
    }

    m_chunksOld = m_chunks;
    m_chunks.clear();
}

cQuad* cQuadImage::findAndRemoveOld(unsigned col, unsigned row)
{
    cQuad* quad = nullptr;

    for (size_t i = 0, size = m_chunksOld.size(); i < size; i++)
    {
        const auto& chunk = m_chunksOld[i];
        if (chunk.col == col && chunk.row == row)
        {
            quad = chunk.quad;
            m_chunksOld[i] = m_chunksOld.back();
            m_chunksOld.pop_back();
            break;
        }
    }

    return quad;
}
