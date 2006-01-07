/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef LOCATION_HPP
#define LOCATION_HPP
#include "vector3d.hpp"
#include <string>
#include "quaternion.hpp"
#include "xmlTag.hpp"

class Location
{
  private:
    Vector3d position;
    Quaternion rotation;
    std::string name;
  public:
    Location (XmlTag * tag);
    Location (std::string name, Vector3d position, Quaternion rotation);
    ~Location ();
    Vector3d getPosition();
    Quaternion getRotation();
    std::string getName();
};

#endif
