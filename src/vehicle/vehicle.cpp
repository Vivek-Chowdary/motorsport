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
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
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


int Vehicle::instancesCount = 0;

Vehicle::Vehicle (const std::string & vehicleName)
    :WorldObject(vehicleName)
{
    relativeVehicleDir = vehicleName;
    std::string vehiclePath = Paths::vehicle(vehicleName);
    std::string vehicleXmlPath = Paths::vehicleXml(vehicleName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(vehiclePath, "FileSystem", "vehicles - " + vehicleName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(vehiclePath + "skybox.zip", "Zip", "vehicles - " + vehicleName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("vehicles - " + vehicleName);
    log->loadscreen (LOG_ENDUSER, "Starting to load a vehicle (%s)", vehicleXmlPath.c_str());
    double time = SDL_GetTicks()/1000.0;
    XmlFile * xmlFile = new XmlFile (vehicleXmlPath.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
    log->loadscreen (LOG_ENDUSER, "Finished loading a vehicle (%s). %f seconds.", vehicleXmlPath.c_str(), SDL_GetTicks()/1000.0 - time);

    userDriver = false;
    instancesCount++;
}

Vehicle::~Vehicle ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();

    delete body; 
    delete engine;
    delete clutch;
    delete transfer;
    delete rearDiff;
    delete gearbox;
    delete finalDrive;
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        delete suspIter->second;
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        delete wheelIter->second;
    }
    std::map < std::string, Pedal * >::const_iterator pedalIter;
    for (pedalIter=pedalMap.begin(); pedalIter != pedalMap.end(); pedalIter++)
    {
        delete pedalIter->second;
    }
}
std::string Vehicle::getRelativeVehicleDir()
{
    return relativeVehicleDir;
}

void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    gearbox->setGear(1);

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

void Vehicle::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    name = "None";
    revision = 0;
    description = "None";
    author = "Anonymous";
    contact = "None";
    license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    DOMNode * bodyNode = 0;
    DOMNode * engineNode = 0;
    DOMNode * clutchNode = 0;
    DOMNode * gearboxNode = 0;
    DOMNode * finalDriveNode = 0;
    DOMNode * wheelListNode = 0;
    DOMNode * suspListNode = 0; //suspension
    DOMNode * cameraListNode = 0;
    DOMNode * pedalListNode = 0;

    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "vehicle")
            {
                log->__format (LOG_CCREATOR, "Found a vehicle.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "position")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the position: %s", attribute.c_str());
                            Vector3d position(attribute);
                        }
                        if (attribute == "name")
                        {
                            assignXmlString (name, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the name: %s", name.c_str());
                        }
                        if (attribute == "revision")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the revision number: %s", attribute.c_str());
                            revision = stoi(attribute);
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (description, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the description: %s", description.c_str());
                        }
                        if (attribute == "author")
                        {
                            assignXmlString (author, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the author: %s", author.c_str());
                        }
                        if (attribute == "contact")
                        {
                            assignXmlString (contact, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the author contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (license, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the license: %s", license.c_str());
                        }
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            assignXmlString (nodeName, n->getNodeName());
                            if (nodeName == "body")
                            {
                                log->__format (LOG_CCREATOR, "Found a body.");
                                bodyNode = n;
                            }
                            if (nodeName == "engine")
                            {
                                log->__format (LOG_CCREATOR, "Found an engine.");
                                engineNode = n;
                            }
                            if (nodeName == "clutch")
                            {
                                log->__format (LOG_CCREATOR, "Found a clutch.");
                                clutchNode = n;
                            }
                            if (nodeName == "gearbox")
                            {
                                log->__format (LOG_CCREATOR, "Found a gearbox.");
                                gearboxNode = n;
                            }
                            if (nodeName == "diff")
                            {
                                log->__format (LOG_CCREATOR, "Found a diff.");
                                finalDriveNode = n;
                            }
                            if (nodeName == "wheelList")
                            {
                                log->__format (LOG_CCREATOR, "Found a wheel list.");
                                wheelListNode = n;
                            }
                            if (nodeName == "pedalList")
                            {
                                log->__format (LOG_CCREATOR, "Found a pedal list.");
                                pedalListNode = n;
                            }
                            if (nodeName == "suspensionList")
                            {
                                log->__format (LOG_CCREATOR, "Found a suspension list.");
                                suspListNode = n;
                            }
                            if (nodeName == "cameraList")
                            {
                                log->__format (LOG_CCREATOR, "Found a camera list.");
                                cameraListNode = n;
                            }
                        }
                    }
                }
            }
        }
    }

    log->loadscreen (LOG_CCREATOR, "Creating the vehicle cameras");
    processXmlCameraListNode(cameraListNode);

    log->loadscreen (LOG_CCREATOR, "Creating the vehicle components");
    
    processXmlPedalListNode(pedalListNode);

    body = new Body (bodyNode, this);
    engine = new Engine (engineNode, this);
    clutch = new Clutch (clutchNode, this);
    gearbox = new Gearbox (gearboxNode, this);
    finalDrive = new FinalDrive (finalDriveNode, this);
    transfer = new Gear (this);
    rearDiff = new LSD (this);
    
    log->loadscreen (LOG_CCREATOR, "Attaching the vehicle components together");
    clutch->setOutputPointer(gearbox);
    transfer->setOutputPointer(finalDrive);
    
    clutch->setInputPointer(engine);
    transfer->setInputPointer(gearbox);
    rearDiff->setInputPointer(finalDrive);
        
    processXmlSuspensionListNode(suspListNode);
    processXmlWheelListNode(wheelListNode);
    placeWheelsOnSuspensions();
    boltWheelsToSuspensions();

    rearDiff->setOutputPointer(wheelMap["RearRight"]);
    rearDiff->setOutputPointer2(wheelMap["RearLeft"]);

    engine->setGasPedal(pedalMap["gasPedal"]);
    clutch->setClutchPedal(pedalMap["clutchPedal"]);
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->setBrakePedal(pedalMap["brakePedal"]);;
    }

    clutch->enable();
    transfer->enable();
    rearDiff->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    gearbox->setGear(2);

    startPhysics(NULL);
    stepGraphics();
}

void Vehicle::processXmlPedalListNode(DOMNode * pedalListNode)
{
    if (pedalListNode != 0)
    {
        DOMNode * pedalNode;
        for (pedalNode = pedalListNode->getFirstChild (); pedalNode != 0; pedalNode = pedalNode->getNextSibling ())
        {
            if (pedalNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, pedalNode->getNodeName());
                if (nodeName == "pedal")
                {
                    log->__format (LOG_CCREATOR, "Found a pedal.");
                    Pedal * tmpPedal = new Pedal (pedalNode, this);
                    pedalMap[tmpPedal->getId()]=tmpPedal;
                }
            }
        }
    }
}
void Vehicle::processXmlWheelListNode(DOMNode * wheelListNode)
{
    if (wheelListNode != 0)
    {
        DOMNode * wheelNode;
        for (wheelNode = wheelListNode->getFirstChild (); wheelNode != 0; wheelNode = wheelNode->getNextSibling ())
        {
            if (wheelNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, wheelNode->getNodeName());
                if (nodeName == "wheel")
                {
                    log->__format (LOG_CCREATOR, "Found a wheel.");
                    Wheel * tmpWheel = new Wheel (wheelNode, this);
                    wheelMap[tmpWheel->getIndex()]=tmpWheel;
                    tmpWheel->setRefBody(body->bodyID);
                }
            }
        }
    }
}

void Vehicle::processXmlSuspensionListNode(DOMNode * suspListNode)
{
    if (suspListNode != 0)
    {
        DOMNode * suspNode;
        for (suspNode = suspListNode->getFirstChild (); suspNode != 0; suspNode = suspNode->getNextSibling ())
        {
            if (suspNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, suspNode->getNodeName());
                if (nodeName == "unidimensional")
                {
                    log->__format (LOG_CCREATOR, "Found a suspension.");
                    Suspension * tmpSusp = new Suspension (suspNode, this);
                    suspensionMap[tmpSusp->getIndex()]=tmpSusp;
                }
            }
        }
    }
}

void Vehicle::processXmlCameraListNode(DOMNode * cameraListNode)
{
    if (cameraListNode != 0)
    {
        DOMNode * cameraNode;
        for (cameraNode = cameraListNode->getFirstChild (); cameraNode != 0; cameraNode = cameraNode->getNextSibling ())
        {
            if (cameraNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, cameraNode->getNodeName());
                if (nodeName == "camera")
                {
                    log->__format (LOG_CCREATOR, "Found a camera.");
                    Camera * tmpCam = new Camera (cameraNode);
                    cameraList.push_back (tmpCam);
                }
            }
        }
    }
}
void Vehicle::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}

void Vehicle::stepGraphics ()
{
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

void Vehicle::setRenderDetail(int renderMode)
{
    body->setRenderDetail(renderMode);
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->setRenderDetail(renderMode);
    }
}

void Vehicle::stopGraphics ()
{
}
void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    // empty
}
dBodyID Vehicle::getVehicleID()
{
    return body->bodyID;
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->__format (LOG_DEVELOPER, "Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    
    body->setPosition ( body->getPosition() + posDiff );

    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        wheelIter->second->setPosition ( wheelIter->second->getPosition() + posDiff );
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

void Vehicle::stopPhysics ()
{
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
            wheelIter->second->setPosition (suspIter->second->getInitialWheelPosition());
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
            suspIter->second->attach(*(wheelIter->second), *this);
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
        const dReal * tmp = dBodyGetLinearVel(body->bodyID);
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(body->bodyID);
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, engine->getOutputAngularVel(), finalDrive->getInputAngularVel(), wheelMap["RearRight"]->getInputAngularVel(), wheelMap["RearLeft"]->getInputAngularVel(), gearbox->getCurrentGearLabel().c_str(), distance);
    }
}
