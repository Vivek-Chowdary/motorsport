/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "vehiclePosition.hpp"

VehiclePosition::VehiclePosition (const Vector3d & position, const Vector3d & rotation)
{
    this->position = position;
    this->rotation = rotation;
}

VehiclePosition::~VehiclePosition ()
{
    //empty
}

Vector3d VehiclePosition::getPosition()
{
    return position;
}

Vector3d VehiclePosition::getRotation()
{
    return rotation;
}
