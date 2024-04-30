/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include <version.h>

namespace version
{
    // clang-format off
    const char* getTitle()
    {
        const char* SVGL_Title = "Simple Viewer GL";
        return SVGL_Title;
    }

    int getMajor()
    {
        const int SVGL_VerMajor = 3;
        return SVGL_VerMajor;
    }

    int getMinor()
    {
        const int SVGL_VerMinor = 1;
        return SVGL_VerMinor;
    }

    int getRelease()
    {
        const int SVGL_VerRelease = 10;
        return SVGL_VerRelease;
    }
    // clang-format on

} // namespace version
