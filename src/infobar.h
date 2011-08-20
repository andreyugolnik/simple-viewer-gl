/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef INFOBAR_H
#define INFOBAR_H

#include "ftstring.h"
#include "quad.h"
#include <string>
#include <memory>

typedef struct INFO_BAR
{
    const char* path;
    int index;
    int width, height, bpp;
    float scale;
    int sub_image, sub_count;
    long file_size;
    size_t mem_size;
    int files_count;
} InfoBar;

class CInfoBar
{
public:
    CInfoBar();
    virtual ~CInfoBar();

    void Init();
    void Render();
    bool Visible() const { return m_visible; }
    void Show(bool show = true) { m_visible = show; }
    void Update(const InfoBar* p);
    float GetHeight() const { return (m_visible == true ? m_height : 0); }

private:
    bool m_visible;
    const float m_height;
    const int m_fntSize;
    std::string m_bottominfo;
    std::auto_ptr<CQuad> m_bg;
    std::auto_ptr<CFTString> m_ft;

private:
    int getHumanSize(long* size, std::string& suffix);
};

#endif // INFOBAR_H

