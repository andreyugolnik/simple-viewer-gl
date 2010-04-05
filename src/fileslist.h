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

class CFilesList {
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
	bool isValidExt(const char* path);
	static int filter(const struct dirent* p);
};

#endif // FILESLIST_H
