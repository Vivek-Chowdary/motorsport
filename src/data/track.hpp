/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef TRACK_HPP
#define TRACK_HPP
#include "ode/objects.h"
#include "worldObject.hpp"
#include <map>
#include <string>
#include <vector>
#include "paths.hpp"

//forward declatation
namespace Ogre {
  class Entity;
  class SceneNode;
};
class Part;
class Location;
class Camera;

class Track : public WorldObject
{
  private:
    std::string relativeTrackDir;
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
    void Track::processXmlPartListNode(XERCES_CPP_NAMESPACE::DOMNode * partListNode);
    
  public:
    // data
    Track (const std::string & xmlFilename);
    ~Track ();
    std::map <std::string, Location * > vehiclePositionMap;
    std::vector < Part * > partList;
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
    void setRenderDetail(int renderMode);
};
#endif
