/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#define WORLDOBJECT_HPP
#include "system.hpp"
#include "logEngine.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"
#include "xmlTag.hpp"
#include <shared.hpp>

//forward declatations
class Quaternion;
class Vector3d;
SHARED_PTR_MAP(OgreObject, pOgreObject, OgreObjects, OgreObjectsIt, wOgreObject);
SHARED_PTR_MAP(OdeObject, pOdeObject, OdeObjects, OdeObjectsIt, wOdeObject);
SHARED_PTR_MAP(WorldObject, pWorldObject, WorldObjects, WorldObjectsIt, wWorldObject);
typedef std::map <std::string, WorldObject *> WorldObjectsC;
typedef std::map <std::string, WorldObject *>::iterator WorldObjectsCIt;

SHARED_PTR_MAP(Camera, pCamera, Cameras, CamerasIt, wCamera);
SHARED_PTR_MAP(Vehicle, pVehicle, Vehicles, VehiclesIt, wVehicle);
SHARED_PTR_MAP(Area, pArea, Areas, AreasIt, wArea);
SHARED_PTR_MAP(Location, pLocation, Locations, LocationsIt, wLocation);
SHARED_PTR(Body, pBody, wBody);
SHARED_PTR(Engine, pEngine, wEngine);
SHARED_PTR(Clutch, pClutch, wClutch);
SHARED_PTR(Gear, pGear, wGear);
SHARED_PTR(Gearbox, pGearbox, wGearbox);
SHARED_PTR(GearboxGear, pGearboxGear, wGearboxGear);
SHARED_PTR(FinalDrive, pFinalDrive, wFinalDrive);
SHARED_PTR(DriveJoint, pDriveJoint, wDriveJoint);
SHARED_PTR(DriveMass, pDriveMass, wDriveMass);
SHARED_PTR(LSD, pLSD, wLSD);
SHARED_PTR(Wheel, pWheel, wWheel);
SHARED_PTR(Pedal, pPedal, wPedal);
SHARED_PTR(Suspension, pSuspension, wSuspension);

class WorldObject: public boost::enable_shared_from_this <WorldObject>
{
    private:
        virtual void hack(){};
        static unsigned int instancesCount;
        static WorldObjectsC worldObjects;
        std::string id;
        std::string path;
        std::string xmlPath;
    protected:
        std::string name;
        pCamera activeCamera;
        pLogEngine log;
        bool userDriver;
        WorldObject * base;
        wWorldObject container;
        OgreObjects ogreObjects;
        OdeObjects odeObjects;
        WorldObjects objects;
        Locations locations;
        WorldObject (const std::string & name);
        void constructFromTag(XmlTag * tag);
        virtual void readCustomDataTag(XmlTag * tag) = 0;
    public:
        virtual ~WorldObject ();
        void setContainer(pWorldObject container);
        pWorldObject getContainer();
        std::string getId();
        std::string getName();
        std::string getFullName();
        std::string getPath();
        std::string getXmlPath();
        void setName(std::string name);
        void setPath(std::string path);
        void setXmlPath(std::string xmlPath);

        static void logAll();
        pLogEngine getLog();

        virtual void stepGraphics ();
        virtual void stepPhysics ();

        pCamera switchNextCamera();
        void pointCameras(pVehicle target);
        void positionCameras(dBodyID baseID);
        void positionCameras(pVehicle base);
        pOdeObject getMainOdeObject();
        
        void setRotation (Quaternion newRotation);
        virtual void setPosition (Vector3d newPosition);
        void addPosition (Vector3d diff);
        void addRotation (Vector3d origin, Quaternion diff);
        virtual Vector3d getPosition ();
        virtual Quaternion getRotation ();

        pLocation getLocationObject(std::string fullname);
        pVehicle getVehicleObject(std::string fullname);
        pArea getAreaObject(std::string fullname);
        pWorldObject getFirstObject(std::string fullname);

        pLocation getLocation (std::string name);

        pArea getArea(std::string name);
        pVehicle getVehicle(std::string name);
        pCamera getCamera (std::string name);
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
        pWorldObject getWorldObject (std::string name);
};
#endif
