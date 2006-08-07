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
#include "mospPath.hpp"

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
    this->userDriver = false;

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
    //TODO use a Class::mospInternalPath()
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
    pOdeObject tmp;
    if (odeObjects.empty()) return tmp;
    return odeObjects.begin()->second;
}
void WorldObject::stepPhysics ()
{
    WorldObjectsIt i = objects.begin();
    //step torque transfer objects first
    for (i = objects.begin(); i != objects.end(); i++)
    {
        if (boost::dynamic_pointer_cast<DriveJoint>(i->second)) i->second->stepPhysics();
    }
    //step the rest of objects
    for (i = objects.begin(); i != objects.end(); i++)
    {
        if (!(boost::dynamic_pointer_cast<DriveJoint>(i->second))) i->second->stepPhysics();
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
        i->second->stepGraphics();
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
pBody WorldObject::getBody (std::string name)
{
    pBody tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(body)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Body>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Body");
    return tmp;
}
pDriveMass WorldObject::getDriveMass (std::string name)
{
    pDriveMass tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(drivemass)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(engine)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(finaldrive)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(gearbox)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(wheel)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveMass");
    return tmp;
}
pDriveJoint WorldObject::getDriveJoint (std::string name)
{
    pDriveJoint tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(drivejoint)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveJoint>(i->second)) break;
        if (i->first == ("(clutch)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Clutch>(i->second)) break;
        if (i->first == ("(gear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gear>(i->second)) break;
        if (i->first == ("(lsd)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<LSD>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveJoint");
    return tmp;
}
pClutch WorldObject::getClutch (std::string name)
{
    pClutch tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(clutch)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Clutch>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Clutch");
    return tmp;
}
pGear WorldObject::getGear (std::string name)
{
    pGear tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gear");
    return tmp;
}
pLSD WorldObject::getLSD (std::string name)
{
    pLSD tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(lsd)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<LSD>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "LSD");
    return tmp;
}
pEngine WorldObject::getEngine (std::string name)
{
    pEngine tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(engine)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Engine>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Engine");
    return tmp;
}
pFinalDrive WorldObject::getFinalDrive (std::string name)
{
    pFinalDrive tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(finaldrive)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<FinalDrive>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "FinalDrive");
    return tmp;
}
pGearbox WorldObject::getGearbox (std::string name)
{
    pGearbox tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gearbox)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gearbox>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gearbox");
    return tmp;
}
pGearboxGear WorldObject::getGearboxGear (std::string name)
{
    pGearboxGear tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gearboxgear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<GearboxGear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "GearboxGear");
    return tmp;
}
pPedal WorldObject::getPedal (std::string name)
{
    pPedal tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(pedal)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Pedal>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Pedal");
    return tmp;
}
pSuspension WorldObject::getSuspension (std::string name)
{
    pSuspension tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(suspension)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Suspension>(i->second)) break;
        if (i->first == ("(doublewishbone)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DoubleWishbone>(i->second)) break;
        if (i->first == ("(fixed)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Fixed>(i->second)) break;
        if (i->first == ("(unidimensional)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Unidimensional>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Suspension");
    return tmp;
}
pWheel WorldObject::getWheel (std::string name)
{
    pWheel tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(wheel)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Wheel>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Wheel");
    return tmp;
}
pWorldObject WorldObject::getObject (std::string name)
{
   if (objects.find(name) == objects.end())
   log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" from generic type \"%s\"", name.c_str(), "WorldObject");
   return objects[name];
}
pWorldObject WorldObject::getWorldObject (std::string name)
{
    pWorldObject tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(worldobject)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<WorldObject>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "WorldObject");
    return tmp;
}
pCamera WorldObject::getCamera (std::string name)
{
    pCamera tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == (name) && i->second) if (tmp = boost::dynamic_pointer_cast<Camera>(i->second)) break;
        if (i->first == ("(camera)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Camera>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Camera");
    return tmp;
}

pCamera WorldObject::switchNextCamera()
{
    WorldObjectsIt i = objects.begin();
    pCamera nextCam;
    bool first = false;
    bool found = false;
    for (;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            if (first == false)
            {
                nextCam = tmp;
                first = true;
            }
            if (found == true)
            {
                nextCam = tmp;
                found = false;
            }
            if (activeCamera == tmp)
            {
                found = true;
            } 
        }
    }
    activeCamera = nextCam;
    return activeCamera;
}
void WorldObject::pointCameras(pVehicle target)
{
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->setTarget(target->getMainOdeObject());
        }
    }
}
void WorldObject::positionCameras(dBodyID baseID)
{
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->setPositionID(baseID);
        }
    }
}
void WorldObject::positionCameras(pVehicle base)
{
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->setPositionID(base->getMainOdeObject()->getBodyID());
        }
    }
}

pLocation WorldObject::getLocation (std::string name)
{
    pLocation tmp;
    for (LocationsIt i = locations.begin(); i != locations.end(); i++)
    {
        if (i->first == name && i->second) if (tmp = boost::dynamic_pointer_cast<Location>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Location");
    return tmp;
}

void WorldObject::constructFromTag(XmlTag * tag)
{
    if (tag == NULL) return;
    if (tag->getName() != name) log->__format(LOG_ERROR, "Expected a <%s> tag, got a <%s>", name.c_str(), tag->getName().c_str());
    XmlTag * t;
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "sharedobject")
        {
            std::string sobjname = t->getAttribute("name");
            if (t->nTags() == 1)
            {
                XmlTag * o = t->getTag(0);
                if (o->getName() == "vehicle")
                {
                    log->loadscreen (LOG_CCREATOR, "Creating a vehicle");
                    std::string model = o->getAttribute("model");
                    pVehicle tmp= Vehicle::create(model);
                    tmp->setName(sobjname);
                    objects[tmp->getName()] = tmp;
                }
                if (o->getName() == "area")
                {
                    log->loadscreen (LOG_CCREATOR, "Creating an area");
                    std::string model = o->getAttribute("model");
                    pArea tmp = Area::create (model);
                    tmp->setName(sobjname);
                    objects[tmp->getName()] = tmp;
                }
            }
        }
    }
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "subobject")
        {
            std::string sobjname = t->getAttribute("name");
            if (t->nTags() == 1)
            {
                XmlTag * o = t->getTag(0);
                if (o->getName() == "body")
                { pBody tmp = Body::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "engine")
                { pEngine tmp = Engine::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "clutch")
                { pClutch tmp = Clutch::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "gearbox")
                { pGearbox tmp = Gearbox::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "gearboxgear")
                { pGearboxGear tmp = GearboxGear::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "gear")
                { pGear tmp = Gear::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "finaldrive")
                { pFinalDrive tmp = FinalDrive::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "pedal")
                { pPedal tmp = Pedal::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "wheel")
                { pWheel tmp = Wheel::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "suspension.unidimensional")
                { pUnidimensional tmp = Unidimensional::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "suspension.fixed")
                { pFixed tmp = Fixed::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "suspension.doublewishbone")
                { pDoubleWishbone tmp = DoubleWishbone::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
                if (o->getName() == "camera")
                { pCamera tmp = Camera::create(o); tmp->setName(sobjname); objects[tmp->getName()] = tmp; }
            }
            System::get()->ogreWindow->update ();
        }
    }
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "location-vehicle")
        {
            log->__format (LOG_CCREATOR, "Setting vehicle location");
            std::string f= t->getAttribute("first");
            std::string s= t->getAttribute("second");
            pLocation first = getLocationObject(f);
            pVehicle second = getVehicleObject(s);

            second->setPosition(Vector3d(0, 0, 0));
            second->applyRotation(first->getRotation());
            second->setPosition(first->getPosition());
        }
    }
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "vehicle-driver")
        {
            log->__format (LOG_CCREATOR, "Setting vehicle driver");
            std::string f= t->getAttribute("first");
            std::string s= t->getAttribute("second");
            pVehicle tmp = getVehicleObject(f);
            if (s == "user" ) tmp->setUserDriver();
        }
    }
    pOgreObjectData ogreData(new OgreObjectData);
    ogreData->meshPath = getPath() + ogreData->meshPath;
    pOgreObject ogreObject (new OgreObject(this, ogreData, getId(), false));
    ogreObjects[ogreObject->getId()] = ogreObject;
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "graphicsMesh")
        {
            pOgreObjectData childData(new OgreObjectData);
            childData->meshPath = getPath() + t->getAttribute("file");
            Vector3d posDiff (t->getAttribute("position"));
            Vector3d scale (t->getAttribute("scale"));
            Quaternion rotDiff (t->getAttribute("rotation"));
            pOgreObject ogreChild (new OgreObject(this, childData, getId()));
            ogreObjects[ogreChild->getId()] = ogreChild;
            ogreChild->setOgreReference(ogreObjects[ogreObject->getId()], rotDiff, posDiff, scale);
            System::get()->ogreWindow->update ();
        }
        if (t->getName() == "rigidBody")
        {
            pPartOdeData data(new PartOdeData);
            XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
            {
                if (u->getName() == "totalMass")
                {
                    data->mass = stod(u->getAttribute("value"));
                }
                if (u->getName() == "box")
                {
                    data->shape = u->getName();
                    data->size = Vector3d(u->getAttribute("size"));
                }
                if (u->getName() == "sphere")
                {
                    data->shape = u->getName();
                    data->radius = stod(u->getAttribute("radius"));
                }
                if (u->getName() == "cappedCylinder")
                {
                    data->shape = u->getName();
                    data->radius = stod(u->getAttribute("radius"));
                    data->length = stod(u->getAttribute("length"));
                    if (u->getAttribute("directionAxis") == "x") data->directionAxis = 1;
                    if (u->getAttribute("directionAxis") == "y") data->directionAxis = 2;
                    if (u->getAttribute("directionAxis") == "z") data->directionAxis = 3;
                }
            }
            if (data->shape == "none") log->__format(LOG_ERROR, "No physics shape specified for this part.");
            odeObjects[getId()] = pOdeObject(new OdeObject(this, data, getId()));
        }
    }
    t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "customData")
        {
            readCustomDataTag(t);
        }
    }
    ogreObjects[ogreObject->getId()]->setOdeReference(getMainOdeObject());
}
pLocation WorldObject::getLocationObject(std::string fullname)
{
    pLocation tmp;
    if (MospPath::getType(fullname) == "area")
    {
        pArea a = getArea(MospPath::getName(fullname));
        log->__format(LOG_DEVELOPER, "Area name=%s", a->getName().c_str());
        tmp = a->getLocation(MospPath::getName(MospPath::getSubFullname(fullname)));
    }
    log->__format(LOG_DEVELOPER, "Location name=%s", tmp->getName().c_str());
    return tmp;
}
pVehicle WorldObject::getVehicleObject(std::string fullname)
{
    pVehicle tmp;
    if ((tmp = boost::dynamic_pointer_cast<Vehicle>(getFirstObject(fullname))) == NULL)
        log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", fullname.c_str(), "Vehicle");
    log->__format(LOG_DEVELOPER, "Vehicle name=%s", tmp->getName().c_str());
    return tmp;
}
pWorldObject WorldObject::getFirstObject(std::string fullname)
{
    std::string type = MospPath::getType(fullname);
    std::string name = MospPath::getName(fullname);
    pWorldObject tmp = getObject("(" + type + ")" + name);
    log->__format(LOG_DEVELOPER, "Object name=%s", tmp->getName().c_str());
    return tmp;
}
