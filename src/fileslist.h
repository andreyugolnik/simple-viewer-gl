/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <string>
#include <vector>

class cFilesList final
{
public:
    cFilesList(bool allValid, bool recursive = false);
    ~cFilesList();

    void addFile(const char* path);
    void sortList();
    void locateFile(const char* path);

    const char* getName(int delta = 0);

    void toggleDeletionMark();
    bool isMarkedForDeletion() const;
    void removeMarkedFromDisk();

    size_t getCount() const
    {
        return m_files.size();
    }

    size_t getIndex() const
    {
        return m_position;
    }

private:
    void parseDir();
    void scanDirectory(const std::string& root);
    bool isValidExt(const char* path);

private:
    bool m_allValid;
    bool m_recursive;
    bool m_scanDirectory;
    size_t m_position; // current position in list

    struct sFile
    {
        bool deletionMark;
        std::string path;
    };
    std::vector<sFile> m_files;
};
