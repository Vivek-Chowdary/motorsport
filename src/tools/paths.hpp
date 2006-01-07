/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PATHS_HPP
#define PATHS_HPP
#include <string>
#ifdef WIN32
#include <direct.h>
#endif

class Paths
{
  private:
    Paths ();
    ~Paths ();
    static std::string cwd();
    static std::string home();

    static std::string customDataDir;
    static std::string homeDataDir;
    static std::string defaultDataDir;

    static std::string getDataDir (std::string relativeDirPath );
    static bool existsDir (std::string dirPath );

  public:
    static void setCustomDataDir(std::string customDataDir);
    static std::string data();
    static std::string vehicles();
    static std::string areas();
    static std::string parts();
    static std::string gui();
    
    static std::string vehicle(std::string vehicle);
    static std::string area(std::string area);
    static std::string world(std::string world);
    static std::string part(std::string part);

    static std::string vehicleXml(std::string vehicle);
    static std::string areaXml(std::string area);
    static std::string worldXml(std::string world);
    static std::string partXml(std::string part);
};

#endif
