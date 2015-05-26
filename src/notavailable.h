/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef NOTAVAILABLE_H
#define NOTAVAILABLE_H

#include "formats/format.h"
#include <memory>

class CNotAvailable : public CFormat
{
public:
    CNotAvailable();
    virtual ~CNotAvailable();

    virtual bool Load(const char* filename, unsigned subImage = 0) override;
};

#endif // NOTAVAILABLE_H

