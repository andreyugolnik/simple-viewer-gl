/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#include "config.h"
#include "window.h"

#include <iostream>
#include <stdlib.h>
#include <sstream>

CConfig::CConfig(CWindow* window)
    : m_window(window)
    , m_opened(false)
{
	m_config.reset(new libconfig::Config());
}

CConfig::~CConfig()
{
}

bool CConfig::Open()
{
	m_opened = false;

	std::stringstream path;

#if defined(__APPLE__)
    path << getenv("HOME") <<"/Library/sviewgl/config";
#else
	// make config path according XDG spec
	if(getenv("XDG_CONFIG_HOME"))
	{
		path << getenv("XDG_CONFIG_HOME");
		if(path.str()[path.str().length() - 1] != '/')
		{
			path << "/";
		}
		path << "sviewgl/config";
	}
	else
	{
		path << getenv("HOME") <<"/.config/sviewgl/config";
	}
#endif

	std::cout << "Using config file: '" << path.str() << "'" << std::endl;

	// try to read config
	try
	{
		m_config->readFile(path.str().c_str());
	}
	catch(libconfig::FileIOException e)
	{
		std::cout << "Can't open config: " << std::endl;
		return false;
	}
	catch(libconfig::ParseException e)
	{
		std::cout << "Can't parse config: " << e.getError() << " in line " << e.getLine() << std::endl;
		return false;
	}

	m_opened	= true;

	return true;
}

void CConfig::Read()
{
	if(m_opened == true)
	{
		bool value;

		if(true == m_config->lookupValue("hide_infobar", value) && value == true) {
			m_window->SetProp(PROP_INFOBAR);
		}

		if(true == m_config->lookupValue("show_pixelinfo", value) && value == true) {
			m_window->SetProp(PROP_PIXELINFO);
		}

		if(true == m_config->lookupValue("hide_checkboard", value) && value == true) {
			m_window->SetProp(PROP_CHECKERS);
		}

		if(true == m_config->lookupValue("fit_image", value) && value == true) {
			m_window->SetProp(PROP_FITIMAGE);
		}

		if(true == m_config->lookupValue("start_fullscreen", value) && value == true) {
			m_window->SetProp(PROP_FULLSCREEN);
		}

		if(true == m_config->lookupValue("show_image_border", value) && value == true) {
			m_window->SetProp(PROP_BORDER);
		}

		if(true == m_config->lookupValue("lookup_recursive", value) && value == true) {
			m_window->SetProp(PROP_RECURSIVE);
		}

		int r, g, b;
		if(m_config->lookupValue("background_r", r)
		        && m_config->lookupValue("background_g", g)
		        && m_config->lookupValue("background_b", b))
		{
			m_window->SetProp(r, g, b);
		}
	}
}

