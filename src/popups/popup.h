/**********************************************\
*
*  Simple Viewer GL edition
*  by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class cPopup
{
public:
    virtual ~cPopup() = default;

    virtual void render() = 0;
};
