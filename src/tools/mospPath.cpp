/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include <iostream>
#include "mospPath.hpp"

std::string MospPath::getType(const std::string & fullname)
{
    std::string type;
    bool error = false;
    std::string errorMessage;
    if (fullname.find_first_of('(') == 0)
    {
        //(
        unsigned int typeend;
        if ((typeend = fullname.find_first_of(')',0)) != std::string::npos)
        {
            //(type)
            type = fullname.substr(1,typeend-1);
        } else {
            //(type
            error = true; errorMessage = "')' not found";
        }
    } else {
        //name
        error = true; errorMessage = "'(' not found";
    }
    if (error) std::cout << std::endl << "Error! Mosp path syntax error \"" << fullname << "\": " << errorMessage;
    //if (error) log->__format (LOG_ERROR, "Mosp object location syntax error \"%s\": %s", fullname.c_str(), errorMessage.c_str());
    return type;
}
std::string MospPath::getName(const std::string & fullname)
{
    std::string name;
    bool error = false;
    std::string errorMessage = "";
    if (fullname.find_first_of('(') == 0)
    {
        //(
        unsigned int typeend;
        if ((typeend = fullname.find_first_of(')',0)) != std::string::npos)
        {
            //(type)
            unsigned int nameend;
            if ((nameend = fullname.find_first_of('/',typeend)) != std::string::npos)
            {
                //(type)name/
                name = fullname.substr(typeend+1,nameend-(typeend+1));
            } else {
                //(type)name
                name = fullname.substr(typeend+1);
            }
        } else {
            //(type
            error = true; errorMessage = "')' not found";
        }
    } else {
        //name
        name = fullname;
    }
    if (error) std::cout << std::endl << "Error! Mosp path syntax error \"" << fullname << "\": " << errorMessage;
    return name;
}
std::string MospPath::getSubFullname(const std::string & fullname)
{
    std::string subfullname;
    bool error = false;
    std::string errorMessage = "";
    if (fullname.find_first_of('(') == 0)
    {
        //(
        unsigned int typeend;
        if ((typeend = fullname.find_first_of(')',0)) != std::string::npos)
        {
            //(type)
            unsigned int nameend;
            if ((nameend = fullname.find_first_of('/',typeend)) != std::string::npos)
            {
                //(type)name/
                subfullname = fullname.substr(nameend+1);
            } else {
                //(type)name
                error = true; errorMessage = "'/' not found";
            }
        } else {
            //(type
            error = true; errorMessage = "')' not found";
        }
    } else {
        //name
        subfullname = fullname;
    }
    if (error) std::cout << std::endl << "Error! Mosp path syntax error \"" << fullname << "\": " << errorMessage;
    return subfullname;
}
