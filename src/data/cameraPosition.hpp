/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef CAMERAPOSITION_HPP
#   define CAMERAPOSITION_HPP
#   include "vector3d.hpp"

class CameraPosition
{
  private:
    Vector3d position;
    Vector3d destination;
  public:
    CameraPosition (const Vector3d & position, const Vector3d & destination);
    ~CameraPosition ();
    Vector3d getPosition();
    Vector3d getDestination();
};

#endif
