/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "paths.hpp"

std::string Paths::customDataDir = Paths::cwd() + "../data/";  //to be modified later
std::string Paths::homeDataDir = Paths::home() + "motorsport/data/";  //to be modified later
std::string Paths::defaultDataDir = Paths::cwd() + "../data/";

Paths::Paths () {}
Paths::~Paths () {}

std::string Paths::cwd()
{
    std::string cwd;
    char buf[4096];
#ifdef WIN32
    if (_getcwd(buf, sizeof(buf) - 1))
#else
    if ( getcwd(buf, sizeof(buf) - 1))
#endif
    {
        cwd = buf;
    }
    return cwd + "/";
}
std::string Paths::home()
{
    char*  str;
    std::string result;
#ifdef WIN32
    // Get home dir from environment variables
    // like - C:\Documents and Settings\user\Application Data
    str = getenv("APPDATA");
    if ( str )
    {
        result = str;
    }
    else
    {
        // like - C:\Documents and Settings\user
        str = getenv("USERPROFILE");
        if ( str )
        {
            result = str;
        }
    }
#else
    // Try get home dir from environment variable
    str = getenv("HOME");
    if ( str )
    {
        result = str;
    }
    /*
    else {
        // Try to retrieve the home dir -- first use user's ID,
        //  and if failed, then use user's login name.
        if ( !s_GetHomeByUID(result) ) {  s_GetHomeByLOGIN(result); }
    }
    */
#endif 

    // Add trailing separator if needed
    if ( (result[result.length()-1] != '/') || (result[result.length()-1] != '\\') )
    {
        result.append("/");
    }
    return result;
}


std::string Paths::getDataDir (std::string relativeDirPath )
{
    std::string result;
    if (existsDir(result = customDataDir  + relativeDirPath)) return result;
    if (existsDir(result = homeDataDir    + relativeDirPath)) return result;
    if (existsDir(result = defaultDataDir + relativeDirPath)) return result;
    return "not found:" + relativeDirPath;
}
bool Paths::existsDir (std::string dirPath )
{
    //TODO
    return true;
}
void Paths::setCustomDataDir(std::string customDataDir)
{
    customDataDir = customDataDir;
}

std::string Paths::vehicle(std::string vehicle)
{
    return getDataDir("vehicles/" + vehicle + "/");
}
std::string Paths::area(std::string area)
{
    return getDataDir("areas/" + area + "/");
}
std::string Paths::world(std::string world)
{
    return getDataDir("worlds/");
}
std::string Paths::part(std::string part)
{
    return getDataDir("parts/" + part + "/");
}

std::string Paths::vehicleXml(std::string vehicle)
{
    return Paths::vehicle(vehicle) + "vehicle.xml";
}
std::string Paths::areaXml(std::string area)
{
    return Paths::area(area) + "area.xml";
}
std::string Paths::worldXml(std::string world)
{
    return Paths::world(world) + world + ".xml";
}
std::string Paths::partXml(std::string part)
{
    return Paths::part(part) + "part.xml";
}
std::string Paths::data()
{
    return Paths::getDataDir("");
}
std::string Paths::vehicles()
{
    return Paths::getDataDir("vehicles/");
}
std::string Paths::areas()
{
    return Paths::getDataDir("areas/");
}
std::string Paths::parts()
{
    return Paths::getDataDir("parts/");
}
std::string Paths::gui()
{
    return Paths::getDataDir("gui/");
}
