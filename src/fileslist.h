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
    cFilesList(const char* initialFile, bool allValid, bool recursive = false);
    ~cFilesList();

    void addFile(const char* path);
    void sortList();
    void locateFile(const char* path);

    const char* GetName(int delta = 0);
    void RemoveFromDisk();
    size_t GetCount() const { return m_files.size(); }
    int GetIndex() const { return m_position; }

private:
    void parseDir();
    void scanDirectory(const std::string& root);
    bool isValidExt(const char* path);

private:
    const char* m_initialFile;
    bool m_allValid;
    bool m_recursive;
    bool m_removeCurrent;
    int m_position; // current position in list
    std::vector<std::string> m_files;
};

