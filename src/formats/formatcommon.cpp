/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#if defined(IMLIB2_SUPPORT)

#include "formatcommon.h"
#include "../common/bitmap_description.h"
#include "../common/file.h"

#include <cstring>
#include <Imlib2.h>

// static cFormatCommon* g_this = nullptr;

// static int callbackProgress(void* [>p*/, char percent, int /*a*/, int /*b*/, int /*c*/, int /*d<])
// {
    // g_this->updateProgress(percent * 0.01f);
    // return 1;
// }

cFormatCommon::cFormatCommon(const char* lib, iCallbacks* callbacks)
    : cFormat(lib, callbacks)
{
    // g_this = this;
    // imlib_context_set_progress_function(callbackProgress);
    // imlib_context_set_progress_granularity(10); // update progress each 10%
}

cFormatCommon::~cFormatCommon()
{
    // g_this = nullptr;
}

static const char* toErrorString(unsigned id)
{
    static const char* errors[] =
    {
        "none",
        "file_does_not_exist",
        "file_is_directory",
        "permission_denied_to_read",
        "no_loader_for_file_format",
        "path_too_long",
        "path_component_non_existant",
        "path_component_not_directory",
        "path_points_outside_address_space",
        "too_many_symbolic_links",
        "out_of_memory",
        "out_of_file_descriptors",
        "permission_denied_to_write",
        "out_of_disk_space",
        "unknow"
    };

    if (id < sizeof(errors) / sizeof(errors[0]))
    {
        return errors[id];
    }
    return "not listed";
}

bool cFormatCommon::LoadImpl(const char* filename, sBitmapDescription& desc)
{
    cFile file;
    if (!file.open(filename))
    {
        return false;
    }

    desc.size = file.getSize();

    // try to load image from disk
    Imlib_Load_Error error_return;
    Imlib_Image image = imlib_load_image_with_error_return(filename, &error_return);
    if (image == nullptr)
    {
        ::printf("(EE) Error loading file '%s' (error %s)\n"
               , filename
               , toErrorString(error_return));
        return false;
    }

    imlib_context_set_image(image);

    desc.width = imlib_image_get_width();
    desc.height = imlib_image_get_height();
    desc.pitch = 4 * desc.width;
    desc.bpp = 32; // Imlib2 always has 32-bit buffer, but sometimes alpha not used
    desc.bppImage = (imlib_image_has_alpha() == 1 ? 32 : 24);

    desc.bitmap.resize(desc.pitch * desc.height);
    ::memcpy(desc.bitmap.data(), imlib_image_get_data_for_reading_only(), desc.bitmap.size());

    desc.format = GL_BGRA;

    auto ext = ::strrchr(filename, '.');
    m_formatName = ext != nullptr ? (ext + 1) : "";

    imlib_free_image();

    return true;
}

#endif
