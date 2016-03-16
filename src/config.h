/////////////////////////////////////////////////
//
// Andrey A. Ugolnik
// andrey@ugolnik.info
//
/////////////////////////////////////////////////

#ifndef CONFIG_H_D5951FCC953627
#define CONFIG_H_D5951FCC953627

namespace libconfig
{
    class Config;
}

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
    std::unique_ptr<libconfig::Config> m_config;
};

#endif /* end of include guard: CONFIG_H_D5951FCC953627 */

