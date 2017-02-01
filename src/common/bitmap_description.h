/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "buffer.h"

#include <string>
#include <GLFW/glfw3.h>

struct sBitmapDescription
{
    void reset()
    {
        bitmap.clear();
        format      = GL_RGB;
        bpp         = 0;
        pitch       = 0;
        width       = 0;
        height      = 0;

        bppImage    = 0;
        size        = -1;

        images      = 0;
        current     = 0;

        isAnimation = false;
        delay       = 0;

        info.clear();
    }

    // buffer related
    Buffer bitmap;
    GLenum format     = GL_RGB;
    unsigned bpp      = 0;
    unsigned pitch    = 0;
    unsigned width    = 0;
    unsigned height   = 0;

    // file related
    unsigned bppImage = 0;  // bit per pixel of original image
    long size         = -1; // file size on disk

    unsigned images   = 0;
    unsigned current  = 0;

    bool isAnimation  = false;
    unsigned delay    = 0; // frame animation delay

    std::string info;       // additional info, such as EXIF
};
