/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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

Vehicle::Vehicle (WorldObject * container, std::string vehicleName)
    :WorldObject(container, vehicleName)
{
    setPath(Paths::vehicle(vehicleName));
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
    
    log->__format(LOG_DEVELOPER, "Deleting suspensions");
    std::map < std::string, Suspension * >::iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        delete suspIter->second;
        suspIter->second = NULL;
        suspensionMap.erase(suspIter);
    }
    log->__format(LOG_DEVELOPER, "Deleting wheels");
    std::map < std::string, Wheel * >::iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        delete wheelIter->second;
        wheelIter->second = NULL;
        wheelMap.erase(wheelIter);
    }
}
void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    getGearbox("gearbox")->setGear(2);

    // spread the news to the necessary (input-able) vehicle parts
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->setUserDriver();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->setUserDriver();
    }
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second)) != 0)
        {
            tmpPedal->setUserDriver();
        }
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

        components["rearDiff"] = new LSD (this);
        components["transfer"] = new Gear(this);
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "body")         components["body"]       = new Body (this, t);
            if (t->getName() == "engine")       components["engine"]     = new Engine (this, t);
            if (t->getName() == "clutch")       components["clutch"]     = new Clutch     (this, t);
            if (t->getName() == "gearbox")      components["gearbox"]    = new Gearbox    (this, t);
            if (t->getName() == "finalDrive")   components["finalDrive"] = new FinalDrive (this, t);
            if (t->getName() == "pedal")
            {
                Pedal * tmp = new Pedal (this, t);
                components[tmp->getName()] = tmp;
            }
            if (t->getName() == "wheel")
            {
                Wheel * tmp = new Wheel (this, t);
                wheelMap[tmp->getName()] = tmp;
            }
            if (t->getName() == "suspension.unidimensional")
            {
                Suspension * tmp = new Suspension (this, t);
                suspensionMap[tmp->getName()] = tmp;
            }
            if (t->getName() == "camera")
            {
                Camera * tmp = new Camera(this, t);
                cameraList.push_back(tmp);
            }
        }
    }

    getDriveJoint("clutch")->setOutputPointer(getDriveMass("gearbox"));
    getDriveJoint("transfer")->setOutputPointer(getDriveMass("finalDrive"));
    
    getDriveJoint("clutch")->setInputPointer(getDriveMass("engine"));
    getDriveJoint("transfer")->setInputPointer(getDriveMass("gearbox"));
    getDriveJoint("rearDiff")->setInputPointer(getDriveMass("finalDrive"));
        
    placeWheelsOnSuspensions();
    boltWheelsToSuspensions();

    getDriveJoint("rearDiff")->setOutputPointer(dynamic_cast<DriveMass*>(wheelMap["RearRight"]));
    getLSD("rearDiff")->setOutputPointer2(dynamic_cast<DriveMass*>(wheelMap["RearLeft"]));

    getEngine("engine")->setGasPedal(getPedal("gasPedal"));
    getClutch("clutch")->setClutchPedal(getPedal("clutchPedal"));
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->setRefBody(getObject("body")->getMainOdeObject()->getBodyID());
        wheelIter->second->setBrakePedal(getPedal("brakePedal"));
    }

    getDriveJoint("clutch")->enable();
    getDriveJoint("transfer")->enable();
    getDriveJoint("rearDiff")->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    getGearbox("gearbox")->setGear(2);

    stepGraphics();
}

void Vehicle::stepGraphics ()
{
    base->stepGraphics();
    getBody("body")->stepGraphics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    //FIXME: uncomment when suspensions have graphics representation
    /*
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stepGraphics();
    }
    */
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stepGraphics();
    }
}

OdeObject * Vehicle::getMainOdeObject()
{
    return getObject("body")->getMainOdeObject();
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER, "Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    
    getObject("body")->setPosition ( getObject("body")->getPosition() + posDiff );

    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        wheelIter->second->getMainOdeObject()->setPosition ( wheelIter->second->getMainOdeObject()->getPosition() + posDiff );
    }

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        // suspension position needs not be re-set.
        //suspIter->second->setPosition ( suspIter->second->getPosition() + posDiff );
    }
}

Vector3d Vehicle::getPosition ()
{
    return getObject("body")->getPosition();
}

void Vehicle::applyRotation (Quaternion rotation)
{
    // apply rotation to wheels
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        wheelIter->second->applyRotation (rotation);
    }

    // apply rotation to body
    getObject("body")->applyRotation (rotation);

    // apply rotation to suspensions
/*
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        // suspension position needs not be re-set.
        //suspIter->second->applyRotation (rotation);
    }
*/
}

Quaternion Vehicle::getRotation ()
{
    return getObject("body")->getRotation();
}

void Vehicle::placeWheelsOnSuspensions()
{
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        std::map < std::string, Wheel *>::iterator wheelIter =  wheelMap.find(suspIter->first);
        if (wheelIter == wheelMap.end())
        {
            log->__format (LOG_ERROR, "No \"%s\" wheel was found!", suspIter->first.c_str());
        }else{
            log->__format (LOG_DEVELOPER, "Placing wheel on suspension \"%s\"", suspIter->first.c_str());
            wheelIter->second->applyRotation (suspIter->second->getInitialWheelRotation());
            wheelIter->second->getMainOdeObject()->setPosition (suspIter->second->getInitialWheelPosition());
        }
    }
}

void Vehicle::boltWheelsToSuspensions()
{
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        std::map < std::string, Wheel *>::iterator wheelIter =  wheelMap.find(suspIter->first);
        if (wheelIter == wheelMap.end())
        {
            log->__format (LOG_ERROR, "No \"%s\" wheel was found!", suspIter->first.c_str());
        }else{
            log->__format (LOG_DEVELOPER, "Bolting wheel to suspension \"%s\"", suspIter->first.c_str());
            suspIter->second->attach(getObject("body"), wheelIter->second);
        }
    }
}

void Vehicle::stepPhysics ()
{
    for (WorldObjectsIt i = components.begin(); i != components.end(); i++)
    {
        Pedal * tmpPedal;
        if ( (tmpPedal = dynamic_cast<Pedal*>(i->second)) != 0)
        {
            tmpPedal->stepPhysics();
        }
    }
    if ( userDriver )
    {
        // check the gearshift levers
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue() == 1)
        {
            getGearbox("gearbox")->gearUp();
        }
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_z)]->getValue() == 1)
        {
            getGearbox("gearbox")->gearDown();
        }
    }

/*
    // Code for using a higher rate in tranny code, thus eliminating some lag from it. FIXME when double seconds are used instead of int milliseconds.
    const int freq = 3;
    SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() / freq);
    for (int i = 0; i<freq; i++){
// higher rate code continues below... */
    // step torque transfer components first

    getClutch("clutch")->stepPhysics();
    getGear("transfer")->stepPhysics();
    getLSD("rearDiff")->stepPhysics();
        
    // step rigid bodies    
    getEngine("engine")->stepPhysics();
    getGearbox("gearbox")->stepPhysics();
    getFinalDrive("finalDrive")->stepPhysics();
    getBody("body")->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
//        SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * freq);
        suspIter->second->stepPhysics();
//        SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() / freq);
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stepPhysics();
    }
//    }
//    SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * freq);

    // print telemetry data
    if ( userDriver )
    {
        const dReal * tmp = dBodyGetLinearVel(getObject("body")->getMainOdeObject()->getBodyID());
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(getObject("body")->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, getDriveMass("engine")->getOutputAngularVel(), getDriveMass("finalDrive")->getInputAngularVel(), wheelMap["RearRight"]->getInputAngularVel(), wheelMap["RearLeft"]->getInputAngularVel(), getGearbox("gearbox")->getCurrentGearName().c_str(), distance);
    }
}

Body *          Vehicle::getBody         (std::string name)
{
     Body * tmp = dynamic_cast<Body*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Body");
     return tmp;
}
DriveMass *    Vehicle::getDriveMass   (std::string name)
{
     DriveMass * tmp = dynamic_cast<DriveMass*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveMass");
     return tmp;
}
DriveJoint *    Vehicle::getDriveJoint   (std::string name)
{
     DriveJoint * tmp = dynamic_cast<DriveJoint*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "DriveJoint");
     return tmp;
}
Clutch *        Vehicle::getClutch       (std::string name)
{
     Clutch * tmp = dynamic_cast<Clutch*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Clutch");
     return tmp;
}
Gear *          Vehicle::getGear         (std::string name)
{
     Gear * tmp = dynamic_cast<Gear*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gear");
     return tmp;
}
LSD *           Vehicle::getLSD          (std::string name)
{
     LSD * tmp = dynamic_cast<LSD*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "LSD");
     return tmp;
}
Engine *        Vehicle::getEngine       (std::string name)
{
     Engine * tmp = dynamic_cast<Engine*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Engine");
     return tmp;
}
FinalDrive *    Vehicle::getFinalDrive   (std::string name)
{
     FinalDrive * tmp = dynamic_cast<FinalDrive*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "FinalDrive");
     return tmp;
}
Gearbox *       Vehicle::getGearbox      (std::string name)
{
     Gearbox * tmp = dynamic_cast<Gearbox*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Gearbox");
     return tmp;
}
GearboxGear *   Vehicle::getGearboxGear  (std::string name)
{
     GearboxGear * tmp = dynamic_cast<GearboxGear*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "GearboxGear");
     return tmp;
}
Pedal *         Vehicle::getPedal        (std::string name)
{
     Pedal * tmp = dynamic_cast<Pedal*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Pedal");
     return tmp;
}
Suspension *    Vehicle::getSuspension   (std::string name)
{
     Suspension * tmp = dynamic_cast<Suspension*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Suspension");
     return tmp;
}
Wheel *         Vehicle::getWheel        (std::string name)
{
     Wheel * tmp = dynamic_cast<Wheel*>(components[name]);
     if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Wheel");
     return tmp;
}
WorldObject *         Vehicle::getObject        (std::string name)
{
    if (components.find(name) == components.end())
    log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "WorldObject");
    return components[name];
}
