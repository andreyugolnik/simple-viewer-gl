/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FORMATCOMMON_H
#define FORMATCOMMON_H

#if defined(IMLIB2_SUPPORT)

#include <Imlib2.h>
#include "format.h"

class CFormatCommon : public CFormat
{
public:
    CFormatCommon(Callback callback, const char* lib, const char* name);
    virtual ~CFormatCommon();

    virtual bool Load(const char* filename, unsigned subImage = 0);
    virtual void FreeMemory();

private:
    Imlib_Image m_image;

private:
    static int callbackProgress(void*, char percent, int, int, int, int);
};

#endif

#endif // FORMATCOMMON_H

