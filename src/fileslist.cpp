/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "fileslist.h"
#include <algorithm>
#include <dirent.h>
#include <stdlib.h>

CFilesList::CFilesList(const char* file) : m_listCreated(false), m_position(0), m_removeCurrent(false) {
	m_files.push_back(file);
}

CFilesList::~CFilesList() {
}

bool CFilesList::ParseDir() {
	if(m_listCreated == true) {
		return true;
	}

	// get base directory
	std::string dir, name;
	size_t pos	= m_files[0].find_last_of('/');
	if(std::string::npos != pos) {
		dir		= m_files[0].substr(0, pos) + '/';
		name	= m_files[0].substr(pos + 1);
	}
	else {
		dir	= "./";
		name	= m_files[0];
	}

	// remove initial image from list (it will be added again below)
	m_files.clear();

	// parse dir
	struct dirent** namelist;
	int n = scandir(dir.c_str(), &namelist, filter, alphasort);
	if(n < 0) {
		printf("Can't scan dir %s\n", dir.c_str());
		return false;
	}
	else {
		while(n--) {
			std::string path	= dir + namelist[n]->d_name;

			// skip non non readable files/dirs
			DIR* d	= opendir(path.c_str());
			if(d == 0) {
				m_files.push_back(path);
			}
			else {
				closedir(d);
			}
			free(namelist[n]);
		}
		free(namelist);
	}

	// sorting images by names
	std::sort(m_files.begin(), m_files.end());

	// search startup image index in sorted list
	size_t len		= name.length();
	size_t count	= m_files.size();
	for(size_t i = 0; i < count; i++) {
		size_t slen	= m_files[i].length();
		if(slen >= len && m_files[i].substr(slen - len, len) == name) {
			m_position	= i;
//			// update remove image index
//			if(g_nImageToRemove != -1) {
//				g_nImageToRemove	= i;
//			}
			break;
		}
	}

	m_listCreated	= true;

	return true;
}

int CFilesList::filter(const struct dirent* p) {
	// skip . and ..
#define DOT_OR_DOTDOT(base)	(base[0] == '.' && (base[1] == '\0' || (base[1] == '.' && base[2] == '\0')))
	if(DOT_OR_DOTDOT(p->d_name)) {
		return 0;
	}

	std::string	s	= p->d_name;

	// skip file without extension
	size_t pos	= s.find_last_of('.');
	if(std::string::npos == pos) {
		return 0;
	}

	// skip non image file (detect by extension)
	std::transform(s.begin(), s.end(), s.begin(), tolower);

	const char* ext[]	= {
		".jpeg", ".jpg", ".png", ".pnm", ".bmp", ".xpm", ".gif",
		".tga", ".targa", ".tiff", ".lbm", ".id3", ".zlib", ".argb"
	};
	for(size_t i = 0; i < sizeof(ext) / sizeof(const char*); i++) {
		if(s.substr(pos) == ext[i]) {
			return 1;
		}
	}

	return 0;
}

const char* CFilesList::GetName(int delta) {
	if(delta != 0 && m_removeCurrent == true) {
		m_removeCurrent	= false;
		if(0 == unlink(m_files[m_position].c_str())) {
			printf("File '%s' has been removed from disk.\n", m_files[m_position].c_str());
		}
		// remove path from list
		m_files.erase(m_files.begin() + m_position);
		if(delta > 0) {
			delta	= 0;
		}
	}

	size_t count	= m_files.size();
	if(count > 0) {
		m_position	+= delta;
		if(m_position < 0) {
			m_position	= count - 1;
		}
		m_position	%= count;

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

void CFilesList::RemoveFromDisk() {
	m_removeCurrent	= true;
}
