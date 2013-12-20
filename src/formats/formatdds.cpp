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
#include <stdlib.h>

/*
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
*/
CFormatDds::CFormatDds(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatDds::~CFormatDds()
{
}

unsigned char *texture_load_dds(const char *name,int *width,int *height);

bool CFormatDds::Load(const char* filename, unsigned subImage)
{
    int w;
    int h;
    unsigned char* tex = texture_load_dds(filename, &w, &h);
    if(tex)
    {
        m_width = w;
        m_height = h;
        m_bitmap.resize(m_width * m_height * 4);
        memcpy(&m_bitmap[0], tex, m_width * m_height * 4);
        return true;
    }
    return false;
         
    /*
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

        uint8_t* data = new uint8_t[size];
        m_bitmap.resize(size * sizeof(uint32_t));

        uint32_t palette[256];
        fread(palette, 4, 256, m_file);

        for(uint32_t ix = 0; ix < mipMapCount; ++ix)
        {
            fread(data, 1, size, m_file);
            for(uint32_t zz = 0; zz < size; ++zz)
            {
                m_bitmap[zz] = palette[data[zz]];
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
        m_bitmap.resize(size);

        // FIXME: how are MIP maps stored for 24-bit if pitch != m_height*3 ?
        for(uint32_t ix = 0; ix < mipMapCount; ++ix)
        {
            fread(&m_bitmap[0], 1, m_bitmap.size(), m_file);
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
    */
}

/* load DDS image
 */
struct dds_colorkey {
	unsigned int dwColorSpaceLowValue;
	unsigned int dwColorSpaceHighValue;
};

struct dds_header {
	unsigned int magic;
	unsigned int dwSize;
	unsigned int dwFlags;
	unsigned int dwHeight;
	unsigned int dwWidth;
	long lPitch;
	unsigned int dwDepth;
	unsigned int dwMipMapCount;
	unsigned int dwAlphaBitDepth;
	unsigned int dwReserved;
	void *lpSurface;
	struct dds_colorkey ddckCKDestOverlay;
	struct dds_colorkey ddckCKDestBlt;
	struct dds_colorkey ddckCKSrcOverlay;
	struct dds_colorkey ddckCKSrcBlt;
	unsigned int dwPFSize;
	unsigned int dwPFFlags;
	unsigned int dwFourCC;
	unsigned int dwRGBBitCount;
	unsigned int dwRBitMask;
	unsigned int dwGBitMask;
	unsigned int dwBBitMask;
	unsigned int dwRGBAlphaBitMask;
	unsigned int dwCaps;
	unsigned int dwCaps2;
	unsigned int dwCaps3;
	unsigned int dwVolumeDepth;
	unsigned int dwTextureStage;
};

struct dds_color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

enum {
	DDS_ERROR = -1,
	DDS_RGB,
	DDS_RGBA,
	DDS_DXT1,
	DDS_DXT2,
	DDS_DXT3,
	DDS_DXT4,
	DDS_DXT5
};

enum {
	DDPF_ALPHAPIXELS = 0x01,
	DDPF_FOURCC = 0x04,
	DDPF_RGB = 0x40,
	DDPF_RGBA = 0x41
};

unsigned char *texture_load_dds(const char *name,int *width,int *height) {
	struct dds_header header;
	FILE *file = fopen(name,"rb");
	int format = DDS_ERROR;
	int x,y,i,j;
	if(!file) {
		fprintf(stderr,"error open DDS file \"%s\"\n",name);
		return NULL;
	}
	fread(&header,sizeof(struct dds_header),1,file);
	if(header.magic != ('D' | 'D' << 8 | 'S' << 16 | ' ' << 24)) {
		printf("error load DDS file \"%s\": wrong magic\n",name);
		fclose(file);
		return NULL;
	}
	*width = header.dwWidth;
	*height = header.dwHeight;
	if(header.dwPFFlags & DDPF_FOURCC) {
		switch(header.dwFourCC) {
			case ('D' | 'X' << 8 | 'T' << 16 | '1' << 24): format = DDS_DXT1; break;
			case ('D' | 'X' << 8 | 'T' << 16 | '2' << 24): format = DDS_DXT2; break;
			case ('D' | 'X' << 8 | 'T' << 16 | '3' << 24): format = DDS_DXT3; break;
			case ('D' | 'X' << 8 | 'T' << 16 | '4' << 24): format = DDS_DXT4; break;
			case ('D' | 'X' << 8 | 'T' << 16 | '5' << 24): format = DDS_DXT5; break;
		}
	}
	else if(header.dwPFFlags == DDPF_RGB && header.dwRGBBitCount == 24) format = DDS_RGB;
	else if(header.dwPFFlags == DDPF_RGBA && header.dwRGBBitCount == 32) format = DDS_RGBA;
	if(format == DDS_ERROR) {
		fprintf(stderr,"error load DDS file \"%s\": unknown format 0x%x RGB %d\n",name,header.dwPFFlags,header.dwRGBBitCount);
		fclose(file);
		return NULL;
	}
	if(format == DDS_DXT2 || format == DDS_DXT4) {
		fprintf(stderr,"error load DDS file \"%s\": DXT2 or DXT4\n",name);
		fclose(file);
		return NULL;
	}
	unsigned char *data = (unsigned char*)malloc(sizeof(unsigned char) * *width * *height * 4);
	if(format == DDS_RGB) {
		int size = *width * *height * 3;
		unsigned char *buf = (unsigned char*)malloc(sizeof(unsigned char) * size);
		fread(buf,size,1,file);
		unsigned char *src = buf;
		unsigned char *dest = data;
		for(y = 0; y < *height; y++) {
			for(x = 0; x < *width; x++) {
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = 255;
			}
		}
		free(buf);
	} else if(format == DDS_RGBA) {
		int size = *width * *height * 4;
		unsigned char *buf = (unsigned char*)malloc(sizeof(unsigned char) * size);
		fread(buf,size,1,file);
		unsigned char *src = buf;
		unsigned char *dest = data;
		for(y = 0; y < *height; y++) {
			for(x = 0; x < *width; x++) {
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
			}
		}
		free(buf);
	} else {
		unsigned char *buf = (unsigned char*)malloc(sizeof(unsigned char) * *width * *height);
		unsigned char *src = buf;
		fread(buf,*width * *height,1,file);
		for(y = 0; y < *height; y += 4) {
			for(x = 0; x < *width; x += 4) {
				unsigned long long alpha = 0;
				unsigned int a0 = 0;
				unsigned int a1 = 0;
				unsigned int c0;
				unsigned int c1;
				struct dds_color color[4];
				if(format == DDS_DXT3) {
					alpha = *(unsigned long long*)src;
					src += 8;
				} else if(format == DDS_DXT5) {
					alpha=  (*(unsigned long long*)src) >> 16;
					a0 = src[0];
					a1 = src[1];
					src += 8;
				}
				c0 = *(unsigned short*)(src + 0);
				c1 = *(unsigned short*)(src + 2);
				src += 4;
				color[0].r = ((c0 >> 11) & 0x1f) << 3;
				color[0].g = ((c0 >> 5) & 0x3f) << 2;
				color[0].b = (c0 & 0x1f) << 3;
				color[1].r = ((c1 >> 11) & 0x1f) << 3;
				color[1].g = ((c1 >> 5) & 0x3f) << 2;
				color[1].b = (c1 & 0x1f) << 3;
				if(c0 > c1) {
					color[2].r = (color[0].r * 2 + color[1].r) / 3;
					color[2].g = (color[0].g * 2 + color[1].g) / 3;
					color[2].b = (color[0].b * 2 + color[1].b) / 3;
					color[3].r = (color[0].r + color[1].r * 2) / 3;
					color[3].g = (color[0].g + color[1].g * 2) / 3;
					color[3].b = (color[0].b + color[1].b * 2) / 3;
				} else {
					color[2].r = (color[0].r + color[1].r) / 2;
					color[2].g = (color[0].g + color[1].g) / 2;
					color[2].b = (color[0].b + color[1].b) / 2;
					color[3].r = 0;
					color[3].g = 0;
					color[3].b = 0;
				}
				for(i = 0; i < 4; i++) {
					unsigned int index = *src++;
					unsigned char *dest = data + (*width * (y + i) + x) * 4;
					for(j = 0; j < 4; j++) {
						*dest++ = color[index & 0x03].r;
						*dest++ = color[index & 0x03].g;
						*dest++ = color[index & 0x03].b;
						if(format == DDS_DXT1) {
							*dest++ = ((index & 0x03) == 3 && c0 <= c1) ? 0 : 255;
						} else if(format == DDS_DXT3) {
							*dest++ = (alpha & 0x0f) << 4;
							alpha >>= 4;
						} else if(format == DDS_DXT5) {
							unsigned int a = alpha & 0x07;
							if(a == 0) *dest++ = a0;
							else if(a == 1) *dest++ = a1;
							else if(a0 > a1) *dest++ = ((8 - a) * a0 + (a - 1) * a1) / 7;
							else if(a > 5) *dest++ = (a == 6) ? 0 : 255;
							else *dest++ = ((6 - a) * a0 + (a - 1) * a1) / 5;
							alpha >>= 3;
						} else *dest++ = 255;
						
						index >>= 2;
					}
				}
			}
		}
		free(buf);
	}
	fclose(file);
	return data;
}
