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
#   include "worldObject.hpp"
#   include <string>

namespace Ogre { 
  class Camera; 
};

class Vector3d;

class Camera : public WorldObject
{
  private:
    // data
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    static int instancesCount;
    std::string index;

    // physics 
    dBodyID positionID;
    Vector3d * positionOffset;
    dBodyID targetID;
    Vector3d * targetOffset;
    void stopPhysics();

    // graphics
    void stopGraphics ();

    // input
    void stopInput ();
  public:
    Camera (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Camera ();
    void updateOgreRotation ();
    void updateOgrePosition ();
    void updateOgreTarget ();

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics();
    void setPositionID (dBodyID positionID);
    void setTargetID (dBodyID targetID);

    // graphics
    void startGraphics ();
    void stepGraphics ();
    Ogre::Camera * ogreCamera;

    // input
    void startInput ();
    void stepInput ();
};

#endif
