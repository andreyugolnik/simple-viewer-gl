/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <memory>

namespace libconfig
{
    class Config;
}

class cViewer;

class CConfig final
{
public:
    CConfig(cViewer* viewer);
    ~CConfig();

    bool Open();
    void Read();

private:
    cViewer* m_viewer;
    bool m_opened;
    std::unique_ptr<libconfig::Config> m_config;
};

