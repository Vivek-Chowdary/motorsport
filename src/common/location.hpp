/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef LOCATION_HPP
#   define LOCATION_HPP
#   include "tools/xercesc_fwd.hpp"
#   include "vector3d.hpp"
#   include <string>
#   include "quaternion.hpp"

class Location
{
  private:
    Vector3d position;
    Quaternion rotation;
    std::string name;
  public:
    Location (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Location ();
    Vector3d getPosition();
    Quaternion getRotation();
    std::string getName();
};

#endif
