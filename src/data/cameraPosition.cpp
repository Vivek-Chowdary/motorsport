/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "cameraPosition.hpp"

CameraPosition::CameraPosition (const Vector3d & position, const Vector3d & destination)
{
    this->position = position;
    this->destination = destination;
}

CameraPosition::~CameraPosition ()
{
    //empty
}

Vector3d CameraPosition::getPosition()
{
    return position;
}

Vector3d CameraPosition::getDestination()
{
    return destination;
}
