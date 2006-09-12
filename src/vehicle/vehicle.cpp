/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
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
    vehicle->pointCameras(vehicle);
    vehicle->positionCameras(vehicle);
    //update all objects positions
    vehicle->stepGraphics();
    return vehicle;
}
Vehicle::Vehicle (std::string vehicleName)
    :WorldObject("vehicle")
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

void Vehicle::readCustomDataTag(XmlTag * tag)
{
    description = tag->getAttribute("description");
    author =      tag->getAttribute("author");
    contact =     tag->getAttribute("contact");
    license =     tag->getAttribute("license");
}
void Vehicle::construct (XmlTag * tag)
{
    pLSD  lsd  = LSD::create();  lsd->setName("rear diff");  objects[lsd->getName()] = lsd;
    pGear gear = Gear::create(); gear->setName("transfer"); objects[gear->getName()] = gear;

    constructFromTag(tag);

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
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]) * 3.6;
        tmp = dBodyGetPosition(getBody("main")->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, getEngine("main")->getOutputAngularVel(), getFinalDrive("main")->getInputAngularVel(), getWheel("rr")->getInputAngularVel(), getWheel("rl")->getInputAngularVel(), getGearbox("main")->getCurrentGear()->getDescription().c_str(), distance);
    }
}
