
/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#include "world.hpp"
#include "vehicle.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "track.hpp"
#include "log/logEngine.hpp"
#include "ode/ode.h"
#include "system.hpp"
#include "camera.hpp"
#include "vector3d.hpp"
#include "data/vehiclePosition.hpp"

World *World::worldPointer = NULL;

World *World::getWorldPointer ()
{
    if (worldPointer == 0)
    {
        std::cout << "WARNING: Auto-creating a default world!" << std::endl;
        worldPointer = new World("../data/worlds/default.xml");
    }
    return (worldPointer);
}

World::World (char * xmlFilename)
{
    if (worldPointer != 0)
    {
        delete this;
    } else {
        log = new LogEngine(LOG_TRACE, "WRL");
        worldPointer = this;

        log->format(LOG_INFO,"Reading %s world file", xmlFilename);
        XmlFile* xmlFile = new XmlFile (xmlFilename);
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;
    }
}

World::~World ()
{
    // unload the bodies from memory
    log->put (LOG_INFO, "Unloading vehicles from memory...");
    int size = vehicleList.size ();
    for (int i = 0; i < size; i++)
    {
        delete vehicleList[i];
    }
    vehicleList.clear ();
    
    log->put (LOG_INFO, "Unloading tracks from memory...");
    size = trackList.size ();
    for (int i = 0; i < size; i++)
    {
        delete trackList[i];
    }
    trackList.clear ();
    
    log->put (LOG_INFO, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->put (LOG_INFO, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    dWorldDestroy (ghostWorldID);
    log->put (LOG_INFO, "Destroying ODE joints group");
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
    bool useTrackCamera = true;    //if false, use vehicle camera
    std::string vehicleDirectory = "testCar";
    std::string vehicleStartPosition = "0";   //first 'grid' position
    std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
    std::string trackDirectory = "testingGround";
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            if (name == "world")
            {
                log->put (LOG_INFO, "Found a world.");
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
                            name.clear();
                            assignXmlString (name, attNode->getValue());
                            log->format (LOG_INFO, "Found the world name: %s", name.c_str());
                        }
                        if (attribute == "description")
                        {
                            description.clear();
                            assignXmlString (description, attNode->getValue());
                            log->format (LOG_INFO, "Found the world description: %s", description.c_str());
                        }
                        if (attribute == "useTrackCamera")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            useTrackCamera = stob (attribute);
                            log->format (LOG_INFO, "Found the selected world camera: %s camera", attribute.c_str());
                        }
                        if (attribute == "gravityX")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityX = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity X module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityY")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityY = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity Y module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityZ")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityZ = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity Z module: %s m/s^2", attribute.c_str());
                        }
                        attribute.clear();
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            name.clear();
                            assignXmlString (name, n->getNodeName());
                            if (name == "vehicle")
                            {
                                log->put (LOG_INFO, "Found a vehicle");
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
                                            vehicleDirectory.clear();
                                            assignXmlString (vehicleDirectory, attNode->getValue());
                                            log->format (LOG_INFO, "Found the vehicle directory: %s", vehicleDirectory.c_str());
                                        }
                                        if (attribute == "driver")
                                        {
                                            driver.clear();
                                            assignXmlString (driver, attNode->getValue());
                                            log->format (LOG_INFO, "Found the vehicle driver: %s", driver.c_str());
                                        }
                                        if (attribute == "startPosition")
                                        {
                                            vehicleStartPosition.clear();
                                            assignXmlString (vehicleStartPosition, attNode->getValue());
                                            log->format (LOG_INFO, "Found the vehicle start position index: %s", vehicleStartPosition.c_str());
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                            if (name == "track")
                            {
                                log->put (LOG_INFO, "Found a track");
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
                                            trackDirectory.clear();
                                            assignXmlString (trackDirectory, attNode->getValue());
                                            log->format (LOG_INFO, "Found the track directory: %s", trackDirectory.c_str());
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            name.clear();
        }
    }
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

/* /////////////////////////////////// STILL THOSE 2 VARS TO USE TODO
    bool useTrackCamera = true;    //if false, use vehicle camera
    std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
/////////////////////////////////// */

    log->put (LOG_INFO, "Creating ODE world");
    dRandSetSeed(0);
    worldID = dWorldCreate ();
    ghostWorldID = dWorldCreate ();
    spaceID = dHashSpaceCreate (0);
    jointGroupID = dJointGroupCreate (0);

    if (double cfmValue = SystemData::getSystemDataPointer()->getCfmValue() != -1)
    {
        log->put (LOG_INFO, "Setting ODE cfm value");
        dWorldSetCFM (worldID, cfmValue);
    }
    if (double erpValue = SystemData::getSystemDataPointer()->getErpValue() != -1)
    {
        log->put (LOG_INFO, "Setting ODE erp value");
        dWorldSetERP (worldID, erpValue);
    }

    log->put ( LOG_INFO, "Setting ODE world gravity");
    dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);
    dWorldSetGravity (ghostWorldID, 0, 0, 0);

    // load track (and its cameras)
    std::string tmpPath = ("../data/tracks/");
    tmpPath.append (trackDirectory);
    tmpPath.append ("/track.xml");
    Track * track = new Track (tmpPath);
    //track->setPosition (0.0, 0.0, 0.0); //evo2 maybe... ;)
    trackList.push_back (track);

    // load vehicle (and its cameras)
    tmpPath = ("../data/vehicles/");
    tmpPath.append (vehicleDirectory);
    tmpPath.append ("/vehicle.xml");

    log->put (LOG_INFO, "Creating a vehicle");
    Vehicle * vehicle = new Vehicle (tmpPath);
    vehicleList.push_back (vehicle);
    
    log->put (LOG_INFO, "Attaching vehicle wheels to its body (via suspensions)");
    vehicle->attachWheelsToBody();
    
    log->put (LOG_INFO, "Setting vehicle starting position");
    vehicle->setPosition (track->vehiclePositionMap[vehicleStartPosition]->getPosition());
    
    log->put (LOG_INFO, "Setting vehicle starting rotation");
    Vector3d rotation = track->vehiclePositionMap[vehicleStartPosition]->getRotation();
    vehicle->setRotation (rotation);

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
    log->put (LOG_INFO, "Setting camera viewport");
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

    // Clean up things, leave others in memory (world properties).
    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    vehicleDirectory.clear();
    driver.clear();
    trackDirectory.clear();
}

void World::setActiveCamera (Camera * camera)
{
    activeCamera = camera;
    SystemData::getSystemDataPointer()->ogreWindow->removeAllViewports ();
    SystemData::getSystemDataPointer()->ogreWindow->addViewport (activeCamera->ogreCamera);
    log->put (LOG_VERBOSE, "Changed camera...");
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
