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

        const auto& current = m_files[m_position];
        const auto path = current.path.c_str();

        scanDirectory(GetBaseDir(path));

        sortList();
        locateFile(path);
    }
}

void cFilesList::addFile(const char* path)
{
    const auto fullPath = ::realpath(path, nullptr);
    if (fullPath != nullptr)
    {
        path = fullPath;
    }

    m_scanDirectory = m_files.size() == 0;

    if (isValidExt(path) == true)
    {
        m_files.push_back({ false, path });
    }
    else
    {
        m_scanDirectory = false;

        scanDirectory(path);
    }

    if (fullPath != nullptr)
    {
        ::free(fullPath);
    }
}

void cFilesList::sortList()
{
    std::sort(m_files.begin(), m_files.end(), [](const sFile& a, const sFile& b) -> bool {
        std::string aa(a.path);
        std::string bb(b.path);
        std::transform(aa.begin(), aa.end(), aa.begin(), ::tolower);
        std::transform(bb.begin(), bb.end(), bb.begin(), ::tolower);
        return aa < bb;
    });

    auto last = std::unique(m_files.begin(), m_files.end(), [](const sFile& a, const sFile& b) {
        return a.path == b.path;
    });
    m_files.erase(last, m_files.end());

    // ::printf("Sorted unique file %u:\n", (uint32_t)m_files.size());
    // for (const auto& f : m_files)
    // {
        // ::printf("'%s'\n", f.path.c_str());
    // }
}

void cFilesList::locateFile(const char* path)
{
    const std::string name = path;
    const auto len = name.length();
    for (size_t i = 0, count = m_files.size(); i < count; i++)
    {
        const auto& file = m_files[i];
        const auto slen = file.path.length();
        if (slen >= len && file.path.substr(slen - len, len) == name)
        {
            m_position = i;
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
                m_files.push_back({ false, path });
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

    const char* ValidExts[] = {
        // external loaders
        ".exr",                           // openexr
        ".jpeg", ".jpg", ".jpe", ".jfif", // libjpeg
        ".tiff", ".tif",                  // libtiff
        ".png",                           // libpng
        ".gif",                           // giflib
        ".webp",                          // webp
        ".lbm", ".id3", ".argb",          // imlib2

        // internal loaders
        ".age",                                                                                                      // internal age
        ".raw",                                                                                                      // internal raw
        ".pvr", ".pvrtc",                                                                                            // internal pvr/pvrtc
        ".bmp",                                                                                                      // BMP
        ".xpm",                                                                                                      // X Pixmap
        ".psd",                                                                                                      // Adobe PSD
        ".ai",                                                                                                       // Adobe Illustrator
        ".eps",                                                                                                      // Encapsulated PostScript
        ".ico",                                                                                                      // ICO
        ".xwd",                                                                                                      // X Window Dump
        ".dds",                                                                                                      // Microsoft DDS
        ".tga", ".targa", ".tpic",                                                                                   // Truevision Advanced Raster Graphics Adapter
        ".pnm", ".pbm", ".pgm", ".ppm",                                                                              // Netpbm
        ".scr", ".atr", ".bsc", ".ifl", ".bmc4", ".mc", ".s", ".$c", ".mg1", ".mg2", ".mg4", ".mg8", ".img", ".mgs", // ZX-Spectrum
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
    }

    const auto count = m_files.size();
    if (count > 0)
    {
        m_position = (m_position + count + delta) % count;
        return m_files[m_position].path.c_str();
    }

    return nullptr;
}

void cFilesList::toggleDeletionMark()
{
    if (m_position < m_files.size())
    {
        auto& file = m_files[m_position];
        file.deletionMark = !file.deletionMark;
    }
}

bool cFilesList::isMarkedForDeletion() const
{
    if (m_position < m_files.size())
    {
        auto& file = m_files[m_position];
        return file.deletionMark;
    }
    return false;
}

void cFilesList::removeMarkedFromDisk()
{
    for (size_t i = 0, size = m_files.size(); i < size;)
    {
        const auto& file = m_files[i];
        if (file.deletionMark)
        {
            const auto path = file.path.c_str();
            if (::unlink(path) == 0)
            {
                ::printf("(II) File '%s' has been removed from disk.\n", path);
            }

            if (i < m_position)
            {
                m_position--;
            }

            m_files.erase(m_files.begin() + i);

            size--;
        }
        else
        {
            i++;
        }
    }
}
