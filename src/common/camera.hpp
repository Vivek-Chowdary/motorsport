/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    bool isFree;

    dBodyID positionID;
    Vector3d * positionOffset;
    dBodyID targetID;
    Vector3d * targetOffset;
    void stopPhysics();
    void stopGraphics ();
    void stopInput ();
  public:
    Camera (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Camera ();
    void updateOgreRotation ();
    Vector3d updateOgrePosition ();
    Vector3d updateOgreTarget ();
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics();
    void setPositionID (dBodyID positionID);
    void setTargetID (dBodyID targetID);
    void startGraphics ();
    void stepGraphics ();
    Ogre::Camera * ogreCamera;
    void startInput ();
    void stepInput ();
};

#endif
