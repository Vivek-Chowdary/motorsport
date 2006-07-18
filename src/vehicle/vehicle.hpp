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

// Forward declarations
SHARED_PTR(Body, pBody);
SHARED_PTR(Engine, pEngine);
SHARED_PTR(Clutch, pClutch);
SHARED_PTR(Gear, pGear);
SHARED_PTR(Gearbox, pGearbox);
SHARED_PTR(GearboxGear, pGearboxGear);
SHARED_PTR(FinalDrive, pFinalDrive);
SHARED_PTR(DriveJoint, pDriveJoint);
SHARED_PTR(DriveMass, pDriveMass);
SHARED_PTR(LSD, pLSD);
SHARED_PTR(Wheel, pWheel);
SHARED_PTR(Pedal, pPedal);
SHARED_PTR(Suspension, pSuspension);
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

    pBody getBody (std::string name);
    pDriveJoint getDriveJoint (std::string name);
    pDriveMass getDriveMass (std::string name);
    pClutch getClutch (std::string name);
    pGear getGear (std::string name);
    pLSD getLSD (std::string name);
    pEngine getEngine (std::string name);
    pFinalDrive getFinalDrive (std::string name);
    pGearbox getGearbox (std::string name);
    pGearboxGear getGearboxGear (std::string name);
    pPedal getPedal (std::string name);
    pSuspension getSuspension (std::string name);
    pWheel getWheel (std::string name);
    pWorldObject getObject (std::string name);

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
