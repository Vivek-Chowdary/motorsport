/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
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
#include "shared.hpp"

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
        void setName(const std::string & name);
        void setPath(const std::string & path);
        void setXmlPath(const std::string & xmlPath);

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

        pLocation getLocationObject(const std::string & fullname);
        pVehicle getVehicleObject(const std::string & fullname);
        pArea getAreaObject(const std::string & fullname);
        pWorldObject getFirstObject(const std::string & fullname);

        pLocation getLocation (const std::string & name);

        pArea getArea(const std::string & name);
        pVehicle getVehicle(const std::string & name);
        pCamera getCamera (const std::string & name);
        pBody getBody (const std::string & name);
        pDriveJoint getDriveJoint (const std::string & name);
        pDriveMass getDriveMass (const std::string & name);
        pClutch getClutch (const std::string & name);
        pGear getGear (const std::string & name);
        pLSD getLSD (const std::string & name);
        pEngine getEngine (const std::string & name);
        pFinalDrive getFinalDrive (const std::string & name);
        pGearbox getGearbox (const std::string & name);
        pGearboxGear getGearboxGear (const std::string & name);
        pPedal getPedal (const std::string & name);
        pSuspension getSuspension (const std::string & name);
        pWheel getWheel (const std::string & name);
        pWorldObject getObject (const std::string & name);
        pWorldObject getWorldObject (const std::string & name);
};
#endif
