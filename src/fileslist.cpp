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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

cFilesList::cFilesList(bool allValid, bool recursive)
    : m_allValid(allValid)
    , m_recursive(recursive)
    , m_removeCurrent(false)
    , m_scanDirectory(false)
    , m_position(0)
{
}

cFilesList::~cFilesList()
{
}

namespace
{

    std::string GetBaseDir(const char* path)
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

    int Filter(const dirent* dir)
    {
        // skip . and ..
        auto p = dir->d_name;
        return (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0'))) ? 0 : 1;
    }

}

void cFilesList::parseDir()
{
    if (m_scanDirectory && m_position < m_files.size())
    {
        m_scanDirectory = false;

        const auto current = m_files[m_position];
        const auto path = current.c_str();

        scanDirectory(GetBaseDir(path));

        sortList();
        locateFile(path);
    }
}

void cFilesList::addFile(const char* path)
{
    path = ::realpath(path, nullptr);

    m_scanDirectory = m_files.size() == 0;

    if (isValidExt(path) == true)
    {
        m_files.push_back(path);
    }
    else if (m_scanDirectory)
    {
        m_scanDirectory = false;

        scanDirectory(path);

        sortList();
    }
}

void cFilesList::sortList()
{
    std::sort(m_files.begin(), m_files.end(), [](const std::string & a, const std::string & b) -> bool
    {
        std::string aa(a);
        std::string bb(b);
        std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
        std::transform(bb.begin(), bb.end(), bb.begin(), ::tolower);
        return aa < bb;
    });

    m_files.erase(std::unique(m_files.begin(), m_files.end()), m_files.end());

    // ::printf("Sorted unique file %u:\n", (uint32_t)m_files.size());
    // for (auto& f : m_files)
    // {
        // ::printf("'%s'\n", f.c_str());
    // }
}

void cFilesList::locateFile(const char* path)
{
    const std::string name = path;
    const auto len = name.length();
    for (size_t i = 0, count = m_files.size(); i < count; i++)
    {
        const auto slen = m_files[i].length();
        if (slen >= len && m_files[i].substr(slen - len, len) == name)
        {
            m_position = (unsigned)i;
            break;
        }
    }
}

void cFilesList::scanDirectory(const std::string& root)
{
    // ::printf("scan: '%s'\n", root.c_str());

    dirent** namelist;
    int n = ::scandir(root.c_str(), &namelist, Filter, alphasort);
    if (n >= 0)
    {
        while (n--)
        {
            std::string path(root);
            path += "/";
            path += namelist[n]->d_name;

            // skip non non readable files/dirs
            DIR* dir = ::opendir(path.c_str());
            if (dir != nullptr)
            {
                ::closedir(dir);
                if (m_recursive == true)
                {
                    scanDirectory(path);
                }
            }
            else if (isValidExt(path.c_str()) == true)
            {
                m_files.push_back(path);
            }

            ::free(namelist[n]);
        }

        ::free(namelist);
    }
}

bool cFilesList::isValidExt(const char* path)
{
    if (m_allValid)
    {
        return true;
    }

    auto point = ::strrchr(path, '.');
    if (point == nullptr)
    {
        return false;
    }

    std::string ext = point;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    const char* ValidExts[] =
    {
        // external loaders
        ".exr", // openexr
        ".jpeg", ".jpg", ".jpe", ".jfif", // libjpeg
        ".tiff", ".tif", // libtiff
        ".png", // libpng
        ".gif", // giflib
        ".webp", // webp
        ".lbm", ".id3", ".argb", // imlib2

        // internal loaders
        ".age",
        ".raw",
        ".pvr", ".pvrtc",
        ".bmp",
        ".xpm",
        ".psd",
        ".ico",
        ".xwd",
        ".dds",
        ".tga", ".targa",
        ".pnm", ".pbm", ".pgm", ".ppm",
        ".scr", ".atr", ".bsc", ".ifl", ".bmc4", ".mc", ".s", ".$c",
    };

    for (const char* e : ValidExts)
    {
        if (ext == e)
        {
            return true;
        }
    }

    return false;
}

const char* cFilesList::getName(int delta)
{
    if (delta != 0)
    {
        parseDir();

        if (m_removeCurrent == true)
        {
            m_removeCurrent = false;
            const auto path = m_files[m_position].c_str();
            if (0 == ::unlink(path))
            {
                ::printf("(II) File '%s' has been removed from disk.\n", path);
            }

            // remove path from list
            m_files.erase(m_files.begin() + m_position);
            if (delta > 0)
            {
                delta = 0;
            }
        }
    }

    const size_t count = m_files.size();
    if (count > 0)
    {
        m_position = (m_position + count + delta) % count;
        return m_files[m_position].c_str();
    }

    return nullptr;
}

void cFilesList::removeFromDisk()
{
    m_removeCurrent = true;
}
