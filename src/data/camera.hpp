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

#ifndef CAMERA_HPP
#   define CAMERA_HPP
#   include "ode/objects.h"

namespace Ogre { 
  class Camera; 
};

class Vector3d;

class Camera
{
  private:
    static int instancesCount;
    dBodyID positionID;
    Vector3d * positionOffset;
    dBodyID targetID;
    Vector3d * targetOffset;

  public:
    Camera (float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ);
    ~Camera ();
    void updateOgrePosition ();
    void updateOgreTarget ();
    void setPositionID (dBodyID positionID);
    void setTargetID (dBodyID targetID);

    // physics
    void startPhysics (float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ);
    void stepPhysics();
    void stopPhysics();

    // graphics
    void startGraphics ();
    void stepGraphics ();
    void stopGraphics ();

    Ogre::Camera * ogreCamera;

    // input
    void startInput ();
    void stepInput ();
    void stopInput ();
};

#endif
