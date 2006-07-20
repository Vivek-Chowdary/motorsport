/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "worldObject.hpp"
#include "quaternion.hpp"
#include "vector3d.hpp"

#include "area.hpp"
#include "part.hpp"
#include "world.hpp"
#include "driveMass.hpp"
#include "body.hpp"
#include "finalDrive.hpp"
#include "gearbox.hpp"
#include "pedal.hpp"
#include "suspension.hpp"
#include "vehicle.hpp"
#include "wheel.hpp"
#include "driveJoint.hpp"
#include "engine.hpp"
#include "camera.hpp"

unsigned int WorldObject::instancesCount = 0;
WorldObjectsC WorldObject::worldObjects;

WorldObject::WorldObject (const std::string & name)
{
    this->base = dynamic_cast<WorldObject *>(this);
    this->name = name;
    this->path = "/unknownPath/";
    this->xmlPath = path + "unknownFilepath";

    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;

    //FIXME what to use, id# or name?
    //log = LogEngine::create (LOG_DEVELOPER, this->getId());
    log = LogEngine::create (LOG_DEVELOPER, this->getFullName());
    log->__format(LOG_CCREATOR, "New world object! Id #%s.", id.c_str());
    worldObjects[this->id] = this;
}
void WorldObject::setContainer(pWorldObject container)
{
    std::string oldName = getFullName();
    this->container = container;
    //if container == NULL then this is the root container, and its paths have already been set.
    if (container != NULL)
    {
        this->path = container->getPath();
        this->xmlPath = container->getPath();
    }
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        dBodySetData (j->second->getBodyID(), (void*) container.get());
    }
    pWorldObject t = shared_from_this();
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->second) i->second->setContainer(t);
    }
    log->setName(this->getFullName());
    log->__format(LOG_CCREATOR, "New container! Old full name: %s. New full name: %s", oldName.c_str(), getFullName().c_str());

}

WorldObject::~WorldObject ()
{
    log->__format(LOG_CCREATOR, "Deleting myself. Id #%s.", id.c_str());
    container.reset();
    worldObjects.erase(this->id);
    base = NULL;
}
void WorldObject::logAll()
{
    pLogEngine log (LogEngine::create(LOG_DEVELOPER, "WorldObjects"));
    WorldObjectsCIt i = worldObjects.begin();
    for(;i != worldObjects.end(); i++)
    {
        if (i->second != NULL)
        log->__format(LOG_DEVELOPER, "WorldObject id#%s.\t Full name: %s", i->second->getId().c_str(), i->second->getFullName().c_str());
    }
}
std::string WorldObject::getId()
{
    return id;
}

std::string WorldObject::getName()
{
    std::string type;
    if (0) type.clear();
    else if (dynamic_cast<DoubleWishbone  *>(this)) type = "doublewishbone";
    else if (dynamic_cast<Fixed           *>(this)) type = "fixed";
    else if (dynamic_cast<Unidimensional  *>(this)) type = "unidimensional";
    else if (dynamic_cast<Suspension     *> (this)) type = "suspension";
    else if (dynamic_cast<Engine          *>(this)) type = "engine";
    else if (dynamic_cast<Gearbox         *>(this)) type = "gearbox";
    else if (dynamic_cast<Wheel           *>(this)) type = "wheel";
    else if (dynamic_cast<FinalDrive      *>(this)) type = "finaldrive";
    else if (dynamic_cast<DriveMass      *> (this)) type = "drivemass";
    else if (dynamic_cast<Gear            *>(this)) type = "gear";
    else if (dynamic_cast<LSD             *>(this)) type = "lsd";
    else if (dynamic_cast<Clutch          *>(this)) type = "clutch";
    else if (dynamic_cast<DriveJoint     *> (this)) type = "drivejoint";
    else if (dynamic_cast<GearboxGear    *> (this)) type = "gearboxgear";
    else if (dynamic_cast<Body           *> (this)) type = "body";
    else if (dynamic_cast<Pedal          *> (this)) type = "pedal";
    else if (dynamic_cast<Part           *> (this)) type = "part";
    else if (dynamic_cast<Vehicle        *> (this)) type = "vehicle";
    else if (dynamic_cast<Area           *> (this)) type = "area";
    else if (dynamic_cast<World          *> (this)) type = "world";
    else if (dynamic_cast<Camera         *> (this)) type = "camera";
    else if (dynamic_cast<WorldObject   *>  (this)) type = "worldobject";
    return "(" + type + ")" + name;
}
void WorldObject::setName(std::string name)
{
    this->name = name;
    log->setName(getFullName());
}
std::string WorldObject::getFullName()
{
    pWorldObject pcontainer = container.lock();
    if (pcontainer == NULL) return getName();
    return pcontainer->getFullName() + "." + getName();
}
std::string WorldObject::getPath()
{
    if (path == "/unknownPath/") return System::get()->getCurrentPath();
    return path;
}
void WorldObject::setPath(std::string path)
{
    this->path = path;
}
std::string WorldObject::getXmlPath()
{
    return xmlPath;
}
void WorldObject::setXmlPath(std::string xmlPath)
{
    this->xmlPath = xmlPath;
}
pLogEngine WorldObject::getLog()
{
    return log;
}
pWorldObject WorldObject::getContainer()
{
    pWorldObject pcontainer = container.lock();
    return pcontainer;
}
pOdeObject WorldObject::getMainOdeObject()
{
    //TODO: check what is returned when odeObjects is empty. prolly a reset shared pointer, but i'm not sure
    //if (odeObjects.empty()) return NULL;
    return odeObjects.begin()->second;
}
void WorldObject::stepPhysics ()
{
    WorldObjectsIt i = objects.begin();
    //step torque transfer objects first
    for (i = objects.begin(); i != objects.end(); i++)
    {
        if (0);
        else if (pGear           tmp = boost::dynamic_pointer_cast<Gear          >(i->second)) tmp->stepPhysics();
        else if (pLSD            tmp = boost::dynamic_pointer_cast<LSD           >(i->second)) tmp->stepPhysics();
        else if (pClutch         tmp = boost::dynamic_pointer_cast<Clutch        >(i->second)) tmp->stepPhysics();
        else if (pDriveJoint     tmp = boost::dynamic_pointer_cast<DriveJoint    >(i->second)) tmp->stepPhysics();
    }
    //step the rest of objects
    for (i = objects.begin(); i != objects.end(); i++)
    {
        if (0);
        else if (pDoubleWishbone tmp = boost::dynamic_pointer_cast<DoubleWishbone>(i->second)) tmp->stepPhysics();
        else if (pFixed          tmp = boost::dynamic_pointer_cast<Fixed         >(i->second)) tmp->stepPhysics();
        else if (pUnidimensional tmp = boost::dynamic_pointer_cast<Unidimensional>(i->second)) tmp->stepPhysics();
        else if (pSuspension     tmp = boost::dynamic_pointer_cast<Suspension    >(i->second)) tmp->stepPhysics();
        else if (pEngine         tmp = boost::dynamic_pointer_cast<Engine        >(i->second)) tmp->stepPhysics();
        else if (pGearbox        tmp = boost::dynamic_pointer_cast<Gearbox       >(i->second)) tmp->stepPhysics();
        else if (pWheel          tmp = boost::dynamic_pointer_cast<Wheel         >(i->second)) tmp->stepPhysics();
        else if (pFinalDrive     tmp = boost::dynamic_pointer_cast<FinalDrive    >(i->second)) tmp->stepPhysics();
        else if (pDriveMass      tmp = boost::dynamic_pointer_cast<DriveMass     >(i->second)) tmp->stepPhysics();
        else if (pGearboxGear    tmp = boost::dynamic_pointer_cast<GearboxGear   >(i->second)) tmp->stepPhysics();
        else if (pBody           tmp = boost::dynamic_pointer_cast<Body          >(i->second)) tmp->stepPhysics();
        else if (pPedal          tmp = boost::dynamic_pointer_cast<Pedal         >(i->second)) tmp->stepPhysics();
        else if (pPart           tmp = boost::dynamic_pointer_cast<Part          >(i->second)) tmp->stepPhysics();
        else if (pVehicle        tmp = boost::dynamic_pointer_cast<Vehicle       >(i->second)) tmp->stepPhysics();
        else if (pArea           tmp = boost::dynamic_pointer_cast<Area          >(i->second)) tmp->stepPhysics();
        else if (pWorld          tmp = boost::dynamic_pointer_cast<World         >(i->second)) tmp->stepPhysics();
        else if (pCamera         tmp = boost::dynamic_pointer_cast<Camera        >(i->second)) tmp->stepPhysics();
        else if (pWorldObject    tmp = boost::dynamic_pointer_cast<WorldObject   >(i->second)) tmp->stepPhysics();
    }
}
void WorldObject::stepGraphics ()
{
    OgreObjectsIt o = ogreObjects.begin();
    for(;o != ogreObjects.end(); o++)
    {
        o->second->stepGraphics();
    }
    WorldObjectsIt i = objects.begin();
    for (; i != objects.end(); i++)
    {
        if (0);
        else if (pDoubleWishbone tmp = boost::dynamic_pointer_cast<DoubleWishbone>(i->second)) tmp->stepGraphics();
        else if (pFixed          tmp = boost::dynamic_pointer_cast<Fixed         >(i->second)) tmp->stepGraphics();
        else if (pUnidimensional tmp = boost::dynamic_pointer_cast<Unidimensional>(i->second)) tmp->stepGraphics();
        else if (pSuspension     tmp = boost::dynamic_pointer_cast<Suspension    >(i->second)) tmp->stepGraphics();
        else if (pEngine         tmp = boost::dynamic_pointer_cast<Engine        >(i->second)) tmp->stepGraphics();
        else if (pGearbox        tmp = boost::dynamic_pointer_cast<Gearbox       >(i->second)) tmp->stepGraphics();
        else if (pWheel          tmp = boost::dynamic_pointer_cast<Wheel         >(i->second)) tmp->stepGraphics();
        else if (pFinalDrive     tmp = boost::dynamic_pointer_cast<FinalDrive    >(i->second)) tmp->stepGraphics();
        else if (pDriveMass      tmp = boost::dynamic_pointer_cast<DriveMass     >(i->second)) tmp->stepGraphics();
        else if (pGear           tmp = boost::dynamic_pointer_cast<Gear          >(i->second)) tmp->stepGraphics();
        else if (pLSD            tmp = boost::dynamic_pointer_cast<LSD           >(i->second)) tmp->stepGraphics();
        else if (pClutch         tmp = boost::dynamic_pointer_cast<Clutch        >(i->second)) tmp->stepGraphics();
        else if (pDriveJoint     tmp = boost::dynamic_pointer_cast<DriveJoint    >(i->second)) tmp->stepGraphics();
        else if (pGearboxGear    tmp = boost::dynamic_pointer_cast<GearboxGear   >(i->second)) tmp->stepGraphics();
        else if (pBody           tmp = boost::dynamic_pointer_cast<Body          >(i->second)) tmp->stepGraphics();
        else if (pPedal          tmp = boost::dynamic_pointer_cast<Pedal         >(i->second)) tmp->stepGraphics();
        else if (pPart           tmp = boost::dynamic_pointer_cast<Part          >(i->second)) tmp->stepGraphics();
        else if (pVehicle        tmp = boost::dynamic_pointer_cast<Vehicle       >(i->second)) tmp->stepGraphics();
        else if (pArea           tmp = boost::dynamic_pointer_cast<Area          >(i->second)) tmp->stepGraphics();
        else if (pWorld          tmp = boost::dynamic_pointer_cast<World         >(i->second)) tmp->stepGraphics();
        else if (pCamera         tmp = boost::dynamic_pointer_cast<Camera        >(i->second)) tmp->stepGraphics();
        else if (pWorldObject    tmp = boost::dynamic_pointer_cast<WorldObject   >(i->second)) tmp->stepGraphics();
    }
}
void WorldObject::setPosition (Vector3d position)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition(position);
    }
}
Vector3d WorldObject::getPosition ()
{
    return odeObjects.begin()->second->getPosition();
}
Quaternion WorldObject::getRotation ()
{
    return odeObjects.begin()->second->getRotation();
}
void WorldObject::setRotation (Quaternion rotation)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setRotation(rotation);
    }
}
void WorldObject::applyRotation (Quaternion rotation)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition ( rotation.rotateObject(j->second->getPosition()) );
        Quaternion finalRotation = rotation * j->second->getRotation();
        j->second->setRotation (finalRotation);
    }
}
pArea WorldObject::getArea(std::string name)
{
    pArea tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(area)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Area>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Area");
    return tmp;
}

pVehicle WorldObject::getVehicle(std::string name)
{
    pVehicle tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(vehicle)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Vehicle>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Vehicle");
    return tmp;
}
