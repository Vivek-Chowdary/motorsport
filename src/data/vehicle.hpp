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

#ifndef VEHICLE_HPP
#   define VEHICLE_HPP
#   include <string>
#   include <map>
#   include <vector>
#   include "worldObject.hpp"
#   include "ode/objects.h"

//forward declarations
class Body;
class Engine;
class Wheel;
class Suspension;
class Vector3d;
class Camera;
class CameraPosition;

class Vehicle : public WorldObject
{
  private:
    static int instancesCount;
    std::string name;
    int revision;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;

  public:
    // data
    Vehicle (const std::string & xmlFilename);
    ~Vehicle ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlWheelListNode(XERCES_CPP_NAMESPACE::DOMNode * wheelListNode);
    void processXmlSuspensionListNode(XERCES_CPP_NAMESPACE::DOMNode * wheelListNode);
    void processXmlCameraListNode(XERCES_CPP_NAMESPACE::DOMNode * cameraListNode);
    void processXmlCameraPositionNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    Body * body;
    Engine * engine;
    std::map < std::string, Wheel *> wheelMap;
    std::map < std::string, Suspension * > suspensionMap;
    std::map < std::string, CameraPosition * > cameraPositionMap;
    std::vector < Camera * > cameraList;

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    void setPosition (Vector3d position);
    void setRotation (Vector3d rotation);
    Vector3d getPosition ();
    Vector3d getRotation ();
    void attachWheelsToBody();
    double getTorque ();
    dBodyID getVehicleID();

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();
};
#endif
