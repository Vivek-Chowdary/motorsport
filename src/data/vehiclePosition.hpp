/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef VEHICLEPOSITION_HPP
#   define VEHICLEPOSITION_HPP
#   include "vector3d.hpp"

class VehiclePosition
{
  private:
    Vector3d position;
    Vector3d rotation;
  public:
    VehiclePosition (const Vector3d & position, const Vector3d & rotation);
    ~VehiclePosition ();
    Vector3d getPosition();
    Vector3d getRotation();
};

#endif
