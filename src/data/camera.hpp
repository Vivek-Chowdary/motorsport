/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
    Camera (Vector3d position, Vector3d lookAt);
    ~Camera ();
    void updateOgrePosition ();
    void updateOgreTarget ();
    void setPositionID (dBodyID positionID);
    void setTargetID (dBodyID targetID);

    // physics
    void startPhysics (Vector3d position, Vector3d lookAt);
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
