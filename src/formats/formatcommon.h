/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATCOMMON_H
#define FORMATCOMMON_H

#include <Imlib2.h>
#include "format.h"

class CFormatCommon : public CFormat
{
public:
    CFormatCommon(Callback callback, const char* _lib, const char* _name);
    virtual ~CFormatCommon();

    virtual bool Load(const char* filename, unsigned subImage = 0);
    virtual void FreeMemory();

private:
    Imlib_Image m_image;

private:
    static int callbackProgress(void*, char percent, int, int, int, int);
};

#endif // FORMATCOMMON_H

