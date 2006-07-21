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

pWorld World::world;
std::string World::newWorld = "";

void World::setNewWorld(std::string name)
{
    world.reset();
    World::newWorld = name;
    World::get();
}
pWorld World::get()
{
    if (!world)
    {
        if (newWorld == "")
        {
            std::cout << "WARNING: Auto-creating a default world!" << std::endl;
            newWorld = "default";
        }
        new World(World::newWorld);
        pWorldObject container;
        world->setContainer(container);
    }
    return (world);
}
void World::destroy()
{
    if (!world)
    {
        std::cout << "WARNING: There's no world to be destroyed!" << std::endl;
    }
    else
    {
        world.reset();
    }
}

World::World (std::string name)
    :WorldObject("world")
{
    if (world)
    {
        world->log->__format(LOG_ERROR, "Tried to create another world, named \"%s\". This shouldn't have happened.", name.c_str());
    } else {
        pWorld tmp(this);
        world = tmp;
        setPath(Paths::world(name));
        setXmlPath(Paths::worldXml(name));
        log->loadscreen (LOG_ENDUSER, "Starting to load the world (%s)", getXmlPath().c_str());
        XmlTag * tag = new XmlTag (getXmlPath().c_str());
        processXmlRootNode (tag);
        setName(name);
        delete tag;
    }
}

World::~World ()
{
    activeCamera.reset();

    //shouldn't be necessary, but vehicle and area geoms can't be deleted after ode world so we must make sure!
    objects.clear();
    objects.clear();
    
    log->__format (LOG_DEVELOPER, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->__format (LOG_DEVELOPER, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    dWorldDestroy (ghostWorldID);
    log->__format (LOG_DEVELOPER, "Destroying ODE joints group");
    dJointGroupDestroy (jointGroupID);
}
void World::readCustomDataTag(XmlTag * tag)
{
    description = tag->getAttribute("description");

    log->__format ( LOG_DEVELOPER, "Setting ODE world gravity");
    double gravityX = stod(tag->getAttribute("gravityX"));
    double gravityY = stod(tag->getAttribute("gravityY"));
    double gravityZ = stod(tag->getAttribute("gravityZ"));
    dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);
    dWorldSetGravity (ghostWorldID, 0, 0, 0);

    bool useAreaCamera = true;    //if false, use vehicle camera
    useAreaCamera = stob(tag->getAttribute("useAreaCamera"));
    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useAreaCamera) setActiveCamera (getArea("main")->getCamera("main"));
    else               setActiveCamera (getVehicle("main")->getCamera("main"));
}

void World::processXmlRootNode (XmlTag * tag)
{
    log->__format (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");
    log->loadscreen (LOG_DEVELOPER, "Creating ODE world");
    dRandSetSeed(0);
    worldID = dWorldCreate ();
    ghostWorldID = dWorldCreate ();
    //spaceID = dHashSpaceCreate (0);
    //spaceID = dSimpleSpaceCreate (0);
    dVector3 center;
    center[0]=center[1]=center[2]=0;
    dVector3 ext;
    ext[0]=ext[1]=1000;
    ext[2]=100;
    spaceID = dQuadTreeSpaceCreate (0, center, ext, 10);
    jointGroupID = dJointGroupCreate (0);

    if (System::get()->getCfmValue() != -1)
    {
        log->__format (LOG_DEVELOPER, "Setting ODE cfm value to %f", System::get()->getCfmValue());
        dWorldSetCFM (worldID, System::get()->getCfmValue());
    }
    if (System::get()->getErpValue() != -1)
    {
        log->__format (LOG_DEVELOPER, "Setting ODE erp value to %f", System::get()->getErpValue());
        dWorldSetERP (worldID, System::get()->getErpValue());
    }
    constructFromTag(tag);
    // initialize cameras (pointing to car 0 by default)
    getArea("main")->pointCameras(getVehicle("main"));
}
void World::setActiveCamera (pCamera camera)
{
    activeCamera = camera;
    System::get()->ogreWindow->removeAllViewports ();
    System::get()->ogreWindow->addViewport (activeCamera->ogreCamera);
    log->__format (LOG_ENDUSER, "Changed camera...");
}

void World::stepGraphics()
{
    WorldObject::stepGraphics();
    if (cameraDirector == true)
    {
        setActiveCamera( getArea("main")->getClosestCamera(getVehicle("main")->getPosition()));
    }
}
void World::switchNextAreaCamera()
{
    setActiveCamera (getArea("main")->switchNextCamera());
    cameraDirector = false;
}
void World::switchNextVehicleCamera()
{
    setActiveCamera (getVehicle("main")->switchNextCamera());
    cameraDirector = false;
}
void World::switchCameraDirector()
{
    cameraDirector = !cameraDirector;
}
bool World::isActiveCamera(pCamera camera)
{
    return activeCamera == camera;
}
