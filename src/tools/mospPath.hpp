/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef MOSPPATH_HPP
#define MOSPPATH_HPP
#include <string>

namespace MospPath
{
    std::string getType(std::string fullname);
    std::string getName(std::string fullname);
    std::string getSubFullname(std::string fullname);
}

#endif
