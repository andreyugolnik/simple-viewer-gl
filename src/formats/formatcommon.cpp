/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#if defined(IMLIB2_SUPPORT)

#include "formatcommon.h"
#include <string.h>

CFormatCommon* g_this = 0;

CFormatCommon::CFormatCommon(const char* lib, const char* name)
    : CFormat(lib, name)
    , m_image(0)
{
    g_this = this;
    imlib_context_set_progress_function(callbackProgress);
    imlib_context_set_progress_granularity(10);	// update progress each 10%
}

CFormatCommon::~CFormatCommon()
{
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

    if(id < sizeof(errors) / sizeof(errors[0]))
    {
        return errors[id];
    }
    return "not listed";
}

bool CFormatCommon::Load(const char* filename, unsigned /*subImage*/)
{
    cFile file;
    if(!file.open(filename))
    {
        return false;
    }

    m_size = file.getSize();

    // try to load image from disk
    Imlib_Load_Error error_return;
    m_image = imlib_load_image_with_error_return(filename, &error_return);
    if(m_image == 0)
    {
        printf(": error loading file '%s' (error %s)"
                , filename
                , toErrorString(error_return));
        return false;
    }

    imlib_context_set_image(m_image);

    m_width = imlib_image_get_width();
    m_height = imlib_image_get_height();
    m_pitch = 4 * m_width;
    m_bpp = 32; // Imlib2 always has 32-bit buffer, but sometimes alpha not used
    m_bppImage = (imlib_image_has_alpha() == 1 ? 32 : 24);

    m_bitmap.resize(m_pitch * m_height);
    memcpy(&m_bitmap[0], imlib_image_get_data_for_reading_only(), m_bitmap.size());

    m_format = GL_BGRA;

    return true;
}

void CFormatCommon::FreeMemory()
{
    if(m_image)
    {
        imlib_free_image();
        m_image = 0;
    }

    CFormat::FreeMemory();
}

int CFormatCommon::callbackProgress(void* /*p*/, char percent, int /*a*/, int /*b*/, int /*c*/, int /*d*/)
{
    g_this->progress(percent);
    return 1;
}

#endif

