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
#include "mospPath.hpp"
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
        world->setContainer();
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
    :WorldObject(name)
{
    if (world)
    {
        world->log->__format(LOG_ERROR, "Tried to create another world, named \"%s\". This shouldn't have happened.", name.c_str());
    } else {
        pWorld tmp(this);
        world = tmp;
        setPath(Paths::world(name));
        setName(name);
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


pLocation World::getLocationObject(std::string fullname)
{
    pLocation tmp;
    if (MospPath::getType(fullname) == "area")
    {
        pArea a = getArea(MospPath::getName(fullname));
        log->__format(LOG_DEVELOPER, "Area name=%s", a->getName().c_str());
        tmp = a->getLocation(MospPath::getName(MospPath::getSubFullname(fullname)));
    }
    log->__format(LOG_DEVELOPER, "Location name=%s", tmp->getName().c_str());
    return tmp;
}
pVehicle World::getVehicleObject(std::string fullname)
{
    pVehicle tmp;
    if ((tmp = boost::dynamic_pointer_cast<Vehicle>(getFirstObject(fullname))) == NULL)
        log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", fullname.c_str(), "Vehicle");
    log->__format(LOG_DEVELOPER, "Vehicle name=%s", tmp->getName().c_str());
    return tmp;
}
pWorldObject World::getFirstObject(std::string fullname)
{
    std::string type = MospPath::getType(fullname);
    std::string name = MospPath::getName(fullname);
    pWorldObject tmp = getObject("(" + type + ")" + name);
    log->__format(LOG_DEVELOPER, "Object name=%s", tmp->getName().c_str());
    return tmp;
}
pWorldObject World::getObject (std::string name)
{
   if (objects.find(name) == objects.end())
   log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" from generic type \"%s\"", name.c_str(), "WorldObject");
   return objects[name];
}

void World::processXmlRootNode (XmlTag * tag)
{
    log->__format (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");
    log->loadscreen (LOG_DEVELOPER, "Creating ODE world");
    dRandSetSeed(0);
    worldID = dWorldCreate ();
    ghostWorldID = dWorldCreate ();
    spaceID = dHashSpaceCreate (0);
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

    bool useAreaCamera = true;    //if false, use vehicle camera
    if (tag->getName() == "world")
    {
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "customData")
            {
                description = t->getAttribute("description");
                useAreaCamera = stob(t->getAttribute("useAreaCamera"));

                double gravityX = stod(t->getAttribute("gravityX"));
                double gravityY = stod(t->getAttribute("gravityY"));
                double gravityZ = stod(t->getAttribute("gravityZ"));
                log->__format ( LOG_DEVELOPER, "Setting ODE world gravity");
                dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);
                dWorldSetGravity (ghostWorldID, 0, 0, 0);
            }
            if (t->getName() == "sharedobject")
            {
                std::string sobjname = t->getAttribute("name");
                XmlTag * o = t->getTag(0); for (int i = 0; i < t->nTags(); o = t->getTag(++i))
                {
                    if (o->getName() == "vehicle")
                    {
                        log->loadscreen (LOG_CCREATOR, "Creating a vehicle");
                        std::string model = o->getAttribute("model");
                        pVehicle tmp= Vehicle::create(model);
                        tmp->setName(sobjname);
                        objects[tmp->getName()] = tmp;
                    }
                    if (o->getName() == "area")
                    {
                        log->loadscreen (LOG_CCREATOR, "Creating an area");
                        std::string model = o->getAttribute("model");
                        pArea tmp = Area::create (model);
                        tmp->setName(sobjname);
                        objects[tmp->getName()] = tmp;
                    }
                }
            }
            if (t->getName() == "location-vehicle")
            {
                log->__format (LOG_CCREATOR, "Setting vehicle location");
                std::string f= t->getAttribute("first");
                std::string s= t->getAttribute("second");
                pLocation first = getLocationObject(f);
                pVehicle second = getVehicleObject(s);

                second->setPosition(Vector3d(0, 0, 0));
                second->applyRotation(first->getRotation());
                second->setPosition(first->getPosition());
                log->__format(LOG_DEVELOPER, "Rotation: x%f, y%f, z%f, w%f. Rotation: %f, %f, %f", first->getRotation().x,first->getRotation().y,first->getRotation().z,first->getRotation().w,first->getPosition().x,first->getPosition().y,first->getPosition().z);
            }
            if (t->getName() == "vehicle-driver")
            {
                log->__format (LOG_CCREATOR, "Setting vehicle driver");
                std::string f= t->getAttribute("first");
                std::string s= t->getAttribute("second");
                pVehicle tmp = getVehicleObject(f);
                if (s == "user" ) tmp->setUserDriver();
            }
        }
    }
    // initialize cameras (pointing to car 0 by default)
    getArea("main")->pointCameras(getVehicle("main"));

    // set active camera
    log->loadscreen (LOG_DEVELOPER, "Setting camera viewport");
    if (useAreaCamera)
    {
        //err... use... area camera, i guess.
        setActiveCamera (getArea("main")->getCamera("main"));
    } else {
        //don't use area camera: use vehicle camera
        setActiveCamera (getVehicle("main")->getCamera("main"));
    }
}

void World::setActiveCamera (pCamera camera)
{
    activeCamera = camera;
    System::get()->ogreWindow->removeAllViewports ();
    System::get()->ogreWindow->addViewport (activeCamera->ogreCamera);
    log->__format (LOG_ENDUSER, "Changed camera...");
}

void World::setContainer()
{
    //NOTE: world is hardcoded not to have a parent, therefore we don't run this line:
    //WorldObject::setContainer(container);
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        i->second->setContainer(shared_from_this());
    }
}

void World::stepGraphics()
{
    base->stepGraphics();
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
