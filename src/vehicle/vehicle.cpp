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
    log->__format(LOG_DEVELOPER, "Deleting components");
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        delete (i->second);
    }
    components.clear();
}

void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    getGearbox("main")->setGear(2);

    // spread the news to the necessary (input-able) vehicle parts
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second.get())) != 0) tmpPedal->setUserDriver();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->setUserDriver();
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0) tmpSusp->setUserDriver();
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

        pLSD  lsd  = LSD::create();  lsd->setName("rear diff");  components[lsd->getName()] = lsd;
        pGear gear = Gear::create(); gear->setName("transfer"); components[gear->getName()] = gear;
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            bool u=false; //update graphics display; render one frame
            if (t->getName() == "body")
                { pBody tmp = Body::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "engine")
                { pEngine tmp = Engine::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "clutch")
                { pClutch tmp = Clutch::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "gearbox")
                { pGearbox tmp = Gearbox::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "finalDrive")
                { pFinalDrive tmp = FinalDrive::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "pedal")
                { pPedal tmp = Pedal::create(t); components[tmp->getName()]=tmp; }
            if (t->getName() == "wheel")
                { pWheel tmp = Wheel::create(t); components[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.unidimensional")
                { pUnidimensional tmp = Unidimensional::create(t); components[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.fixed")
                { pFixed tmp = Fixed::create(t); components[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "suspension.doublewishbone")
                { pDoubleWishbone tmp = DoubleWishbone::create(t); components[tmp->getName()]=tmp; u=true;}
            if (t->getName() == "camera")
                { pCamera tmp = Camera::create(t); cameras[tmp->getName()]=tmp; u=true;}
            if (u) System::get()->ogreWindow->update ();
        }
    }
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
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
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

    stepGraphics();
}

void Vehicle::setContainer (pWorldObject container)
{
    WorldObject::setContainer(container);
    // tell all the objects who's the boss.
    pWorldObject t = shared_from_this();
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->second) i->second->setContainer(t);
    }
    getGearbox("main")->setContainer(shared_from_this());
    for (CamerasIt i = cameras.begin(); i != cameras.end(); i++)
    {
        i->second->setContainer(shared_from_this());
    }
}

void Vehicle::stepGraphics ()
{
    base->stepGraphics();

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        DoubleWishbone * tmpSusp;
        if ( (tmpSusp = dynamic_cast<DoubleWishbone*>(i->second.get())) != 0) tmpSusp->stepGraphics();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->stepGraphics();
        Body * tmpBody;
        if ( (tmpBody = dynamic_cast<Body*>(i->second.get())) != 0) tmpBody->stepGraphics();
    }
}

pOdeObject Vehicle::getMainOdeObject()
{
    return getObject("(Body)main")->getMainOdeObject();
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER,"Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    getObject("(Body)main")->setPosition ( getObject("(Body)main")->getPosition() + posDiff );
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
            tmpWheel->getMainOdeObject()->setPosition ( tmpWheel->getMainOdeObject()->getPosition() + posDiff );
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0)
            tmpSusp->setPosition ( tmpSusp->getPosition() + posDiff );
    }
}

Vector3d Vehicle::getPosition ()
{
    return getObject("(Body)main")->getPosition();
}

void Vehicle::applyRotation (Quaternion rotation)
{
    // apply rotation to wheels
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0)
            tmpWheel->applyRotation (rotation);
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0)
            tmpSusp->applyRotation (rotation);
    }
    // apply rotation to body
    getObject("(Body)main")->applyRotation (rotation);
}

Quaternion Vehicle::getRotation ()
{
    return getObject("(Body)main")->getRotation();
}

void Vehicle::placeWheelsOnSuspensions()
{
    getWheel("fr")->applyRotation(getSuspension("fr")->getSecondLinkRotation());
    getWheel("fr")->setPosition(getSuspension("fr")->getSecondLinkPosition());
    getWheel("fl")->applyRotation(getSuspension("fl")->getSecondLinkRotation());
    getWheel("fl")->setPosition(getSuspension("fl")->getSecondLinkPosition());
    getWheel("rr")->applyRotation(getSuspension("rr")->getSecondLinkRotation());
    getWheel("rr")->setPosition(getSuspension("rr")->getSecondLinkPosition());
    getWheel("rl")->applyRotation(getSuspension("rl")->getSecondLinkRotation());
    getWheel("rl")->setPosition(getSuspension("rl")->getSecondLinkPosition());
}

void Vehicle::boltWheelsToSuspensions()
{
    getSuspension("fr")->attach(getObject("(Body)main"), getObject("(Wheel)fr"));
    getSuspension("fl")->attach(getObject("(Body)main"), getObject("(Wheel)fl"));
    getSuspension("rr")->attach(getObject("(Body)main"), getObject("(Wheel)rr"));
    getSuspension("rl")->attach(getObject("(Body)main"), getObject("(Wheel)rl"));
}

void Vehicle::stepPhysics ()
{
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second.get())) != 0)
        {
            tmpPedal->stepPhysics();
        }
    }
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

    // step torque transfer components first
    getClutch("main")->stepPhysics();
    getGear("transfer")->stepPhysics();
    getLSD("rear diff")->stepPhysics();
        
    // step rigid bodies    
    getEngine("main")->stepPhysics();
    getGearbox("main")->stepPhysics();
    getFinalDrive("main")->stepPhysics();
    getBody("main")->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Suspension * tmpSusp;
        if ( (tmpSusp = dynamic_cast<Suspension*>(i->second.get())) != 0) tmpSusp->stepPhysics();
        Wheel * tmpWheel;
        if ( (tmpWheel = dynamic_cast<Wheel*>(i->second.get())) != 0) tmpWheel->stepPhysics();
    }

    // print telemetry data
    if ( userDriver )
    {
        const dReal * tmp = dBodyGetLinearVel(getObject("(Body)main")->getMainOdeObject()->getBodyID());
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(getObject("(Body)main")->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, getEngine("main")->getOutputAngularVel(), getFinalDrive("main")->getInputAngularVel(), getWheel("rr")->getInputAngularVel(), getWheel("rl")->getInputAngularVel(), getGearbox("main")->getCurrentGearName().c_str(), distance);
    }
}

pBody Vehicle::getBody (std::string name)
{
    pBody tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Body)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Body>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Body");
    return tmp;
}
pDriveMass Vehicle::getDriveMass (std::string name)
{
    pDriveMass tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(DriveMass)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(Engine)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(FinalDrive)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(Gearbox)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
        if (i->first == ("(Wheel)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveMass>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveMass");
    return tmp;
}
pDriveJoint Vehicle::getDriveJoint (std::string name)
{
    pDriveJoint tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(DriveJoint)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DriveJoint>(i->second)) break;
        if (i->first == ("(Clutch)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Clutch>(i->second)) break;
        if (i->first == ("(Gear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gear>(i->second)) break;
        if (i->first == ("(LSD)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<LSD>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveJoint");
    return tmp;
}
pClutch Vehicle::getClutch (std::string name)
{
    pClutch tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Clutch)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Clutch>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Clutch");
    return tmp;
}
pGear Vehicle::getGear (std::string name)
{
    pGear tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Gear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gear");
    return tmp;
}
pLSD Vehicle::getLSD (std::string name)
{
    pLSD tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(LSD)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<LSD>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "LSD");
    return tmp;
}
pEngine Vehicle::getEngine (std::string name)
{
    pEngine tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Engine)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Engine>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Engine");
    return tmp;
}
pFinalDrive Vehicle::getFinalDrive (std::string name)
{
    pFinalDrive tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(FinalDrive)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<FinalDrive>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "FinalDrive");
    return tmp;
}
pGearbox Vehicle::getGearbox (std::string name)
{
    pGearbox tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Gearbox)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Gearbox>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gearbox");
    return tmp;
}
pGearboxGear Vehicle::getGearboxGear (std::string name)
{
    pGearboxGear tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(GearboxGear)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<GearboxGear>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "GearboxGear");
    return tmp;
}
pPedal Vehicle::getPedal (std::string name)
{
    pPedal tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Pedal)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Pedal>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Pedal");
    return tmp;
}
pSuspension Vehicle::getSuspension (std::string name)
{
    pSuspension tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Suspension)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Suspension>(i->second)) break;
        if (i->first == ("(DoubleWishbone)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<DoubleWishbone>(i->second)) break;
        if (i->first == ("(Fixed)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Fixed>(i->second)) break;
        if (i->first == ("(Unidimensional)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Unidimensional>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Suspension");
    return tmp;
}
pWheel Vehicle::getWheel (std::string name)
{
    pWheel tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(Wheel)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Wheel>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Wheel");
    return tmp;
}
pWorldObject Vehicle::getObject (std::string name)
{
   if (components.find(name) == components.end())
   log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" from generic type \"%s\"", name.c_str(), "WorldObject");
   return components[name];
}
pWorldObject Vehicle::getWorldObject (std::string name)
{
    pWorldObject tmp;
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        if (i->first == ("(WorldObject)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<WorldObject>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "WorldObject");
    return tmp;
}
