/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef VEHICLE_HPP
#define VEHICLE_HPP
#include "worldObject.hpp"
#include "ode/objects.h"
#include "xmlTag.hpp"

// Forward declarations
class Vector3d;
class Quaternion;
class CameraPosition;

SHARED_PTR_MAP(Vehicle, pVehicle, Vehicles, VehiclesIt, wVehicle);
SHARED_PTR_MAP(Camera, pCamera, Cameras, CameraIt, wCamera);

class Vehicle : public WorldObject
{
  private:
    std::string description;
    std::string author;
    std::string contact;
    std::string license;
    void construct (XmlTag * tag);
    void stopPhysics ();
    void placeWheelsOnSuspensions();
    void boltWheelsToSuspensions();
    void stopGraphics ();
    Vehicle (std::string vehicleName);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pVehicle create(std::string vehicleName);
    void setContainer (pWorldObject container);
    ~Vehicle ();

    void setUserDriver ();
    void stepPhysics ();
    void setPosition (Vector3d position);
    void applyRotation (Quaternion rotation);
    Vector3d getPosition ();
    Quaternion getRotation ();
    double getOutputTorque ();
    pOdeObject getMainOdeObject();
};
#endif
