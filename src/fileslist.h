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

class CFilesList final
{
public:
    CFilesList(const char* file, bool recursive = false);
    ~CFilesList();

    void setAllValid(bool allValid) { m_allValid = allValid; }
    bool ParseDir();
    const char* GetName(int delta = 0);
    void RemoveFromDisk();
    size_t GetCount() const { return m_files.size(); }
    int GetIndex() const { return m_position; }

private:
    bool m_listCreated;
    bool m_recursive;
    int m_position;	// current position in list
    bool m_removeCurrent;
    bool m_allValid;
    std::vector<std::string> m_files;

private:
    bool scanDirectory(const std::string& dir);
    bool isValidExt(const std::string& path);
    static int filter(const struct dirent* p);
};

