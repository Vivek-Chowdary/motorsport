/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
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
#include "camera.hpp"
#include "system.hpp"
#include "SDL.h"


int Vehicle::instancesCount = 0;

Vehicle::Vehicle (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_DEVELOPER, "VEH");
    SystemData::getSystemDataPointer()->tmpPath = xmlFilename;
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/vehicles/");
    file.append(xmlFilename);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(file, "FileSystem", "vehicles - " + xmlFilename);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(file + "/skybox.zip", "Zip", "vehicles - " + xmlFilename);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("vehicles - " + xmlFilename);
    file.append("/vehicle.xml");
    log->loadscreen (LOG_ENDUSER, "Starting to load a vehicle (%s)", file.c_str());
    Uint32 time = SDL_GetTicks();
    XmlFile * xmlFile = new XmlFile (file.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
    log->loadscreen (LOG_ENDUSER, "Finished loading a vehicle (%s). %f seconds.", file.c_str(), (SDL_GetTicks() - time) / 1000.0);

    userDriver = false;
    instancesCount++;
}

Vehicle::~Vehicle ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

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
    delete log;
}

void Vehicle::setUserDriver ()
{
    userDriver = true;
    log->log (LOG_ENDUSER, LOG_TELEMETRY, "VehSpeed EngineSpeed DiffAngularVel RRWhAngulVel RLWhAngulVel Gear Distance");

    // engage neutral gear
    gearbox->setGear(1);

    // spread the news to the necessary (input-able) vehicle parts
    engine->setUserDriver();
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

    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "vehicle")
            {
                log->put (LOG_CCREATOR, "Found a vehicle.");
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
                            log->format (LOG_CCREATOR, "Found the position: %s", attribute.c_str());
                            Vector3d position;
                            position = stov3d (attribute);
                        }
                        if (attribute == "name")
                        {
                            assignXmlString (name, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the name: %s", name.c_str());
                        }
                        if (attribute == "revision")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the revision number: %s", attribute.c_str());
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
                            log->format (LOG_CCREATOR, "Found the author contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (license, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the license: %s", license.c_str());
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
                                log->put (LOG_CCREATOR, "Found a body.");
                                bodyNode = n;
                            }
                            if (nodeName == "engine")
                            {
                                log->put (LOG_CCREATOR, "Found an engine.");
                                engineNode = n;
                            }
                            if (nodeName == "clutch")
                            {
                                log->put (LOG_CCREATOR, "Found a clutch.");
                                clutchNode = n;
                            }
                            if (nodeName == "gearbox")
                            {
                                log->put (LOG_CCREATOR, "Found a gearbox.");
                                gearboxNode = n;
                            }
                            if (nodeName == "diff")
                            {
                                log->put (LOG_CCREATOR, "Found a diff.");
                                finalDriveNode = n;
                            }
                            if (nodeName == "wheelList")
                            {
                                log->put (LOG_CCREATOR, "Found a wheel list.");
                                wheelListNode = n;
                            }
                            if (nodeName == "suspensionList")
                            {
                                log->put (LOG_CCREATOR, "Found a suspension list.");
                                suspListNode = n;
                            }
                            if (nodeName == "cameraList")
                            {
                                log->put (LOG_CCREATOR, "Found a camera list.");
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
    body = new Body (bodyNode);
    engine = new Engine (engineNode);
    clutch = new Clutch (clutchNode);
    gearbox = new Gearbox (gearboxNode);
    finalDrive = new FinalDrive (finalDriveNode);
    transfer = new Gear ();
    rearDiff = new LSD ();
    
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

    clutch->enable();
    transfer->enable();
    rearDiff->enable();
    
    //start in 1st gear (hardcoded to 2)
    // this helps brake the vehicle (since it's not user-controlled by default and would roll around freely otherwise)
    gearbox->setGear(2);

    startPhysics(NULL);
    stepGraphics();
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
                    log->put (LOG_CCREATOR, "Found a wheel.");
                    Wheel * tmpWheel = new Wheel (wheelNode);
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
                    log->put (LOG_CCREATOR, "Found a suspension.");
                    Suspension * tmpSusp = new Suspension (suspNode);
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
                    log->put (LOG_CCREATOR, "Found a camera.");
                    Camera * tmpCam = new Camera (cameraNode);
                    cameraList.push_back (tmpCam);
                }
            }
        }
    }
}
