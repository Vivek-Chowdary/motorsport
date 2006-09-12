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

    //create ode world
    log->__format ( LOG_DEVELOPER, "Setting ODE world gravity");
    double gravityX = stod(tag->getAttribute("gravityX"));
    double gravityY = stod(tag->getAttribute("gravityY"));
    double gravityZ = stod(tag->getAttribute("gravityZ"));
    dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);
    dWorldSetGravity (ghostWorldID, 0, 0, 0);

    //create sky box
    std::string skyMaterialName = tag->getAttribute("skyMaterialName");
    double skyDistance = stod(tag->getAttribute("skyDistance"));
    bool skyDrawFirst = stob (tag->getAttribute("skyDrawFirst"));
    log->loadscreen (LOG_CCREATOR, "Creating the area sky");
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    System::get()->ogreSceneManager->setSkyBox (true, skyMaterialName.c_str(), skyDistance, skyDrawFirst, rotationToZAxis);
    System::get()->ogreWindow->update ();
    
    //create ground plane
    double groundHeight = 0.0;
    std::string groundMaterialName = "groundMaterial";
    groundHeight = stod (tag->getAttribute("height"));

    log->__format (LOG_DEVELOPER, "Creating the ode plane");
    dCreatePlane (World::get()->spaceID, 0, 0, 1, groundHeight);

    log->__format (LOG_DEVELOPER, "Creating the ogre plane");
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3(0,0,1);
    plane.d = -groundHeight;
    Ogre::SceneManager* pOgreSceneManager = System::get()->ogreSceneManager;
    Ogre::MeshManager::getSingleton().createPlane("Ground plane", "general", plane, 1000,1000,1,1,true,1,20,20,Ogre::Vector3::UNIT_Y);
    planeEntity = pOgreSceneManager->createEntity("plane", "Ground plane");
    planeEntity->setMaterialName(groundMaterialName.c_str());
    planeNode = pOgreSceneManager->getRootSceneNode()->createChildSceneNode();
    planeNode->attachObject(planeEntity);
    System::get()->ogreWindow->update ();

    //set cameras
    bool useAreaCamera = true;    //if false, use vehicle camera
    useAreaCamera = stob(tag->getAttribute("useAreaCamera"));
    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useAreaCamera)
    {
        WorldObjectsIt i = objects.begin();
        for(;i != objects.end(); i++)
        {
            if (pArea tmp = boost::dynamic_pointer_cast<Area>(i->second))
            {
                setActiveCamera (tmp->getCamera("main"));
            }
        }
    } else {
        setActiveCamera (getVehicle("main")->getCamera("main"));
    }
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
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pArea tmp = boost::dynamic_pointer_cast<Area>(i->second))
        {
            tmp->pointCameras(getVehicle("main"));
        }
    }
}
void World::setActiveCamera (pCamera camera)
{
    if (camera)
    {
        activeCamera = camera;
        System::get()->ogreWindow->removeAllViewports ();
        System::get()->ogreWindow->addViewport (activeCamera->ogreCamera);
        log->__format (LOG_ENDUSER, "Changed camera...");
    } else {
        log->__format (LOG_WARNING, "Unable to change camera! No such camera found.");
    }
}

void World::stepGraphics()
{
    WorldObject::stepGraphics();

    // Update infinite plane position according to vehicle position
    Ogre::Vector3 areaPos (planeNode->getPosition());
    Vector3d vehiclePos (World::get()->getVehicle("main")->getPosition());
    Vector3d diff (areaPos.x - vehiclePos.x, areaPos.y - vehiclePos.y, areaPos.z - vehiclePos.z);
    const double tile = 1000.0 / 20.0;
    if (diff.x > tile || diff.x < -tile) areaPos.x -= int ((diff.x) / (tile)) * (tile);
    if (diff.y > tile || diff.y < -tile) areaPos.y -= int ((diff.y) / (tile)) * (tile);
    planeNode->setPosition(areaPos);
    
    if (cameraDirector == true)
    {
        WorldObjectsIt i = objects.begin();
        for(;i != objects.end(); i++)
        {
            if (pArea tmp = boost::dynamic_pointer_cast<Area>(i->second))
            {
                setActiveCamera( tmp->getClosestCamera(getVehicle("main")->getPosition()));
            }
        }
    }
}
void World::switchNextAreaCamera()
{
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pArea tmp = boost::dynamic_pointer_cast<Area>(i->second))
        {
            setActiveCamera (tmp->switchNextCamera());
        }
    }
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
