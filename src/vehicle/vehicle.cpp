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
    log->__format(LOG_DEVELOPER, "Deleting body");
    delete body; 
    body = NULL;
    log->__format(LOG_DEVELOPER, "Deleting engine");
    delete engine;
    engine = NULL;
    log->__format(LOG_DEVELOPER, "Deleting clutch");
    delete clutch;
    clutch = NULL;
    log->__format(LOG_DEVELOPER, "Deleting transfer");
    delete transfer;
    transfer = NULL;
    log->__format(LOG_DEVELOPER, "Deleting rearDiff");
    delete rearDiff;
    rearDiff = NULL;
    log->__format(LOG_DEVELOPER, "Deleting gearbox");
    delete gearbox;
    gearbox = NULL;
    log->__format(LOG_DEVELOPER, "Deleting finalDrive");
    delete finalDrive;
    finalDrive = NULL;
    
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
    log->__format(LOG_DEVELOPER, "Deleting pedals");
    std::map < std::string, Pedal * >::iterator pedalIter;
    for (pedalIter=pedalMap.begin(); pedalIter != pedalMap.end(); pedalIter++)
    {
        delete pedalIter->second;
        pedalIter->second = NULL;
        pedalMap.erase(pedalIter);
    }
}
void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    gearbox->setGear(2);

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
    std::map < std::string, Pedal * >::const_iterator pedalIter;
    for (pedalIter=pedalMap.begin(); pedalIter != pedalMap.end(); pedalIter++)
    {
        pedalIter->second->setUserDriver();
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

        rearDiff   = new LSD        (this);
        transfer   = new Gear       (this);
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "body")       body       = new Body       (this, t);
            //if (t->getName() == "body.path")  body       = new Body       (this, t->getAttribute("path"));
            if (t->getName() == "engine")     engine     = new Engine     (this, t);
            if (t->getName() == "clutch")     clutch     = new Clutch     (this, t);
            if (t->getName() == "gearbox")    gearbox    = new Gearbox    (this, t);
            if (t->getName() == "finalDrive") finalDrive = new FinalDrive (this, t);
            if (t->getName() == "pedalList")
            {
                XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
                {
                    if (u->getName() == "pedal")
                    {
                        Pedal * tmp = new Pedal (this, u);
                        pedalMap[tmp->getName()] = tmp;
                    }
                }
            }
            if (t->getName() == "wheelList")
            {
                XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
                {
                    if (u->getName() == "wheel")
                    {
                        Wheel * tmp = new Wheel (this, u);
                        wheelMap[tmp->getName()] = tmp;
                    }
                }
            }
            if (t->getName() == "suspensionList")
            {
                XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
                {
                    if (u->getName() == "suspension.unidimensional")
                    {
                        Suspension * tmp = new Suspension (this, u);
                        suspensionMap[tmp->getName()] = tmp;
                    }
                }
            }
            if (t->getName() == "cameraList")
            {
                XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
                {
                    if (u->getName() == "camera")
                    {
                        Camera * tmp = new Camera(this, u);
                        cameraList.push_back(tmp);
                    }
                }
            }
        }
    }

    clutch->setOutputPointer(gearbox);
    transfer->setOutputPointer(finalDrive);
    
    clutch->setInputPointer(engine);
    transfer->setInputPointer(gearbox);
    rearDiff->setInputPointer(finalDrive);
        
    placeWheelsOnSuspensions();
    boltWheelsToSuspensions();

    rearDiff->setOutputPointer(wheelMap["RearRight"]);
    rearDiff->setOutputPointer2(wheelMap["RearLeft"]);

    engine->setGasPedal(pedalMap["gasPedal"]);
    clutch->setClutchPedal(pedalMap["clutchPedal"]);
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->setRefBody(body->getMainOdeObject()->getBodyID());
        wheelIter->second->setBrakePedal(pedalMap["brakePedal"]);;
    }

    clutch->enable();
    transfer->enable();
    rearDiff->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    gearbox->setGear(2);

    stepGraphics();
}

void Vehicle::stepGraphics ()
{
    base->stepGraphics();
    body->stepGraphics();
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
    return body->getMainOdeObject();
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER, "Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    
    body->setPosition ( body->getPosition() + posDiff );

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
    return body->getPosition();
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
    body->applyRotation (rotation);

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
    return body->getRotation();
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
            suspIter->second->attach(body, wheelIter->second);
        }
    }
}

void Vehicle::stepPhysics ()
{
    std::map < std::string, Pedal * >::const_iterator pedalIter;
    for (pedalIter=pedalMap.begin(); pedalIter != pedalMap.end(); pedalIter++)
    {
        pedalIter->second->stepPhysics();
    }
    if ( userDriver )
    {
        // check the gearshift levers
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue() == 1)
        {
            gearbox->gearUp();
        }
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_z)]->getValue() == 1)
        {
            gearbox->gearDown();
        }
    }

/*
    // Code for using a higher rate in tranny code, thus eliminating some lag from it. FIXME when double seconds are used instead of int milliseconds.
    const int freq = 3;
    SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() / freq);
    for (int i = 0; i<freq; i++){
// higher rate code continues below... */
    // step torque transfer components first

    clutch->stepPhysics();
    transfer->stepPhysics();
    rearDiff->stepPhysics();
        
    // step rigid bodies    
    engine->stepPhysics();
    gearbox->stepPhysics();
    finalDrive->stepPhysics();
    body->stepPhysics();
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
        const dReal * tmp = dBodyGetLinearVel(body->getMainOdeObject()->getBodyID());
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(body->getMainOdeObject()->getBodyID());
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, engine->getOutputAngularVel(), finalDrive->getInputAngularVel(), wheelMap["RearRight"]->getInputAngularVel(), wheelMap["RearLeft"]->getInputAngularVel(), gearbox->getCurrentGearName().c_str(), distance);
    }
}
