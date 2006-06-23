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
    activeCamera.reset();
    activeAreaCamera.reset();
    activeVehicleCamera.reset();

    //shouldn't be necessary, but vehicle and area geoms can't be deleted after ode world so we must make sure!
    vehicles.clear();
    areas.clear();
    
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
    XmlTag * vehiclesTag = NULL;
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
                vehiclesTag = t;
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
    pArea area = Area::create (this, areaDirectory);
    //area->setPosition (0.0, 0.0, 0.0); //evo2 maybe... ;)
    areas[area->getName()] = area;

    // load all vehicles
    log->loadscreen (LOG_CCREATOR, "Creating all vehicles");
    processXmlVehicleListNode (vehiclesTag);

    // initialize cameras (pointing to car 0 by default)
    CamerasIt i = areas.begin()->second->cameras.begin();
    for(;i != areas.begin()->second->cameras.end(); i++)
    {
        i->second->setPositionID( areas.begin()->second->areaBodyID );
        i->second->setTarget( vehicles.begin()->second->getMainOdeObject());
    }
    i = vehicles.begin()->second->cameras.begin();
    for(;i != vehicles.begin()->second->cameras.end(); i++)
    {
        i->second->setPositionID( vehicles.begin()->second->getMainOdeObject()->getBodyID());
        i->second->setTarget( vehicles.begin()->second->getMainOdeObject() );
    }

    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useAreaCamera)
    {
        //err... use... area camera, i guess.
        setActiveCamera (areas.begin()->second->cameras.begin()->second);
    } else {
        //don't use area camera: use vehicle camera
        setActiveCamera (vehicles.begin()->second->cameras.begin()->second);
    }
    activeAreaCamera = areas.begin()->second->cameras.begin()->second;
    activeVehicleCamera = vehicles.begin()->second->cameras.begin()->second;
}

void World::setActiveCamera (pCamera camera)
{
    activeCamera = camera;
    SystemData::getSystemDataPointer()->ogreWindow->removeAllViewports ();
    SystemData::getSystemDataPointer()->ogreWindow->addViewport (activeCamera->ogreCamera);
    log->__format (LOG_ENDUSER, "Changed camera...");
}

pCamera World::getActiveCamera (void)
{
    return activeCamera;
}

pCamera World::getActiveAreaCamera()
{
    return areas.begin()->second->cameras[activeAreaCamera->getName()];
}

pCamera World::getActiveVehicleCamera()
{
    return vehicles.begin()->second->cameras[activeVehicleCamera->getName()];
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
                pVehicle tmpVehicle  = Vehicle::create(this, vehicleDirectory);
                if (driver == "user" )
                {
                    tmpVehicle->setUserDriver();
                }
                vehicles[tmpVehicle->getName()] = tmpVehicle;

                log->__format (LOG_CCREATOR, "Setting vehicle starting relative rotation");
                if (areas.begin()->second->vehiclePositionMap.count(vehicleStartPosition) == 0)
                {
                    log->__format(LOG_ERROR, "Vehicle start position \"%s\" hasn't been defined in the area!", vehicleStartPosition.c_str());
                }
                tmpVehicle->setPosition (Vector3d(0, 0, 0));
                tmpVehicle->applyRotation ( areas.begin()->second->vehiclePositionMap[vehicleStartPosition]->getRotation() );

                log->__format (LOG_CCREATOR, "Setting vehicle starting position");
                tmpVehicle->setPosition (areas.begin()->second->vehiclePositionMap[vehicleStartPosition]->getPosition());
            }
        }
    }
}
