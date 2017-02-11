/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "quadimage.h"
#include "common/helpers.h"
#include "imageborder.h"
#include "quad.h"

#include <cassert>
#include <cmath>
#include <cstdio>
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

    m_texPitch = helpers::calculatePitch(m_texWidth, bytesPP);
    // ::printf("(II) Textue pitch: %u.\n", m_texPitch);

    m_cols = (unsigned)::ceilf((float)width / m_texWidth);
    m_rows = (unsigned)::ceilf((float)height / m_texHeight);
    // ::printf("(II) Image size: %u x %u.\n", width, height);
    // ::printf("(II) Textures: %u (%u x %u) required\n", m_cols * m_rows, m_cols, m_rows);

    moveToOld();

    m_width = width;
    m_height = height;
    m_pitch = pitch;
    m_format = format;
    m_bytesPP = bytesPP;
    m_image = image;

    m_buffer.resize(m_texPitch * m_texHeight);

    m_started = true;
}

bool cQuadImage::upload(unsigned mipmapTextureSize)
{
    const auto size = m_chunks.size();
    assert(size < m_rows * m_cols);

    const unsigned col = size % m_cols;
    const unsigned row = size / m_cols;

    const unsigned w = (col < m_cols - 1) ? m_texWidth : (m_width - m_texWidth * (m_cols - 1));
    const unsigned h = (row < m_rows - 1) ? m_texHeight : (m_height - m_texHeight * (m_rows - 1));
    // ::printf("cols %u : col %u : w %u\n", m_cols, col, w);
    // ::printf("rows %u : row %u : h %u\n", m_rows, row, h);

    const unsigned sx = col * m_texPitch;
    const unsigned sy = row * m_texHeight;
    const unsigned dstPitch = helpers::calculatePitch(w, m_bytesPP);
    for (unsigned y = 0; y < h; y++)
    {
        const unsigned src = sx + (sy + y) * m_pitch;
        if (src + dstPitch <= m_pitch * m_height)
        {
            const unsigned dst = y * dstPitch;
            ::memcpy(&m_buffer[dst], &m_image[src], dstPitch);
        }
        else
        {
            ::printf("cols %u : col %u : w %u\n", m_cols, col, w);
            ::printf("rows %u : row %u : h %u\n", m_rows, row, h);
            ::printf("out at line %u sx %u sy %u bpp: %u\n", y, sx, sy, m_bytesPP);
            break;
        }
    }

    cRenderer::enableMipmap(m_width >= mipmapTextureSize || m_height >= mipmapTextureSize);

    cQuad* quad = findAndRemoveOld(col, row);
    if (quad == nullptr
        || quad->getTexWidth() != w || quad->getTexHeight() != h
        || quad->getFormat() != m_format)
    {
        delete quad;
        quad = new cQuad(w, h, m_buffer.data(), m_format);
    }
    else
    {
        quad->setData(m_buffer.data());
    }

    quad->useFilter(m_filter);

    m_chunks.push_back({ col, row, quad });

    const bool isDone = isUploading() == false;
    if (isDone)
    {
        stop();
    }

    return isDone;
}

void cQuadImage::stop()
{
    clearOld();
    m_buffer.resize(0);
    m_started = false;
}

bool cQuadImage::isUploading() const
{
    return m_started && m_chunks.size() < m_rows * m_cols;
}

float cQuadImage::getProgress() const
{
    return m_chunks.size() / (float)(m_rows * m_cols);
}

void cQuadImage::useFilter(bool filter)
{
    m_filter = filter;
    for (auto& chunk : m_chunks)
    {
        chunk.quad->useFilter(filter);
    }
}

bool cQuadImage::isInsideViewport(const sChunk& chunk, const Vectorf& pos) const
{
    auto& rc = cRenderer::getRect();
    const auto& size = chunk.quad->getSize();
    const Rectf rcQuad{ pos, pos + size };
    return rc.intersect(rcQuad);
}

void cQuadImage::render()
{
    const float halfWidth = ::ceilf(m_width * 0.5f);
    const float halfHeight = ::ceilf(m_height * 0.5f);
    const float texWidth = m_texWidth;
    const float texHeight = m_texHeight;

    for (const auto& chunk : m_chunksOld)
    {
        const Vectorf pos
        {
            chunk.col * texWidth - halfWidth,
            chunk.row * texHeight - halfHeight
        };
        if (isInsideViewport(chunk, pos))
        {
            chunk.quad->render(pos);
        }
    }

    for (const auto& chunk : m_chunks)
    {
        const Vectorf pos
        {
            chunk.col * texWidth - halfWidth,
            chunk.row * texHeight - halfHeight
        };
        if (isInsideViewport(chunk, pos))
        {
            chunk.quad->render(pos);
        }
    }

#if 0
    cImageBorder border;
    border.setColor({ 0, 0, 255, 255 });
    border.setThickness(0.5f);
    const float zoom = cRenderer::getZoom();

    uint32_t rendered = 0;
    static uint32_t prevRendered = 0;

    for (const auto& chunk : m_chunksOld)
    {
        const Vectorf pos
        {
            chunk.col * texWidth - halfWidth,
            chunk.row * texHeight - halfHeight
        };
        if (isInsideViewport(chunk, pos))
        {
            rendered++;
            border.render(pos, chunk.quad->getSize(), zoom);
        }
    }

    for (const auto& chunk : m_chunks)
    {
        const Vectorf pos
        {
            chunk.col * texWidth - halfWidth,
            chunk.row * texHeight - halfHeight
        };
        if (isInsideViewport(chunk, pos))
        {
            rendered++;
            border.render(pos, chunk.quad->getSize(), zoom);
        }
    }

    if (prevRendered != rendered)
    {
        const auto total = (uint32_t)(m_chunksOld.size() + m_chunks.size());
        prevRendered = rendered;
        ::printf("(II) Total chunks: %u rendered: %u\n"
                 , total
                 , rendered);
    }
#endif
}

void cQuadImage::moveToOld()
{
    clearOld();

    for (size_t i = 0, size = m_chunks.size(); i < size;)
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
