/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "fileslist.h"
#include <iostream>
#include <dirent.h>

CFilesList::CFilesList(const char* file, bool recursive)
    : m_listCreated(false)
    , m_recursive(recursive)
    , m_position(0)
    , m_removeCurrent(false)
    , m_allValid(false)
{
    if(file != 0)
    {
        m_files.push_back(file);
    }
}

CFilesList::~CFilesList()
{
}

bool CFilesList::ParseDir()
{
    if(m_listCreated == true)
    {
        return true;
    }

    std::string initial_file = m_files[0];

    // get base directory
    std::string dir, name;
    size_t pos	= m_files[0].find_last_of('/');
    if(std::string::npos != pos)
    {
        dir = m_files[0].substr(0, pos);
        if(pos + 1 != std::string::npos)
        {
            name = m_files[0].substr(pos + 1);
        }
    }
    else {
        dir = ".";
        name = m_files[0];
    }

    // remove initial image from list (it will be added again below)
    m_files.clear();

    if(true == scanDirectory(dir))
    {
        // sorting images by names
        std::sort(m_files.begin(), m_files.end(), CComparator());

        // search startup image index in sorted list
        size_t len = name.length();
        size_t count = m_files.size();
        for(size_t i = 0; i < count; i++)
        {
            size_t slen	= m_files[i].length();
            if(slen >= len && m_files[i].substr(slen - len, len) == name)
            {
                m_position = i;
                break;
            }
        }

        // push back initial file
        if(count == 0)
        {
            m_files.push_back(initial_file);
        }

        m_listCreated = true;

        return true;
    }

    return false;
}

bool CFilesList::scanDirectory(const std::string& dir)
{
    struct dirent** namelist;
    int n = scandir(dir.c_str(), &namelist, filter, alphasort);
    if(n < 0)
    {
        std::cout << "Can't scan \"" << dir << "\" " << n << "." << std::endl;
        return false;
    }
    else
    {
        int count = 0;
        while(n--)
        {
            std::string path(dir);
            path += "/";
            path += namelist[n]->d_name;

            // skip non non readable files/dirs
            DIR* d = opendir(path.c_str());
            if(d != 0)
            {
                closedir(d);
                if(m_recursive == true)
                {
                    scanDirectory(path);
                }
            }
            else if(isValidExt(path) == true)
            {
                m_files.push_back(path);
                count++;
            }
            free(namelist[n]);
        }
        if(count > 0)
        {
            std::cout << "Scaning \"" << dir << "\" directory... " << count << " images found." << std::endl;
        }
        free(namelist);
    }

    return true;
}

#if defined(__APPLE__)
int CFilesList::filter(struct dirent* p)
#else
int CFilesList::filter(const struct dirent* p)
#endif
{
    // skip . and ..
#define DOT_OR_DOTDOT(base)	(base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))
    if(DOT_OR_DOTDOT(p->d_name))
    {
        return 0;
    }

    return 1;
}

bool CFilesList::isValidExt(const std::string& path)
{
    if(m_allValid)
    {
        return true;
    }

    std::string s(path);
    std::transform(s.begin(), s.end(), s.begin(), tolower);

    // skip file without extension
    size_t pos	= s.find_last_of('.');
    if(std::string::npos == pos)
    {
        return false;
    }

    // skip non image file (detect by extension)

    const char* ext[] =
    {
        ".jpeg", ".jpg", ".png", ".psd", ".pnm", ".bmp", ".xpm", ".gif",
        ".tga", ".targa", ".tiff", ".tif", ".ico", ".lbm", ".id3", ".argb",
        ".xwd", ".dds",
    };
    for(size_t i = 0; i < sizeof(ext) / sizeof(const char*); i++)
    {
        if(s.substr(pos) == ext[i])
        {
            return true;
        }
    }

    return false;
}

const char* CFilesList::GetName(int delta)
{
    if(delta != 0 && m_removeCurrent == true)
    {
        m_removeCurrent = false;
        if(0 == unlink(m_files[m_position].c_str()))
        {
            std::cout << "File '" << m_files[m_position] << "' has been removed from disk." << std::endl;
        }
        // remove path from list
        m_files.erase(m_files.begin() + m_position);
        if(delta > 0)
        {
            delta = 0;
        }
    }

    size_t count = m_files.size();
    if(count > 0)
    {
        if(delta == 0 && count == 1 && isValidExt(m_files[m_position]) == false)
        {
            if(false == ParseDir())
            {
                return 0;
            }
        }

        m_position += delta;
        if(m_position < 0)
        {
            m_position = count - 1;
        }
        m_position %= count;

        return m_files[m_position].c_str();
    }

    return 0;
}
//
//void CFilesList::RemoveFromList() {
//	size_t count	= m_files.size();
//	if(count > (size_t)m_position) {
//		m_files.erase(m_files.begin() + m_position);
//	}
//}

void CFilesList::RemoveFromDisk()
{
    m_removeCurrent = true;
}

