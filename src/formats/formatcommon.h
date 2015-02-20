/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATCOMMON_H_CE3HRGR7
#define FORMATCOMMON_H_CE3HRGR7

#if defined(IMLIB2_SUPPORT)

#include "format.h"

class CFormatCommon : public CFormat
{
public:
    CFormatCommon(const char* lib, const char* name);
    virtual ~CFormatCommon();

    virtual bool Load(const char* filename, unsigned subImage = 0);
    virtual void FreeMemory();
};

#endif

#endif /* end of include guard: FORMATCOMMON_H_CE3HRGR7 */

