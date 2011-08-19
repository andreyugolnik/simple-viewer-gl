/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

/* DDS loader written by Jon Watte 2002 */
/* Permission granted to use freely, as long as Jon Watte */
/* is held harmless for all possible damages resulting from */
/* your use or failure to use this code. */
/* No warranty is expressed or implied. Use at your own risk, */
/* or not at all. */

#include "formatdds.h"
#include <string.h>
#include <assert.h>
#include <iostream>

//DdsLoadInfo loadInfoDXT1 = {
//true, false, false, 4, 8, GL_COMPRESSED_RGBA_S3TC_DXT1
//};
//DdsLoadInfo loadInfoDXT3 = {
//true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT3
//};
//DdsLoadInfo loadInfoDXT5 = {
//true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT5
//};
DdsLoadInfo loadInfoBGRA8 = {
    false, false, false, 1, 4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR8 = {
    false, false, false, 1, 3, GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR5A1 = {
    false, true, false, 1, 2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV
};
DdsLoadInfo loadInfoBGR565 = {
    false, true, false, 1, 2, GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5
};
DdsLoadInfo loadInfoIndex8 = {
    false, false, true, 1, 1, GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE
};

CFormatDds::CFormatDds(Callback callback)
    : CFormat(callback)
{
}

CFormatDds::~CFormatDds()
{
    FreeMemory();
}

bool CFormatDds::Load(const char* filename, int subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }

    DDS_header header;
    //  DDS is so simple to read, too
    if(sizeof(header) != fread(&header, 1, sizeof(header), m_file))
    {
        fclose(m_file);
        return false;
    }

    if(header.dwMagic != DDS_MAGIC || header.dwSize != 124 || !(header.dwFlags & DDSD_PIXELFORMAT) || !(header.dwFlags & DDSD_CAPS))
    {
        fclose(m_file);
        return false;
    }

    m_width = header.dwWidth;
    m_height = header.dwHeight;
    assert(!(m_width  & (m_width  - 1)));
    assert(!(m_height & (m_height - 1)));

    DdsLoadInfo* li;

    //if(PF_IS_DXT1(header.sPixelFormat))
    //{
    //li = &loadInfoDXT1;
    //}
    //else if(PF_IS_DXT3(header.sPixelFormat))
    //{
    //li = &loadInfoDXT3;
    //}
    //else if(PF_IS_DXT5(header.sPixelFormat))
    //{
    //li = &loadInfoDXT5;
    //}
    //else
    if(PF_IS_BGRA8(header.sPixelFormat))
    {
        li = &loadInfoBGRA8;
    }
    else if(PF_IS_BGR8(header.sPixelFormat))
    {
        li = &loadInfoBGR8;
    }
    else if(PF_IS_BGR5A1(header.sPixelFormat))
    {
        li = &loadInfoBGR5A1;
    }
    else if(PF_IS_BGR565(header.sPixelFormat))
    {
        li = &loadInfoBGR565;
    }
    else if(PF_IS_INDEX8(header.sPixelFormat))
    {
        li = &loadInfoIndex8;
    }
    else
    {
        fclose(m_file);
        return false;
    }

    //FIXME: do cube maps later
    //FIXME: do 3d later
    uint32_t x = m_width;
    uint32_t y = m_height;
    //glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
    uint32_t mipMapCount = (header.dwFlags & DDSD_MIPMAPCOUNT) ? header.dwMipMapCount : 1;
    //if( mipMapCount > 1 ) {
    //hasMipmaps_ = true;
    //}
    if(li->compressed)
    {
        //size_t size	= std::max( li->divSize, x )/li->divSize * std::max( li->divSize, y )/li->divSize * li->blockBytes;
        //assert( size == header.dwPitchOrLinearSize );
        //assert( header.dwFlags & DDSD_LINEARSIZE );
        //uint8_t* data = new uint8_t[size];

        //format	= cFormat = li->internalFormat;
        //for(uint32_t ix = 0; ix < mipMapCount; ++ix) {
        //fread( data, 1, size, f );
        //glCompressedTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, size, data );
        //gl->updateError();
        //x = (x+1)>>1;
        //y = (y+1)>>1;
        //size = max( li->divSize, x )/li->divSize * max( li->divSize, y )/li->divSize * li->blockBytes;
        //}
        //delete[] data;
    }
    else if(li->palette)
    {
        //  currently, we unpack palette into BGRA
        //  I'm not sure we always get pitch...
        assert(header.dwFlags & DDSD_PITCH);
        assert(header.sPixelFormat.dwRGBBitCount == 8);

        m_pitch		= m_width * 4;
        size_t size	= header.dwPitchOrLinearSize * m_height;
        //  And I'm even less sure we don't get padding on the smaller MIP levels...
        assert(size == x * y * li->blockBytes);

        m_format	= li->externalFormat;
        //cFormat	= li->internalFormat;

        uint8_t* data	= new uint8_t[size];
        m_sizeMem	= size * sizeof(uint32_t);
        m_bitmap	= new uint8_t[m_sizeMem];

        uint32_t palette[256];
        fread(palette, 4, 256, m_file);

        for(uint32_t ix = 0; ix < mipMapCount; ++ix) {
            fread(data, 1, size, m_file);
            for(uint32_t zz = 0; zz < size; ++zz) {
                m_bitmap[zz]	= palette[data[zz]];
            }
            //glPixelStorei( GL_UNPACK_ROW_LENGTH, y );
            //glTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, m_bitmap);
            //gl->updateError();
            x	= (x + 1) >> 1;
            y	= (y + 1) >> 1;
            size	= x * y * li->blockBytes;
        }
        delete[] data;
    }
    else
    {
        //if(li->swap) {
        //glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_TRUE );
        //}
        m_format	= li->externalFormat;
        //cFormat	= li->internalFormat;

        m_pitch		= m_width * 4;
        size_t size = x * y * li->blockBytes;
        m_sizeMem	= size;
        m_bitmap	= new uint8_t[m_sizeMem];

        // FIXME: how are MIP maps stored for 24-bit if pitch != m_height*3 ?
        for(uint32_t ix = 0; ix < mipMapCount; ++ix) {
            fread(m_bitmap, 1, size, m_file);
            //glPixelStorei( GL_UNPACK_ROW_LENGTH, y );
            //glTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, m_bitmap);
            //gl->updateError();
            x	= (x + 1) >> 1;
            y	= (y + 1) >> 1;
            size	= x * y * li->blockBytes;
        }
        //glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
        //gl->updateError();
    }
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount-1 );
    //gl->updateError();

    m_bpp = 32;
    m_bppImage = 32;
    //memset(m_bitmap, 0, m_pitch * m_height);

    fclose(m_file);

    return true;
}

