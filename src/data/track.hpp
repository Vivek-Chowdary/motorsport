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
class CameraPosition;
class Cube;
class VehiclePosition;
class Camera;

class Track : public WorldObject
{
  private:
    std::string name;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
  public:
    // data
    Track (const std::string & xmlFilename);
    ~Track ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlVehiclePositionNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlCameraPositionNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    std::map <std::string, CameraPosition * > cameraPositionMap;
    std::map <std::string, VehiclePosition * > vehiclePositionMap;

    std::vector < Cube * > cubeList;
    std::vector < Camera * > cameraList;

    // physics
/*    void startPhysics (DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();*/
    dGeomID bodyGeomID;

    // graphics
/*    void startGraphics (DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();*/
    Ogre::Entity * trackEntity;
    Ogre::SceneNode * trackNode;
};
#endif
