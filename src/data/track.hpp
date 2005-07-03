/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef TRACK_HPP
#   define TRACK_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include <map>
#   include <string>
#   include <vector>

//forward declatation
namespace Ogre {
  class Entity;
  class SceneNode;
};
class Cube;
class VehiclePosition;
class Camera;

class Track : public WorldObject
{
  private:
    // data
    std::string path;
    std::string name;
    int revision;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlCameraNode (XERCES_CPP_NAMESPACE::DOMNode * n);

  public:
    // data
    Track (const std::string & xmlFilename);
    ~Track ();
    std::map <std::string, VehiclePosition * > vehiclePositionMap;
    std::vector < Cube * > cubeList;
    std::vector < Camera * > cameraList;

    // physics
/*    void startPhysics (DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();*/
    dGeomID bodyGeomID;
    dBodyID trackBodyID;

    // graphics
/*    void startGraphics (DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();*/
    Ogre::SceneNode * planeNode;
    Ogre::Entity * planeEntity;
    Ogre::Entity * trackEntity;
    Ogre::SceneNode * trackNode;
    void setCastShadows(bool castShadows);
};
#endif
