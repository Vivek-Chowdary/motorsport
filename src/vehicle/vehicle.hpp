/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef VEHICLE_HPP
#define VEHICLE_HPP
#include "worldObject.hpp"
#include "ode/objects.h"
#include "xmlTag.hpp"

//forward declarations
class Body;
class Engine;
class Clutch;
class Gear;
class Gearbox;
class GearboxGear;
class FinalDrive;
class DriveJoint;
class DriveMass;
class LSD;
class Wheel;
class Pedal;
class Suspension;
class Vector3d;
class Quaternion;
class CameraPosition;

SHARED_PTR_MAP(Camera, pCamera, Cameras, CameraIt);
SHARED_PTR_MAP(Vehicle, pVehicle, Vehicles, VehiclesIt);

class Vehicle : public WorldObject
{
  private:
    bool userDriver;
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
    void construct (XmlTag * tag);
    void stopPhysics ();
    void placeWheelsOnSuspensions();
    void boltWheelsToSuspensions();
    void stopGraphics ();
    WorldObjects components;
    Vehicle (WorldObject * container, std::string vehicleName);
  public:
    static pVehicle create(WorldObject * container, std::string vehicleName);
    ~Vehicle ();
    Cameras cameras;
    Body *          getBody         (std::string name);
    DriveJoint *    getDriveJoint   (std::string name);
    DriveMass *     getDriveMass    (std::string name);
    Clutch *        getClutch       (std::string name);
    Gear *          getGear         (std::string name);
    LSD *           getLSD          (std::string name);
    Engine *        getEngine       (std::string name);
    FinalDrive *    getFinalDrive   (std::string name);
    Gearbox *       getGearbox      (std::string name);
    GearboxGear *   getGearboxGear  (std::string name);
    Pedal *         getPedal        (std::string name);
    Suspension *    getSuspension   (std::string name);
    Wheel *         getWheel        (std::string name);
    WorldObject *   getObject       (std::string name);
    void setUserDriver ();
    void stepPhysics ();
    void setPosition (Vector3d position);
    void applyRotation (Quaternion rotation);
    Vector3d getPosition ();
    Quaternion getRotation ();
    double getOutputTorque ();
    pOdeObject getMainOdeObject();
    void stepGraphics ();
};
#endif
