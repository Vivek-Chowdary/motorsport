/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
class Gear;
class Gearbox;
class GearboxGear;
class FinalDrive;
class LSD;
class Wheel;
class Pedal;
class Suspension;
class Vector3d;
class Quaternion;
class Camera;
class CameraPosition;

class Vehicle : public WorldObject
{
  private:
    bool userDriver;
    std::string longName;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void processXmlWheelListNode(XERCES_CPP_NAMESPACE::DOMNode * wheelListNode);
    void processXmlSuspensionListNode(XERCES_CPP_NAMESPACE::DOMNode * wheelListNode);
    void processXmlPedalListNode(XERCES_CPP_NAMESPACE::DOMNode * pedalListNode);
    void processXmlCameraListNode(XERCES_CPP_NAMESPACE::DOMNode * cameraListNode);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    void placeWheelsOnSuspensions();
    void boltWheelsToSuspensions();
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();
  public:
    Vehicle (WorldObject * container, std::string name);
    ~Vehicle ();
    Body * body;
    Engine * engine;
    Clutch * clutch;
    Gear * transfer;
    Gearbox * gearbox;
    FinalDrive * finalDrive;
    LSD * rearDiff;
    std::map < std::string, Wheel * > wheelMap;
    std::map < std::string, Suspension * > suspensionMap;
    std::map < std::string, Pedal * > pedalMap;
    std::vector < Camera * > cameraList;
    void setUserDriver ();
    void stepPhysics ();
    void setPosition (Vector3d position);
    void applyRotation (Quaternion rotation);
    Vector3d getPosition ();
    Quaternion getRotation ();
    double getOutputTorque ();
    dBodyID getVehicleID();
    void setRenderDetail(int renderMode);
    void stepGraphics ();
};
#endif
