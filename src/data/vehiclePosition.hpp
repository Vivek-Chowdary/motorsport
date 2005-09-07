/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef VEHICLEPOSITION_HPP
#   define VEHICLEPOSITION_HPP
#   include "data/xercesc_fwd.hpp"
#   include "vector3d.hpp"
#   include <string>
#   include "quaternion.hpp"

class VehiclePosition
{
  private:
    Vector3d position;
    Quaternion rotation;
    std::string index;
  public:
    VehiclePosition (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~VehiclePosition ();
    Vector3d getPosition();
    Quaternion getRotation();
    std::string getIndex();
};

#endif
