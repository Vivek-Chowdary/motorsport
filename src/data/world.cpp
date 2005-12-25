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
#include "tools/xmlParser.hpp"
#include "track.hpp"
#include "log/logEngine.hpp"
#include "ode/ode.h"
#include "system.hpp"
#include "camera.hpp"
#include "vector3d.hpp"
#include "data/location.hpp"
#include "SDL.h"

World *World::worldPointer = NULL;

World *World::getWorldPointer ()
{
    if (worldPointer == 0)
    {
        std::cout << "WARNING: Auto-creating a default world!" << std::endl;
        worldPointer = new World("default.xml");
    }
    return (worldPointer);
}

World::World (char * xmlFilename)
{
    if (worldPointer != 0)
    {
        delete this;
    } else {
        log = new LogEngine(LOG_DEVELOPER, "WRL");
        worldPointer = this;
        std::string file = SystemData::getSystemDataPointer()->dataDir;
        file.append("/worlds/");
        file.append(xmlFilename);
        log->loadscreen (LOG_ENDUSER, "Starting to load the world (%s)", file.c_str());
        double time = SDL_GetTicks()/1000.0;
        XmlFile* xmlFile = new XmlFile (file.c_str());
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;
        log->loadscreen (LOG_ENDUSER, "Finished loading the world (%s). %f seconds.", file.c_str(), SDL_GetTicks()/1000.0 - time);
    }
}

World::~World ()
{
    // unload the bodies from memory
    log->__format (LOG_DEVELOPER, "Unloading vehicles from memory...");
    int size = vehicleList.size ();
    for (int i = 0; i < size; i++)
    {
        delete vehicleList[i];
    }
    vehicleList.clear ();
    
    log->__format (LOG_DEVELOPER, "Unloading tracks from memory...");
    size = trackList.size ();
    for (int i = 0; i < size; i++)
    {
        delete trackList[i];
    }
    trackList.clear ();
    
    log->__format (LOG_DEVELOPER, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->__format (LOG_DEVELOPER, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    dWorldDestroy (ghostWorldID);
    log->__format (LOG_DEVELOPER, "Destroying ODE joints group");
    dJointGroupDestroy (jointGroupID);
    
    worldPointer = NULL;
    delete log;
}


void World::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    name.assign ("None");
    description.assign ("None");
    double gravityX = 0.0;
    double gravityY = 0.0;
    double gravityZ = 0.0;
    DOMNode * vehicleListNode = NULL;
    bool useTrackCamera = true;    //if false, use vehicle camera
    std::string trackDirectory = "testingGround";
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            if (name == "world")
            {
                log->__format (LOG_CCREATOR, "Found a world.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "name")
                        {
                            assignXmlString (name, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the world name: %s", name.c_str());
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (description, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the world description: %s", description.c_str());
                        }
                        if (attribute == "useTrackCamera")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            useTrackCamera = stob (attribute);
                            log->__format (LOG_CCREATOR, "Found the selected world camera: %s camera", attribute.c_str());
                        }
                        if (attribute == "gravityX")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            gravityX = stod (attribute);
                            log->__format (LOG_CCREATOR, "Found the world gravity X module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityY")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            gravityY = stod (attribute);
                            log->__format (LOG_CCREATOR, "Found the world gravity Y module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityZ")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            gravityZ = stod (attribute);
                            log->__format (LOG_CCREATOR, "Found the world gravity Z module: %s m/s^2", attribute.c_str());
                        }
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            assignXmlString (name, n->getNodeName());
                            if (name == "vehicleList")
                            {
                                log->__format (LOG_CCREATOR, "Found the vehicle list");
                                vehicleListNode = n;
                            }
                            if (name == "track")
                            {
                                log->__format (LOG_CCREATOR, "Found a track");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "directory")
                                        {
                                            assignXmlString (trackDirectory, attNode->getValue());
                                            log->__format (LOG_CCREATOR, "Found the track directory: %s", trackDirectory.c_str());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    log->__format (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");
    log->loadscreen (LOG_DEVELOPER, "Creating ODE world");
    dRandSetSeed(0);
    worldID = dWorldCreate ();
    ghostWorldID = dWorldCreate ();
    spaceID = dHashSpaceCreate (0);
    jointGroupID = dJointGroupCreate (0);
    
    if (SystemData::getSystemDataPointer()->getCfmValue() != -1)
    {
        log->__format (LOG_DEVELOPER, "Setting ODE cfm value to %f", SystemData::getSystemDataPointer()->getCfmValue());
        dWorldSetCFM (worldID, SystemData::getSystemDataPointer()->getCfmValue());
    }
    if (SystemData::getSystemDataPointer()->getErpValue() != -1)
    {
        log->__format (LOG_DEVELOPER, "Setting ODE erp value to %f", SystemData::getSystemDataPointer()->getErpValue());
        dWorldSetERP (worldID, SystemData::getSystemDataPointer()->getErpValue());
    }

    log->__format ( LOG_DEVELOPER, "Setting ODE world gravity");
    dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);
    dWorldSetGravity (ghostWorldID, 0, 0, 0);

    // load track (and its cameras)
    log->loadscreen (LOG_CCREATOR, "Creating a track");
    Track * track = new Track (trackDirectory);
    //track->setPosition (0.0, 0.0, 0.0); //evo2 maybe... ;)
    trackList.push_back (track);

    // load all vehicles
    log->loadscreen (LOG_CCREATOR, "Creating all vehicles");
    processXmlVehicleListNode (vehicleListNode);

    // initialize cameras (pointing to car 0 by default)
    for (unsigned int i=0; i< trackList[0]->cameraList.size(); i++)
    {
        trackList[0]->cameraList[i]->setPositionID( trackList[0]->trackBodyID );
        trackList[0]->cameraList[i]->setTargetID( vehicleList[0]->getVehicleID() );
    }
    for (unsigned int i=0; i< vehicleList[0]->cameraList.size(); i++)
    {
        vehicleList[0]->cameraList[i]->setPositionID( vehicleList[0]->getVehicleID() );
        vehicleList[0]->cameraList[i]->setTargetID( vehicleList[0]->getVehicleID() );
    }

    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useTrackCamera)
    {
        //err... use... track camera, i guess.
        setActiveCamera (trackList[0]->cameraList[0]);
    } else {
        //don't use track camera: use vehicle camera
        setActiveCamera (vehicleList[0]->cameraList[0]);
    }
    activeTrackCamera = trackList[0]->cameraList[0];
    activeVehicleCamera = vehicleList[0]->cameraList[0];
}

void World::setActiveCamera (Camera * camera)
{
    activeCamera = camera;
    SystemData::getSystemDataPointer()->ogreWindow->removeAllViewports ();
    SystemData::getSystemDataPointer()->ogreWindow->addViewport (activeCamera->ogreCamera);
    log->__format (LOG_ENDUSER, "Changed camera...");
}

Camera * World::getActiveCamera (void)
{
    return activeCamera;
}

int World::getActiveTrackCameraIndex()
{
    int camNumber = 0;
    while ( activeTrackCamera != trackList[0]->cameraList[camNumber] )
    {
        camNumber++;
    }
    return camNumber;
}

int World::getActiveVehicleCameraIndex()
{
    int camNumber = 0;
    while ( activeVehicleCamera != vehicleList[0]->cameraList[camNumber] )
    {
        camNumber++;
    }
    return camNumber;
}

void World::processXmlVehicleListNode (DOMNode * vehicleListNode)
{
    if (vehicleListNode != 0)
    {
        log->__format (LOG_CCREATOR, "Processing vehicle list");
        DOMNode * vNode;
        for (vNode = vehicleListNode->getFirstChild (); vNode != 0; vNode = vNode->getNextSibling ())
        {
            std::string name;
            assignXmlString (name, vNode->getNodeName());
            int nVehicles = 0;
            if (name == "vehicle")
            {
                nVehicles++;
                log->__format (LOG_CCREATOR, "Found vehicle #%i", nVehicles);
                std::string vehicleDirectory = "mosp1";
                std::string vehicleStartPosition = "grid01";   //first 'grid' position
                std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
                if (vNode->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = vNode->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "directory")
                        {
                            assignXmlString (vehicleDirectory, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the vehicle directory: %s", vehicleDirectory.c_str());
                        }
                        if (attribute == "driver")
                        {
                            assignXmlString (driver, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the vehicle driver: %s", driver.c_str());
                        }
                        if (attribute == "startPosition")
                        {
                            assignXmlString (vehicleStartPosition, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the vehicle start position index: %s", vehicleStartPosition.c_str());
                        }
                    }
                }
                log->loadscreen (LOG_CCREATOR, "Creating a vehicle");
                Vehicle * tmpVehicle = new Vehicle (vehicleDirectory);
                if (driver == "user" )
                {
                    tmpVehicle->setUserDriver();
                }
                vehicleList.push_back (tmpVehicle);

                log->__format (LOG_CCREATOR, "Setting vehicle starting relative rotation");
                if (trackList[0]->vehiclePositionMap.count(vehicleStartPosition) == 0)
                {
                    log->__format(LOG_ERROR, "Vehicle start position \"%s\" hasn't been defined in the track!", vehicleStartPosition.c_str());
                }
                tmpVehicle->setPosition (Vector3d(0, 0, 0));
                tmpVehicle->applyRotation ( trackList[0]->vehiclePositionMap[vehicleStartPosition]->getRotation() );

                log->__format (LOG_CCREATOR, "Setting vehicle starting position");
                tmpVehicle->setPosition (trackList[0]->vehiclePositionMap[vehicleStartPosition]->getPosition());
            }
        }
    }
}
