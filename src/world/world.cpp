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
#include "xmlTag.hpp"
#include "area.hpp"
#include "log/logEngine.hpp"
#include "ode/ode.h"
#include "system.hpp"
#include "camera.hpp"
#include "vector3d.hpp"
#include "location.hpp"
#include "SDL.h"

World *World::worldPointer = NULL;

World *World::getWorldPointer ()
{
    if (worldPointer == 0)
    {
        std::cout << "WARNING: Auto-creating a default world!" << std::endl;
        worldPointer = new World(NULL, "default");
    }
    return (worldPointer);
}

World::World (WorldObject * container, std::string name)
    :WorldObject(container, name)
{
    if (worldPointer != 0)
    {
        delete this;
    } else {
        log->loadscreen (LOG_ENDUSER, "Starting to load the world (%s)", getXmlPath().c_str());
        worldPointer = this;
        setPath(Paths::world(name));
        setXmlPath(Paths::worldXml(name));
        log->loadscreen (LOG_ENDUSER, "Starting to load the world (%s)", getXmlPath().c_str());
        XmlTag * tag = new XmlTag (getXmlPath().c_str());
        processXmlRootNode (tag);
        delete tag;
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
        vehicleList[i] = NULL;
    }
    vehicleList.clear ();
    
    log->__format (LOG_DEVELOPER, "Unloading areas from memory...");
    size = areaList.size ();
    for (int i = 0; i < size; i++)
    {
        delete areaList[i];
        areaList[i] = 0;
    }
    areaList.clear ();
    
    log->__format (LOG_DEVELOPER, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->__format (LOG_DEVELOPER, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    dWorldDestroy (ghostWorldID);
    log->__format (LOG_DEVELOPER, "Destroying ODE joints group");
    dJointGroupDestroy (jointGroupID);
    
    worldPointer = NULL;
}


void World::processXmlRootNode (XmlTag * tag)
{
    description = "none";
    double gravityX = 0.0;
    double gravityY = 0.0;
    double gravityZ = 0.0;
    XmlTag * vehicleListTag = NULL;
    bool useAreaCamera = true;    //if false, use vehicle camera
    std::string areaDirectory = "testingGround";

    if (tag->getName() == "world")
    {
        setName(tag->getAttribute("name"));
        description = tag->getAttribute("description");
        useAreaCamera = stob(tag->getAttribute("useAreaCamera"));
        gravityX = stod(tag->getAttribute("gravityX"));
        gravityY = stod(tag->getAttribute("gravityY"));
        gravityZ = stod(tag->getAttribute("gravityZ"));
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "vehicleList")
            {
                vehicleListTag = t;
            }
            if (t->getName() == "area")
            {
                areaDirectory = t->getAttribute("directory");
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

    // load area (and its cameras)
    log->loadscreen (LOG_CCREATOR, "Creating a area");
    Area * area = new Area (this, areaDirectory);
    //area->setPosition (0.0, 0.0, 0.0); //evo2 maybe... ;)
    areaList.push_back (area);

    // load all vehicles
    log->loadscreen (LOG_CCREATOR, "Creating all vehicles");
    processXmlVehicleListNode (vehicleListTag);

    // initialize cameras (pointing to car 0 by default)
    for (unsigned int i=0; i< areaList[0]->cameraList.size(); i++)
    {
        areaList[0]->cameraList[i]->setPositionID( areaList[0]->areaBodyID );
        areaList[0]->cameraList[i]->setTarget( vehicleList[0]->getMainOdeObject());
    }
    for (unsigned int i=0; i< vehicleList[0]->cameraList.size(); i++)
    {
        vehicleList[0]->cameraList[i]->setPositionID( vehicleList[0]->getMainOdeObject()->getBodyID());
        vehicleList[0]->cameraList[i]->setTarget( vehicleList[0]->getMainOdeObject() );
    }

    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useAreaCamera)
    {
        //err... use... area camera, i guess.
        setActiveCamera (areaList[0]->cameraList[0]);
    } else {
        //don't use area camera: use vehicle camera
        setActiveCamera (vehicleList[0]->cameraList[0]);
    }
    activeAreaCamera = areaList[0]->cameraList[0];
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

int World::getActiveAreaCameraIndex()
{
    int camNumber = 0;
    while ( activeAreaCamera != areaList[0]->cameraList[camNumber] )
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

void World::processXmlVehicleListNode (XmlTag * tag)
{
    if (tag->getName() == "vehicleList")
    {
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "vehicle")
            {
                std::string vehicleDirectory = "mosp1";
                std::string vehicleStartPosition = "grid01";   //first 'grid' position
                std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
                vehicleDirectory = t->getAttribute("directory");
                driver = t->getAttribute("driver");
                vehicleStartPosition = t->getAttribute("startPosition");

                log->loadscreen (LOG_CCREATOR, "Creating a vehicle");
                Vehicle * tmpVehicle = new Vehicle (this, vehicleDirectory);
                if (driver == "user" )
                {
                    tmpVehicle->setUserDriver();
                }
                vehicleList.push_back (tmpVehicle);

                log->__format (LOG_CCREATOR, "Setting vehicle starting relative rotation");
                if (areaList[0]->vehiclePositionMap.count(vehicleStartPosition) == 0)
                {
                    log->__format(LOG_ERROR, "Vehicle start position \"%s\" hasn't been defined in the area!", vehicleStartPosition.c_str());
                }
                tmpVehicle->setPosition (Vector3d(0, 0, 0));
                tmpVehicle->applyRotation ( areaList[0]->vehiclePositionMap[vehicleStartPosition]->getRotation() );

                log->__format (LOG_CCREATOR, "Setting vehicle starting position");
                tmpVehicle->setPosition (areaList[0]->vehiclePositionMap[vehicleStartPosition]->getPosition());
            }
        }
    }
}
