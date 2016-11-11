/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#ifndef FORMATPVR_H_BD4C879BFA
#define FORMATPVR_H_BD4C879BFA

#include "format.h"

class cFileInterface;

class cFormatPvr : public CFormat
{
public:
    cFormatPvr(const char* lib, const char* name, iCallbacks* callbacks);
    virtual ~cFormatPvr();

    virtual bool isSupported(cFile& file, Buffer& buffer) const;
    virtual bool Load(const char* filename, unsigned subImage = 0);

private:
    bool readPvr(cFileInterface& file);
};

#endif // FORMATPVR_H_BD4C879BFA

