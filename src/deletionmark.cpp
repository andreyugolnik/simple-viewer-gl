/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "deletionmark.h"
#include "img-na.c"

void cDeletionMark::init()
{
    m_image.reset(new cQuad(imgNa.width, imgNa.height, imgNa.pixel_data, imgNa.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA));
}

void cDeletionMark::render()
{
    m_image->render({ 10.0f, 10.0f });
}
