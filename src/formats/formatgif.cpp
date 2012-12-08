/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "formatgif.h"
#include <string.h>
#include <iostream>

CFormatGif::CFormatGif(Callback callback, const char* _lib, const char* _name)
    : CFormat(callback, _lib, _name)
{
}

CFormatGif::~CFormatGif()
{
}

bool CFormatGif::Load(const char* filename, unsigned subImage)
{
    if(openFile(filename) == false)
    {
        return false;
    }
    fclose(m_file);

    GifFileType* file = DGifOpenFileName(filename);
    if(file == 0)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        return false;
    }

    int res = DGifSlurp(file);
    if(res != GIF_OK || file->ImageCount < 1)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        DGifCloseFile(file);
        return false;
    }

    m_subImage = std::max<unsigned>(subImage, 0);
    m_subImage = std::min<unsigned>(m_subImage, file->ImageCount - 1);
    m_subCount = file->ImageCount;

    const SavedImage* image = &file->SavedImages[m_subImage];

    // place next frame abov previous
    if(!m_subImage || m_bitmap.empty())
    {
        m_width = file->SWidth;
        m_height = file->SHeight;
        m_pitch = file->SWidth * 4;
        m_bpp = 32;
        m_bppImage = 8;//file->Image.ColorMap->BitsPerPixel;
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
        cmap = file->SColorMap;
    }
    //if(cmap->ColorCount != (1 << cmap->BitsPerPixel))
    //{
        //std::cout << "Invalid ColorMap" << std::endl;
        //DGifCloseFile(file);
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
    //	if(GIF_ERROR == DGifGetRecordType(file, &recordType)) {
    //		DGifCloseFile(file);
    //		std::cout << "Error Opening GIF image" << std::endl;
    //		return false;
    //	}
    //
    //	std::cout << "Record Type" << (int)recordType << std::endl;

    DGifCloseFile(file);

    return true;
}

void CFormatGif::putPixel(int _pos, const GifColorType* _color, bool _transparent)
{
    if(!m_subImage || !_transparent)
    {
        m_bitmap[_pos + 0] = _color->Red;
        m_bitmap[_pos + 1] = _color->Green;
        m_bitmap[_pos + 2] = _color->Blue;
        m_bitmap[_pos + 3] = (_transparent ? 0 : 255);
    }
}

