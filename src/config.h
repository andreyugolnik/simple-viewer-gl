/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef CONFIG_H
#define CONFIG_H

#include <libconfig.h++>
#include <memory>

class CWindow;

class CConfig
{
public:
	CConfig(CWindow* window);
	virtual ~CConfig();

	bool Open();
	void Read();

private:
	CWindow* m_window;
	bool m_opened;
	std::auto_ptr<libconfig::Config> m_config;
};

#endif // CONFIG_H

