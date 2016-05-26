/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "progress.h"
#include "img-loading.c"
#include <iostream>
#include <math.h>

void CProgress::Init()
{
    int format = (imgLoading.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
    m_loading.reset(new CQuad(imgLoading.width, imgLoading.height, imgLoading.pixel_data, format));
    m_square.reset(new CQuad(4, 4));
}

void CProgress::Start()
{
    std::cout << "Loading...";
    //m_loadingTime = glutGet(GLUT_ELAPSED_TIME);
}

void CProgress::Render(int percent)
{
    //if(m_loadingTime + 600 < glutGet(GLUT_ELAPSED_TIME))
    //{
        //float w = 0.0f;//(float)glutGet(GLUT_WINDOW_WIDTH);
        //float h = 0.0f;//(float)glutGet(GLUT_WINDOW_HEIGHT);
        //float x = ceil((w - imgLoading.width) / 2);
        //float y = ceil((h - imgLoading.height) / 2);

        //m_loading->Render(x, y);

        //const int count = 20;
        //const float step = imgLoading.width / count;
        //x = ceilf((w - step * count) / 2);
        //for(int i = 0; i < percent / (100 / count); i++)
        //{
            //m_square->Render(x + i * step, y + imgLoading.height);
        //}

        //glutSwapBuffers();
    //}
}

