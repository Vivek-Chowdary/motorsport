
/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

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
    VehiclePosition (double posX, double posY, double posZ, double rotX, double rotY, double rotZ);
    ~VehiclePosition ();
    Vector3d getPosition();
    Vector3d getRotation();
};

#endif
