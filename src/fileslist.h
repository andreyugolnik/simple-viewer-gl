/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef FILESLIST_H
#define FILESLIST_H

#include <vector>
#include <string>
#include <algorithm>

class CFilesList
{
public:
    CFilesList(const char* file, bool recursive = false);
    virtual ~CFilesList();

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
    std::vector<std::string> m_files;

private:
    bool scanDirectory(const std::string& dir);
    bool isValidExt(const std::string& path);
    static int filter(const struct dirent* p);
};



class CComparator
{
public:
    bool operator()(const std::string& a, const std::string& b)
    {
        std::string aa(a);
        std::string bb(b);
        std::transform(aa.begin(), aa.end(), aa.begin(), tolower);
        std::transform(bb.begin(), bb.end(), bb.begin(), tolower);
        return (aa < bb);
    }
};

#endif // FILESLIST_H

