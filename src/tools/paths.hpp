/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
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

    static std::string getDataDir (const std::string & relativeDirPath );
    static bool existsDir (const std::string & dirPath );

  public:
    static void setCustomDataDir(const std::string & customDataDir);
    static std::string data();
    static std::string vehicles();
    static std::string areas();
    static std::string parts();
    static std::string gui();
    
    static std::string vehicle(const std::string & vehicle);
    static std::string area(const std::string & area);
    static std::string world(const std::string & world);
    static std::string part(const std::string & part);

    static std::string vehicleXml(const std::string & vehicle);
    static std::string areaXml(const std::string & area);
    static std::string worldXml(const std::string & world);
    static std::string partXml(const std::string & part);
};

#endif
