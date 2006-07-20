/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "world.hpp"
#include "vehicle.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "body.hpp"
#include "engine.hpp"
#include "driveJoint.hpp"
#include "gearbox.hpp"
#include "finalDrive.hpp"
#include "wheel.hpp"
#include "suspension.hpp"
#include "pedal.hpp"
#include "camera.hpp"
#include "system.hpp"
#include "SDL.h"
#include "SDL/SDL_keysym.h"
#include "quaternion.hpp"

pVehicle Vehicle::create(std::string vehicleName)
{
    pVehicle vehicle(new Vehicle(vehicleName));
    return vehicle;
}
Vehicle::Vehicle (std::string vehicleName)
    :WorldObject(vehicleName)
{
    setPath(Paths::vehicle(vehicleName));
    System::get()->setCurrentPath(Paths::vehicle(vehicleName));
    setXmlPath(Paths::vehicleXml(vehicleName));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "vehicles." + vehicleName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath()+"skybox.zip","Zip","vehicles."+vehicleName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("vehicles." + vehicleName);

    XmlTag * tag = new XmlTag (getXmlPath());
    construct (tag);
    delete tag;

    userDriver = false;
}

Vehicle::~Vehicle ()
{
    log->__format(LOG_DEVELOPER, "Deleting objects");
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        delete (i->second);
    }
    objects.clear();
}

void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->__format(LOG_ENDUSER, "Vehicle %s will now be driven by a human.", getName().c_str());
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    getGearbox("main")->setGear(2);

    // spread the news to the necessary (input-able) vehicle parts
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        pBody tmpBody;
        if ( (tmpBody = boost::dynamic_pointer_cast<Body>(i->second))) tmpBody->setUserDriver();
        pPedal tmpPedal;
        if ( (tmpPedal = boost::dynamic_pointer_cast<Pedal>(i->second))) tmpPedal->setUserDriver();
        pWheel tmpWheel;
        if ( (tmpWheel = boost::dynamic_pointer_cast<Wheel>(i->second))) tmpWheel->setUserDriver();
        pSuspension tmpSusp;
        if ( (tmpSusp = boost::dynamic_pointer_cast<Suspension>(i->second))) tmpSusp->setUserDriver();
    }
}

void Vehicle::construct (XmlTag * tag)
{
    if (tag->getName() == "vehicle")
    {
        setName (     tag->getAttribute("name"));
        description = tag->getAttribute("description");
        author =      tag->getAttribute("author");
        contact =     tag->getAttribute("contact");
        license =     tag->getAttribute("license");

        pLSD  lsd  = LSD::create();  lsd->setName("rear diff");  objects[lsd->getName()] = lsd;
        pGear gear = Gear::create(); gear->setName("transfer"); objects[gear->getName()] = gear;
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            bool u=false; //update graphics display; render one frame
            if (t->getName() == "body")
                { pBody tmp = Body::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "engine")
                { pEngine tmp = Engine::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "clutch")
                { pClutch tmp = Clutch::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "gearbox")
                { pGearbox tmp = Gearbox::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "finalDrive")
                { pFinalDrive tmp = FinalDrive::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "pedal")
                { pPedal tmp = Pedal::create(t); objects[tmp->getName()]=tmp; }
            if (t->getName() == "wheel")
                { pWheel tmp = Wheel::create(t); objects[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.unidimensional")
                { pUnidimensional tmp = Unidimensional::create(t); objects[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.fixed")
                { pFixed tmp = Fixed::create(t); objects[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.doublewishbone")
                { pDoubleWishbone tmp = DoubleWishbone::create(t); objects[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "camera")
                { pCamera tmp = Camera::create(t); objects[tmp->getName()]=tmp; u=true;}
            if (u) System::get()->ogreWindow->update ();
        }
    }
    //initialize objects...
    pointCameras();
    // spread the news to the necessary (input-able) vehicle parts
    log->__format(LOG_DEVELOPER, "Setting some drive joint pointers...");
    getClutch("main")->setOutputPointer(getGearbox("main"));
    getDriveJoint("transfer")->setOutputPointer(getFinalDrive("main"));
    
    getClutch("main")->setInputPointer(getEngine("main"));
    getDriveJoint("transfer")->setInputPointer(getGearbox("main"));
    getDriveJoint("rear diff")->setInputPointer(getFinalDrive("main"));
        
    log->__format(LOG_DEVELOPER, "Placing wheels on suspensions...");
    placeWheelsOnSuspensions();
    log->__format(LOG_DEVELOPER, "Bolting wheels to suspensions...");
    boltWheelsToSuspensions();

    log->__format(LOG_DEVELOPER, "Setting some more drive joint pointers...");
    getDriveJoint("rear diff")->setOutputPointer(getWheel("rr"));
    getLSD("rear diff")->setOutputPointer2(getWheel("rl"));

    getEngine("main")->setGasPedal(getPedal("gas"));
    getClutch("main")->setClutchPedal(getPedal("clutch"));
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        pWheel tmpWheel;
        if ((tmpWheel = boost::dynamic_pointer_cast<Wheel>(i->second)))
        {
            tmpWheel->setBrakePedal(getPedal("brake"));
        }
    }

    getClutch("main")->enable();
    getDriveJoint("transfer")->enable();
    getDriveJoint("rear diff")->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    getGearbox("main")->setGear(2);

    //update all objects positions
    stepGraphics();
}

pOdeObject Vehicle::getMainOdeObject()
{
    return getBody("main")->getMainOdeObject();
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER,"Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    getBody("main")->setPosition ( getBody("main")->getPosition() + posDiff );
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        pWheel tmpWheel;
        if ( (tmpWheel = boost::dynamic_pointer_cast<Wheel>(i->second)))
            tmpWheel->getMainOdeObject()->setPosition ( tmpWheel->getMainOdeObject()->getPosition() + posDiff );
        pSuspension tmpSusp;
        if ( (tmpSusp = boost::dynamic_pointer_cast<Suspension>(i->second)))
            tmpSusp->setPosition ( tmpSusp->getPosition() + posDiff );
    }
}

Vector3d Vehicle::getPosition ()
{
    return getBody("main")->getPosition();
}

void Vehicle::applyRotation (Quaternion rotation)
{
    // apply rotation to wheels
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        pWheel tmpWheel;
        if ( (tmpWheel = boost::dynamic_pointer_cast<Wheel>(i->second))) tmpWheel->applyRotation (rotation);
        pSuspension tmpSusp;
        if ( (tmpSusp = boost::dynamic_pointer_cast<Suspension>(i->second))) tmpSusp->applyRotation (rotation);
    }
    // apply rotation to body
    getBody("main")->applyRotation (rotation);
}

Quaternion Vehicle::getRotation ()
{
    return getBody("main")->getRotation();
}

void Vehicle::placeWheelsOnSuspensions()
{
    getWheel("fr")->applyRotation(getSuspension("fr")->getSecondLinkRotation());
    getWheel("fr")->setPosition  (getSuspension("fr")->getSecondLinkPosition());
    getWheel("fl")->applyRotation(getSuspension("fl")->getSecondLinkRotation());
    getWheel("fl")->setPosition  (getSuspension("fl")->getSecondLinkPosition());
    getWheel("rr")->applyRotation(getSuspension("rr")->getSecondLinkRotation());
    getWheel("rr")->setPosition  (getSuspension("rr")->getSecondLinkPosition());
    getWheel("rl")->applyRotation(getSuspension("rl")->getSecondLinkRotation());
    getWheel("rl")->setPosition  (getSuspension("rl")->getSecondLinkPosition());
}

void Vehicle::boltWheelsToSuspensions()
{
    getSuspension("fr")->attach(getBody("main"), getWheel("fr"));
    getSuspension("fl")->attach(getBody("main"), getWheel("fl"));
    getSuspension("rr")->attach(getBody("main"), getWheel("rr"));
    getSuspension("rl")->attach(getBody("main"), getWheel("rl"));
}

void Vehicle::stepPhysics ()
{
    WorldObject::stepPhysics();
    if ( userDriver )
    {
        // check the gearshift levers
        if(System::get()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue() == 1)
        {
            getGearbox("main")->gearUp();
        }
        if(System::get()->axisMap[getIDKeyboardKey(SDLK_z)]->getValue() == 1)
        {
            getGearbox("main")->gearDown();
        }
    }

    // print telemetry data
    if ( userDriver )
    {
        const dReal * tmp = dBodyGetLinearVel(getBody("main")->getMainOdeObject()->getBodyID());
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(getBody("main")->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, getEngine("main")->getOutputAngularVel(), getFinalDrive("main")->getInputAngularVel(), getWheel("rr")->getInputAngularVel(), getWheel("rl")->getInputAngularVel(), getGearbox("main")->getCurrentGearName().c_str(), distance);
    }
}

pBody Vehicle::getBody (std::string name)
{
    pBody tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(body)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Body>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Body");
    return tmp;
}
pDriveMass Vehicle::getDriveMass (std::string name)
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
pDriveJoint Vehicle::getDriveJoint (std::string name)
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
pClutch Vehicle::getClutch (std::string name)
{
    pClutch tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(clutch)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Clutch>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Clutch");
    return tmp;
}
pGear Vehicle::getGear (std::string name)
{
    pGear tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gear");
    return tmp;
}
pLSD Vehicle::getLSD (std::string name)
{
    pLSD tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(lsd)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<LSD>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "LSD");
    return tmp;
}
pEngine Vehicle::getEngine (std::string name)
{
    pEngine tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(engine)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Engine>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Engine");
    return tmp;
}
pFinalDrive Vehicle::getFinalDrive (std::string name)
{
    pFinalDrive tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(finaldrive)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<FinalDrive>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "FinalDrive");
    return tmp;
}
pGearbox Vehicle::getGearbox (std::string name)
{
    pGearbox tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gearbox)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gearbox>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gearbox");
    return tmp;
}
pGearboxGear Vehicle::getGearboxGear (std::string name)
{
    pGearboxGear tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(gearboxgear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<GearboxGear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "GearboxGear");
    return tmp;
}
pPedal Vehicle::getPedal (std::string name)
{
    pPedal tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(pedal)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Pedal>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Pedal");
    return tmp;
}
pSuspension Vehicle::getSuspension (std::string name)
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
pWheel Vehicle::getWheel (std::string name)
{
    pWheel tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(wheel)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Wheel>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Wheel");
    return tmp;
}
pWorldObject Vehicle::getObject (std::string name)
{
   if (objects.find(name) == objects.end())
   log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" from generic type \"%s\"", name.c_str(), "WorldObject");
   return objects[name];
}
pWorldObject Vehicle::getWorldObject (std::string name)
{
    pWorldObject tmp;
    for (WorldObjectsIt i = objects.begin(); i != objects.end(); i++)
    {
        if (i->first == ("(worldobject)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<WorldObject>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "WorldObject");
    return tmp;
}
pCamera Vehicle::getCamera (std::string name)
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

pCamera Vehicle::switchNextCamera()
{
    WorldObjectsIt i = objects.begin();
    pCamera tmp;
    pCamera nextCam;
    bool first = false;
    bool found = false;
    for (;i != objects.end(); i++)
    {
        if (tmp = boost::dynamic_pointer_cast<Camera>(i->second))
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
void Vehicle::pointCameras()
{
    WorldObjectsIt i = objects.begin();
    pCamera tmp;
    for(;i != objects.end(); i++)
    {
        if (tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->setPositionID( getMainOdeObject()->getBodyID());
            tmp->setTarget( getMainOdeObject());
        }
    }
}
