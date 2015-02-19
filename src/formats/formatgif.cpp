/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatgif.h"
#include "file.h"

#include <cstring>
#include <iostream>
#include <gif_lib.h>

CFormatGif::CFormatGif(const char* lib, const char* name)
    : CFormat(lib, name)
{
}

CFormatGif::~CFormatGif()
{
}

bool CFormatGif::Load(const char* filename, unsigned subImage)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();
    file.close();

    GifFileType* gif_file = DGifOpenFileName(filename);
    if(gif_file == 0)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        return false;
    }

    int res = DGifSlurp(gif_file);
    if(res != GIF_OK || gif_file->ImageCount < 1)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        DGifCloseFile(gif_file);
        return false;
    }

    m_subImage = std::max<unsigned>(subImage, 0);
    m_subImage = std::min<unsigned>(m_subImage, gif_file->ImageCount - 1);
    m_subCount = gif_file->ImageCount;

    const SavedImage* image = &gif_file->SavedImages[m_subImage];

    // place next frame abov previous
    if(!m_subImage || m_bitmap.empty())
    {
        m_width = gif_file->SWidth;
        m_height = gif_file->SHeight;
        m_pitch = gif_file->SWidth * 4;
        m_bpp = 32;
        m_bppImage = 8;//gif_file->Image.ColorMap->BitsPerPixel;
        m_bitmap.resize(m_pitch * m_height);
        memset(&m_bitmap[0], 0, m_bitmap.size());
        m_format = GL_RGBA;
    }

    // look for the transparent color extension
    int	transparent = -1;
    for(int i = 0; i < image->ExtensionBlockCount; i++)
    {
        ExtensionBlock* eb = image->ExtensionBlocks + i;
        if(eb->Function == 0xF9 && eb->ByteCount == 4)
        {
            bool has_transparency = ((eb->Bytes[0] & 1) == 1);
            if(has_transparency)
            {
                transparent = eb->Bytes[3];
            }
        }
    }

    const ColorMapObject* cmap = image->ImageDesc.ColorMap;
    if(cmap == 0)
    {
        cmap = gif_file->SColorMap;
    }
    //if(cmap->ColorCount != (1 << cmap->BitsPerPixel))
    //{
        //std::cout << "Invalid ColorMap" << std::endl;
        //DGifCloseFile(gif_file);
        //return false;
    //}
    const int width = image->ImageDesc.Width;
    const int height = image->ImageDesc.Height;

    if(image->ImageDesc.Interlace)
    {
        // Need to perform 4 passes on the images:
        const int interlacedOffset[] = { 0, 4, 2, 1 };	// The way Interlaced image should
        const int interlacedJumps[] = { 8, 8, 4, 2 };	// be read - offsets and jumps...
        int interlaced_y = 0;

        for(int i = 0; i < 4; i++)
        {
            for(int y = interlacedOffset[i]; y < height; y += interlacedJumps[i])
            {
                for(int x = 0; x < width; x++)
                {
                    int index = image->RasterBits[interlaced_y * width + x];
                    int pos = (y + image->ImageDesc.Top) * m_pitch + (x + image->ImageDesc.Left) * 4;
                    putPixel(pos, &cmap->Colors[index], (transparent == index));
                }

                int percent = static_cast<int>(100.0f * interlaced_y / height);
                progress(percent);

                interlaced_y++;
            }
        }
    }
    else
    {
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = image->RasterBits[y * width + x];
                int pos = (y + image->ImageDesc.Top) * m_pitch + (x + image->ImageDesc.Left) * 4;
                putPixel(pos, &cmap->Colors[index], (transparent == index));
            }

            int percent = static_cast<int>(100.0f * y / height);
            progress(percent);
        }
    }

    // UndefinedRecordType		- something is wrong!
    // ScreenDescRecordType	- screen information. As the screen info is automatically read in when the file is open, this should not happen.
    // ImageDescRecordType		- next record is an image descriptor.
    // ExtensionRecordType		- next record is extension block.
    // TerminateRecordType		- last record reached, can close the file.
    //	GifRecordType recordType;
    //	if(GIF_ERROR == DGifGetRecordType(gif_file, &recordType)) {
    //		DGifCloseFile(gif_file);
    //		std::cout << "Error Opening GIF image" << std::endl;
    //		return false;
    //	}
    //
    //	std::cout << "Record Type" << (int)recordType << std::endl;

    DGifCloseFile(gif_file);

    return true;
}

void CFormatGif::putPixel(int pos, const GifColorType* color, bool transparent)
{
    if(!m_subImage || !transparent)
    {
        m_bitmap[pos + 0] = color->Red;
        m_bitmap[pos + 1] = color->Green;
        m_bitmap[pos + 2] = color->Blue;
        m_bitmap[pos + 3] = (transparent ? 0 : 255);
    }
}

