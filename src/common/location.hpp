/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "vector3d.hpp"
#include <string>
#include "quaternion.hpp"
#include "xmlTag.hpp"
#include "common/shared.hpp"

SHARED_PTR_MAP(Location, pLocation, Locations, LocationsIt, wLocation);

class Location
{
  private:
    Vector3d position;
    Quaternion rotation;
    std::string name;
    Location (XmlTag * tag);
    Location (const std::string & name, Vector3d position, Quaternion rotation);
  public:
    static pLocation create(XmlTag * tag);
    static pLocation create(const std::string & name, Vector3d position, Quaternion rotation);
    ~Location ();
    Vector3d getPosition();
    Quaternion getRotation();
    std::string getName();

    void addPosition (Vector3d diff);
    void addRotation (Vector3d origin, Quaternion diff);
    void setRotation (Quaternion newRotation);
    void setPosition (Vector3d newPosition);
};

#endif
