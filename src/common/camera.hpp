/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef CAMERA_HPP
#   define CAMERA_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include <string>

namespace Ogre { 
  class Camera; 
};

class Vector3d;

class Camera : public WorldObject
{
  private:
    bool isFree;
    dBodyID positionID;
    Vector3d * positionOffset;
    dBodyID targetID;
    Vector3d * targetOffset;
    void stopPhysics();
  public:
    Camera (WorldObject * container, XmlTag * tag);
    ~Camera ();
    void updateOgreRotation ();
    Vector3d updateOgrePosition ();
    Vector3d updateOgreTarget ();
    void stepPhysics();
    void setPositionID (dBodyID positionID);
    void setTarget (pOdeObject odeObject);
    void startGraphics ();
    void stepGraphics ();
    Ogre::Camera * ogreCamera;
    void startInput ();
};

#endif
