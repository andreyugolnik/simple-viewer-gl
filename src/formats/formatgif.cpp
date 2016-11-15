/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "formatgif.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <iostream>
#include <gif_lib.h>

CFormatGif::CFormatGif(const char* lib, const char* name, iCallbacks* callbacks)
    : CFormat(lib, name, callbacks)
{
}

CFormatGif::~CFormatGif()
{
}

bool CFormatGif::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    m_filename = filename;
    return load(0, desc);
}

bool CFormatGif::LoadSubImageImpl(unsigned current, sBitmapDescription& desc)
{
    return load(current, desc);
}

bool CFormatGif::load(unsigned current, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(m_filename.c_str()))
    {
        return false;
    }

    desc.size = file.getSize();
    file.close();

    GifFileType* gif_file = DGifOpenFileName(m_filename.c_str());
    if (gif_file == 0)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        return false;
    }

    int res = DGifSlurp(gif_file);
    if (res != GIF_OK || gif_file->ImageCount < 1)
    {
        std::cout << "Error Opening GIF image" << std::endl;
        DGifCloseFile(gif_file);
        return false;
    }

    desc.images = gif_file->ImageCount;
    desc.current = std::max<unsigned>(current, 0);
    desc.current = std::min<unsigned>(desc.current, desc.images - 1);

    const SavedImage* image = &gif_file->SavedImages[desc.current];

    // place next frame abov previous
    if (!desc.current || desc.bitmap.empty())
    {
        desc.width = gif_file->SWidth;
        desc.height = gif_file->SHeight;
        desc.pitch = gif_file->SWidth * 4;
        desc.bpp = 32;
        desc.bppImage = 8;//gif_file->Image.ColorMap->BitsPerPixel;
        desc.bitmap.resize(desc.pitch * desc.height);
        memset(&desc.bitmap[0], 0, desc.bitmap.size());
        desc.format = GL_RGBA;
    }

    // look for the transparent color extension
    int transparent = -1;
    for (int i = 0; i < image->ExtensionBlockCount; i++)
    {
        ExtensionBlock* eb = image->ExtensionBlocks + i;
        if (eb->Function == 0xF9 && eb->ByteCount == 4)
        {
            bool has_transparency = ((eb->Bytes[0] & 1) == 1);
            if (has_transparency)
            {
                transparent = eb->Bytes[3];
            }
        }
    }

    const ColorMapObject* cmap = image->ImageDesc.ColorMap;
    if (cmap == 0)
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

    if (image->ImageDesc.Interlace)
    {
        // Need to perform 4 passes on the images:
        const int interlacedOffset[] = { 0, 4, 2, 1 };  // The way Interlaced image should
        const int interlacedJumps[] = { 8, 8, 4, 2 };   // be read - offsets and jumps...
        int interlaced_y = 0;

        for (int i = 0; i < 4; i++)
        {
            for (int y = interlacedOffset[i]; y < height; y += interlacedJumps[i])
            {
                for (int x = 0; x < width; x++)
                {
                    int index = image->RasterBits[interlaced_y * width + x];
                    int pos = (y + image->ImageDesc.Top) * desc.pitch + (x + image->ImageDesc.Left) * 4;
                    putPixel(desc, pos, &cmap->Colors[index], (transparent == index));
                }

                updateProgress((float)interlaced_y / height);

                interlaced_y++;
            }
        }
    }
    else
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int index = image->RasterBits[y * width + x];
                int pos = (y + image->ImageDesc.Top) * desc.pitch + (x + image->ImageDesc.Left) * 4;
                putPixel(desc, pos, &cmap->Colors[index], (transparent == index));
            }

            updateProgress((float)y / height);
        }
    }

    // UndefinedRecordType      - something is wrong!
    // ScreenDescRecordType - screen information. As the screen info is automatically read in when the file is open, this should not happen.
    // ImageDescRecordType      - next record is an image descriptor.
    // ExtensionRecordType      - next record is extension block.
    // TerminateRecordType      - last record reached, can close the file.
    //  GifRecordType recordType;
    //  if(GIF_ERROR == DGifGetRecordType(gif_file, &recordType)) {
    //      DGifCloseFile(gif_file);
    //      std::cout << "Error Opening GIF image" << std::endl;
    //      return false;
    //  }
    //
    //  std::cout << "Record Type" << (int)recordType << std::endl;

    DGifCloseFile(gif_file);

    return true;
}

void CFormatGif::putPixel(sBitmapDescription& desc, int pos, const GifColorType* color, bool transparent)
{
    if (!desc.current || !transparent)
    {
        desc.bitmap[pos + 0] = color->Red;
        desc.bitmap[pos + 1] = color->Green;
        desc.bitmap[pos + 2] = color->Blue;
        desc.bitmap[pos + 3] = (transparent ? 0 : 255);
    }
}
