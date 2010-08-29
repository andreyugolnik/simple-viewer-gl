/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "ftstring.h"
#include "arial.hpp"
#include <iostream>

CFTString::CFTString(int size) : m_height(size),
	m_ft(0), m_symbols(0), m_unicode(0), m_unicodeSize(0),
	m_tex(0), m_texW(256), m_texH(256) {

	if(FT_Init_FreeType(&m_ft) != 0) {
		std::cout << "(EE) Error initiation FreeType2" << std::endl;
	}
}

CFTString::CFTString(const char* ttf, int size) : m_ttf(ttf), m_height(size),
	m_ft(0), m_symbols(0), m_unicode(0), m_unicodeSize(0),
	m_tex(0), m_texW(256), m_texH(256) {

	if(FT_Init_FreeType(&m_ft) != 0) {
		std::cout << "(EE) Error initiation FreeType2" << std::endl;
	}
}

CFTString::~CFTString() {
	clearSymbols();
	delete[] m_unicode;
	delete[] m_symbols;
	if(m_tex != 0) {
		glDeleteTextures(1, &m_tex);
	}
	if(m_ft != 0) {
		FT_Done_FreeType(m_ft);
	}
}

void CFTString::Update(const char* utf8) {
    int size	= mbstowcs(NULL, utf8, 0) + 1;
    if(size > m_unicodeSize) {
		delete[] m_unicode;
		m_unicodeSize	= size;
		m_unicode		= new wchar_t[m_unicodeSize];
    }
    mbstowcs(m_unicode, utf8, m_unicodeSize);
}

void CFTString::Render(int x, int y) {
	wchar_t* string	= m_unicode;
	if(string != 0) {
		int xStart	= x;

		glBindTexture(GL_TEXTURE_2D, m_tex);

		SymbolsIc it, itEnd = m_mapSymbol.end();
		while(*string) {
			const wchar_t i	= *string;
			if(i == L'\n') {
				x	= xStart;
				y	+= m_height;
			}
			else {
				it	= m_mapSymbol.find(i);
				if(it == itEnd) {
					generateNewSymbol(string);
					itEnd	= m_mapSymbol.end();
					continue;
				}
				if(it->second.p) {
					it->second.p->Render(x + it->second.l, y - it->second.t);
					x  += it->second.ax;
				}
			}

			string++;
		}
	}
}

int CFTString::GetStringWidth() {
	int width		= 0;
	int widthMax	= 0;
	wchar_t* string	= m_unicode;
	if(string != 0) {
		SymbolsIc it, itEnd = m_mapSymbol.end();
		while(*string) {
			const wchar_t i	= *string;
			if(i == L'\n') {
				width	= 0;
			}
			else {
				it	= m_mapSymbol.find(i);
				if(it == itEnd) {
					generateNewSymbol(string);
					itEnd	= m_mapSymbol.end();
					continue;
				}
				if(it->second.p) {
					width  += it->second.ax;
					widthMax	= std::max(widthMax, width);
				}
			}

			string++;
		}
	}

    return widthMax;
}

void CFTString::generateNewSymbol(const wchar_t* string) {
    if(m_ft != 0) {
		size_t len		= (m_symbols ? wcslen(m_symbols) : 0) + wcslen(string);
		wchar_t* tmp	= new wchar_t[len + 1];
		wchar_t* p		= tmp;
		p[0]	= 0;
		if(m_symbols != 0) {
			wcscpy(p, m_symbols);
		}
		wchar_t* s	= (wchar_t*)string;
		while(*s) {
			len	= wcslen(p);
			bool dup	= false;
			for(size_t i = 0; i < len; i++) {
				if(p[i] == *s) {
					dup	= true;
					break;
				}
			}

			if(dup == false) {
				p[len]		= *s;
				p[len + 1]	= 0;
			}

			s++;
		}

		delete[] m_symbols;

		m_symbols	= tmp;

		generate();
    }
}

void CFTString::generate() {
	FT_Face face;

	FT_Error err	= 0;
	if(m_ttf.empty() == true) {
		err	= FT_New_Memory_Face(m_ft, arial_ttf, arial_ttf_size, 0, &face);
	}
	else {
		err	= FT_New_Face(m_ft, m_ttf.c_str(), 0, &face);
	}
	if(err != 0) {
		std::cout << "(EE) Error creating face" << std::endl;
		return;
	}

	if(FT_Set_Pixel_Sizes(face, 0, m_height) != 0) {
		std::cout << "(EE) Error set font size" << std::endl;
		FT_Done_Face(face);
		return;
	}

	clearSymbols();

	FT_GlyphSlot slot	= face->glyph;
	size_t len	= wcslen(m_symbols);
	for(size_t i = 0; i < len; i++) {
		FT_ULong charcode	= m_symbols[i];
		err		= FT_Load_Char(face, charcode, FT_LOAD_RENDER);
		Symbol str;
		memset(&str, 0, sizeof(Symbol));
		if(err == 0) {
			FT_Bitmap bmp	= slot->bitmap;
			const int size	= bmp.pitch * bmp.rows;

//          str.p	= 0;
			str.bmp	= new unsigned char[size];
			str.w	= bmp.width;
			str.h	= bmp.rows;
			str.pitch	= bmp.pitch;
			str.l	= slot->bitmap_left;
			str.t	= slot->bitmap_top;
			str.ax	= slot->advance.x >> 6;
//          str.px	= 0;
//          str.py	= 0;
			memcpy(str.bmp, bmp.buffer, size);
		}
		m_mapSymbol[charcode]   = str;
	}

	glBindTexture(GL_TEXTURE_2D, m_tex);

	// calculate symbols placement
	if(placeSymbols() == false) {
		std::cout << "(EE) texture to big" << std::endl;
		FT_Done_Face(face);
		return;
	}

#define PX(a)	(((unsigned int)(a)<<24) + (unsigned int)0x00ffffff)

	unsigned int* buffer	= new unsigned int[m_texW * m_texH];
	memset(buffer, 0, m_texW * m_texH * 4);

	// regenerate texture
	SymbolsIc it	= m_mapSymbol.begin();
	for( ; it != m_mapSymbol.end(); ++it) {
		int dx	= it->second.px;
		int dy	= it->second.py;
		int w	= it->second.w;
		int h	= it->second.h;
		int pitch	= it->second.pitch;
		unsigned char *pIn  = it->second.bmp;
		if(pIn) {
			for(int y = 0; y < h; y++) {
				size_t pos  = dx + m_texW + (dy + y) * m_texW;
				for(int x = 0; x < w; x++) {
					unsigned char pixel = pIn[x + y * pitch];
					buffer[pos]   = PX(pixel);
					pos++;
				}
			}
		}
	}

	if(m_tex == 0) {
		glGenTextures(1, &m_tex);
		glBindTexture(GL_TEXTURE_2D, m_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, 4, m_texW, m_texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

	delete[] buffer;

	SymbolsIt it2 = m_mapSymbol.begin();
	for( ; it2 != m_mapSymbol.end(); ++it2) {
		if(it2->second.bmp != 0) {
			it2->second.p	= new CFTSymbol(m_texW, m_texH, it2->second.px, it2->second.py, it2->second.w, it2->second.h + 1);
			delete[] it2->second.bmp;
			it2->second.bmp	= 0;
		}
	}

	FT_Done_Face(face);

//	std::cout << "Texture (" << m_texW << "x" << m_texH << ") with " << m_mapSymbol.size() << " / " << len << " symbols has been created." << std::endl;
}

bool CFTString::placeSymbols() {
    for( ; ; ) {
    	bool done	= true;
		int max_h	= 0;

		SymbolsIt it = m_mapSymbol.begin(), itEnd = m_mapSymbol.end();
		for(int x = 1, y = 1; it != itEnd; ++it) {
			if(max_h < it->second.h) {
				max_h	= it->second.h;
			}
			if(y + it->second.h + 1 >= m_texH) {
				done	= false;
				break;
			}
			if(x + it->second.w + 1 >= m_texW) {
				x	= 1;
				y	+= max_h + 1;
				if(y + max_h + 1 >= m_texH) {
					done	= false;
					break;
				}
			}

			it->second.px	= x;
			it->second.py	= y;
			x	+= it->second.w + 1;
		}

		if(done == false) {
			if(m_texW <= m_texH) {
				m_texW	<<= 1;
			}
			else {
				m_texH	<<= 1;
			}

			if(m_texW > 1024 || m_texH > 1024) {
				return false;
			}
		}
		else {
			return true;
		}
    }
}

void CFTString::clearSymbols() {
    SymbolsIc it = m_mapSymbol.begin(), itEnd = m_mapSymbol.end();
    for( ; it != itEnd; ++it) {
        delete it->second.p;
    }
}
