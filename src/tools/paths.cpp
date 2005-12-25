/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
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


std::string Paths::getDataFile(std::string relativeFilePath)
{
    std::string result;
    if (existsFile(result = customDataDir  + relativeFilePath)) return result;
    if (existsFile(result = homeDataDir    + relativeFilePath)) return result;
    if (existsFile(result = defaultDataDir + relativeFilePath)) return result;
    return "not found:" + relativeFilePath;
}
std::string Paths::getDataDir (std::string relativeDirPath )
{
    std::string result;
    if (existsDir(result = customDataDir  + relativeDirPath)) return result;
    if (existsDir(result = homeDataDir    + relativeDirPath)) return result;
    if (existsDir(result = defaultDataDir + relativeDirPath)) return result;
    return "not found:" + relativeDirPath;
}
bool Paths::existsFile(std::string filePath)
{
    //TODO
    return true;
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
std::string Paths::track(std::string track)
{
    return getDataDir("tracks/" + track + "/");
}
std::string Paths::world(std::string world)
{
    //return getDataDir("worlds/" + world + "/");
    return getDataDir("worlds/");
}
std::string Paths::part(std::string part)
{
    return getDataDir("parts/" + part + "/");
}

std::string Paths::vehicleXml(std::string vehicle)
{
    return Paths::getDataFile(Paths::vehicle(vehicle) + "vehicle.xml");
}
std::string Paths::trackXml(std::string track)
{
    return Paths::getDataFile(Paths::track(track) + "track.xml");
}
std::string Paths::worldXml(std::string world)
{
    //return Paths::getDataFile(Paths::world(world) + "world.xml");
    return Paths::getDataFile(Paths::world(world) + world + ".xml");
}
std::string Paths::partXml(std::string part)
{
    return Paths::getDataFile(Paths::part(part) + "part.xml");
}
