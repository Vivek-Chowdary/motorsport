/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
class Clutch;
class Gearbox;
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
    Clutch * clutch;
    Gearbox * gearbox;
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
