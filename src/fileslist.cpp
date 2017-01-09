/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "fileslist.h"

#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <unistd.h>

CFilesList::CFilesList(const char* initialFile, bool allValid, bool recursive)
    : m_initialFile(initialFile)
    , m_allValid(allValid)
    , m_recursive(recursive)
    , m_removeCurrent(false)
    , m_position(0)
{
    if (initialFile != nullptr)
    {
        addFile(initialFile);
    }
}

CFilesList::~CFilesList()
{
}

static std::string GetBaseDir(const char* path)
{
    std::string dir = path;
    size_t pos = dir.find_last_of('/');
    if (std::string::npos != pos)
    {
        dir = dir.substr(0, pos);
    }
    else
    {
        dir = ".";
    }

    return dir;
}

void CFilesList::parseDir()
{
    if (m_initialFile != nullptr)
    {
        m_files.clear();

        scanDirectory(GetBaseDir(m_initialFile));

        sortList();
        locateFile(m_initialFile);

        m_initialFile = nullptr;
    }
}

void CFilesList::addFile(const char* path)
{
    if (isValidExt(path) == true)
    {
        m_files.push_back(path);
    }
}

void CFilesList::sortList()
{
    std::sort(m_files.begin(), m_files.end(), [](const std::string & a, const std::string & b) -> bool
    {
        std::string aa(a);
        std::string bb(b);
        std::transform(aa.begin(), aa.end(), aa.begin(), tolower);
        std::transform(bb.begin(), bb.end(), bb.begin(), tolower);
        return aa < bb;
    });

    m_files.erase(std::unique(m_files.begin(), m_files.end()), m_files.end());
}

void CFilesList::locateFile(const char* path)
{
    std::string name = path;

    size_t len = name.length();
    size_t count = m_files.size();
    for (size_t i = 0; i < count; i++)
    {
        size_t slen = m_files[i].length();
        if (slen >= len && m_files[i].substr(slen - len, len) == name)
        {
            m_position = i;
            break;
        }
    }
}

void CFilesList::scanDirectory(const std::string& root)
{
    struct dirent** namelist;
    int n = scandir(root.c_str(), &namelist, filter, alphasort);
    if (n < 0)
    {
        // printf("Can't scan \"%s\" %d.\n", root.c_str(), n);
    }
    else
    {
        size_t count = 0;
        while (n--)
        {
            std::string path(root);
            path += "/";
            path += namelist[n]->d_name;

            // skip non non readable files/dirs
            DIR* dir = opendir(path.c_str());
            if (dir != nullptr)
            {
                closedir(dir);
                if (m_recursive == true)
                {
                    scanDirectory(path);
                }
            }
            else if (isValidExt(path) == true)
            {
                m_files.push_back(path);
                count++;
            }
            free(namelist[n]);
        }
        // if (count > 0)
        // {
            // printf("Scaning \"%s\" directory... %zu images found.\n", root.c_str(), count);
        // }
        free(namelist);
    }
}

int CFilesList::filter(const struct dirent* p)
{
    // skip . and ..
#define DOT_OR_DOTDOT(base) (base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))
    if (DOT_OR_DOTDOT(p->d_name))
    {
        return 0;
    }

    return 1;
}

bool CFilesList::isValidExt(const std::string& path)
{
    if (m_allValid)
    {
        return true;
    }

    std::string s(path);
    std::transform(s.begin(), s.end(), s.begin(), tolower);

    // skip file without extension
    size_t pos  = s.find_last_of('.');
    if (std::string::npos == pos)
    {
        return false;
    }

    // skip non image file (detect by extension)

    const char* ext[] =
    {
#if defined(IMLIB2_SUPPORT)
        ".bmp",
#endif
        ".jpeg", ".jpg", ".png", ".psd", ".pnm",
        ".xpm", ".gif",
        ".tga", ".targa", ".tiff", ".tif", ".ico", ".lbm", ".id3", ".argb",
        ".xwd", ".dds", ".raw", ".ppm", ".pvr", ".pvrtc",
        ".scr",
        ".webp",
    };
    for (size_t i = 0; i < sizeof(ext) / sizeof(ext[0]); i++)
    {
        if (s.substr(pos) == ext[i])
        {
            return true;
        }
    }

    return false;
}

const char* CFilesList::GetName(int delta)
{
    if (delta != 0)
    {
        parseDir();

        if (m_removeCurrent == true)
        {
            m_removeCurrent = false;
            if (0 == unlink(m_files[m_position].c_str()))
            {
                printf("File '%s' has been removed from disk.\n", m_files[m_position].c_str());
            }

            // remove path from list
            m_files.erase(m_files.begin() + m_position);
            if (delta > 0)
            {
                delta = 0;
            }
        }
    }

    size_t count = m_files.size();
    if (count > 0)
    {
        // if (delta == 0 && count == 1 && isValidExt(m_files[m_position]) == false)
        // {
        // if (false == ParseDir())
        // {
        // return nullptr;
        // }
        // }

        m_position += delta;
        if (m_position < 0)
        {
            m_position = count - 1;
        }
        m_position %= count;

        return m_files[m_position].c_str();
    }

    return nullptr;
}
//
//void CFilesList::RemoveFromList() {
//  size_t count    = m_files.size();
//  if(count > (size_t)m_position) {
//      m_files.erase(m_files.begin() + m_position);
//  }
//}

void CFilesList::RemoveFromDisk()
{
    m_removeCurrent = true;
}
