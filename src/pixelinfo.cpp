/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "pixelinfo.h"
#include "img-pointer-cross.c"

#include <sstream>
#include <iomanip>

const int BORDER	= 4;
const int ALPHA		= 200;
const int FONT_HEIGHT	= 13;
const int FRAME_DELTA	= 10;
const int LINES_COUNT	= 4;

CPixelInfo::CPixelInfo()
    : m_visible(false)
{
    memset(&m_pixelInfo, 0, sizeof(m_pixelInfo));
}

CPixelInfo::~CPixelInfo()
{
}

void CPixelInfo::Init()
{
    m_bg.reset(new CQuad(0, 0));
    m_bg->SetColor(0, 0, 0, ALPHA);

    int format = (imgPointerCross.bytes_per_pixel == 3 ? GL_RGB : GL_RGBA);
    m_pointer.reset(new CQuadSeries(imgPointerCross.width, imgPointerCross.height, imgPointerCross.pixel_data, format));
    m_pointer->Setup(21, 21, 10);
    SetCursor(0);

    m_ft.reset(new CFTString(FONT_HEIGHT));
    m_ft->SetColor(255, 255, 255, ALPHA);
}

void CPixelInfo::Update(const PixelInfo* _p)
{
    m_pixelInfo = *_p;

    // TODO correct cursor position according scale factor
    //if(checkBoundary() == true)
    {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        if(_p->rc.IsSet() == true)
        {
            x = std::min(_p->rc.x1, _p->rc.x2);
            y = std::min(_p->rc.y1, _p->rc.y2);
            w = _p->rc.GetWidth();
            h = _p->rc.GetHeight();
        }

        static char info[200];
        snprintf(info, sizeof(info), "pos: %.0f x %.0f\nargb: 0x%.2X%.2X%.2X%.2X\nrect: %d x %d\nsize: %d, %d -> %d, %d"
                , _p->img.x, _p->img.y
                , _p->a, _p->r, _p->g, _p->b
                , w, h
                , x, y, x + w, y + h);
        m_ft->Update(info);
    }
    //else
    //{
        //m_ft->Update("out of image");
    //}
}

void CPixelInfo::Render()
{
    if(m_visible == true)
    {
        m_pointer->Render(m_pixelInfo.cursor.x - 10, m_pixelInfo.cursor.y - 10);

        if(checkBoundary() == true)
        {
            int frameWidth = m_ft->GetStringWidth() + 2 * BORDER;
            int frameHeight = FONT_HEIGHT * LINES_COUNT + 2 * BORDER;

            int cursor_x = m_pixelInfo.cursor.x + FRAME_DELTA;
            int cursor_y = m_pixelInfo.cursor.y + FRAME_DELTA;
            if(cursor_x > m_window.x - frameWidth)
            {
                //cursorx = m_window.x - frameWidth;
                cursor_x = m_pixelInfo.cursor.x - FRAME_DELTA - frameWidth;
            }
            if(cursor_y > m_window.y - frameHeight)
            {
                //cursory = m_window.y - frameHeight;
                cursor_y = m_pixelInfo.cursor.y - FRAME_DELTA - frameHeight;
            }

            m_bg->SetSpriteSize(frameWidth, frameHeight);
            m_bg->Render(cursor_x, cursor_y);

            m_ft->Render(cursor_x + BORDER, cursor_y + FONT_HEIGHT);
        }
    }
}

bool CPixelInfo::checkBoundary() const
{
    if(
            m_pixelInfo.img.x >= 0
            && m_pixelInfo.img.x < m_pixelInfo.w
            && m_pixelInfo.img.y >= 0
            && m_pixelInfo.img.y < m_pixelInfo.h
      )
    {
        return true;
    }

    return false;
}

void CPixelInfo::SetCursor(int cursor)
{
    m_pointer->SetFrame(cursor);
}

