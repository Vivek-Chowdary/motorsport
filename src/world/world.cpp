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

//pWorld World::world = NULL;
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
    vehicles.clear();
    areas.clear();
    
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
    pLocation location;

    std::string type;
    std::string name;
    std::string locname;
    bool error = false;
    std::string errorMessage = "";

    if (fullname.find_first_of('(') == 0)
    {
        //(
        unsigned int typeend;
        if ((typeend = fullname.find_first_of(')',0)) != std::string::npos)
        {
            //(type)
            type = fullname.substr(1,typeend-1);
            unsigned int nameend;
            if ((nameend = fullname.find_first_of('/')) != std::string::npos)
            {
                //(type)name/
                name = fullname.substr(typeend+1,nameend-(typeend+1));
                locname = fullname.substr(nameend+1);
            } else {
                //(type)name
                error = true;
                errorMessage = "'/' not found";
            }
        } else {
            //(type
            error = true;
            errorMessage = "')' not found";
        }
    } else {
        //name
        log->__format (LOG_DEVELOPER, "Not found '('!");
    }
    if (error)
    {
        log->__format (LOG_ERROR, "XML mosp object location syntax error \"%s\": %s", fullname.c_str(), errorMessage.c_str());
    } else {
        log->__format (LOG_DEVELOPER, "Type: '%s', Name: '%s'. LocationName: '%s'", type.c_str(), name.c_str(), locname.c_str());
        if (type == "area")
        {
            pArea tmp = getArea(name);
            location = tmp->getLocation(locname);
        }
    }
    return location;
}
pVehicle World::getVehicleObject(std::string fullname)
{
    pVehicle vehicle;

    std::string type;
    std::string name;
    bool error = false;
    std::string errorMessage = "";

    if (fullname.find_first_of('(') == 0)
    {
        //(
        unsigned int typeend;
        if ((typeend = fullname.find_first_of(')',0)) != std::string::npos)
        {
            //(type)
            type = fullname.substr(1,typeend-1);
            unsigned int nameend;
            if ((nameend = fullname.find_first_of('/')) != std::string::npos)
            {
                //(type)name/
                name = fullname.substr(typeend+1,nameend-(typeend+1));
            } else {
                //(type)name
                name = fullname.substr(typeend+1);
            }
        } else {
            //(type
            error = true;
            errorMessage = "')' not found";
        }
    } else {
        //name
        log->__format (LOG_DEVELOPER, "Not found '('!");
    }
    if (error)
    {
        log->__format (LOG_ERROR, "XML mosp object location syntax error \"%s\": %s", fullname.c_str(), errorMessage.c_str());
    } else {
        log->__format (LOG_DEVELOPER, "Type: '%s', Name: '%s'.", type.c_str(), name.c_str());
        if (type == "vehicle")
        {
            vehicle = getVehicle(name);
        }
    }
    return vehicle;
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
                        vehicles[tmp->getName()] = tmp;
                    }
                    if (o->getName() == "area")
                    {
                        log->loadscreen (LOG_CCREATOR, "Creating an area");
                        std::string model = o->getAttribute("model");
                        pArea tmp = Area::create (model);
                        tmp->setName(sobjname);
                        areas[tmp->getName()] = tmp;
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
                log->__format (LOG_CCREATOR, "Setting vehicle ver");
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

    AreasIt i = areas.begin();
    for(;i != areas.end(); i++)
    {
        i->second->setContainer(shared_from_this());
    }
    VehiclesIt v = vehicles.begin();
    for(;v != vehicles.end(); v++)
    {
        v->second->setContainer(shared_from_this());
    }
}

pArea World::getArea(std::string name)
{
    pArea tmp;
    for (AreasIt i = areas.begin(); i != areas.end(); i++)
    {
        if (i->first == ("(Area)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Area>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Area");
    return tmp;
}

pVehicle World::getVehicle(std::string name)
{
    pVehicle tmp;
    for (VehiclesIt i = vehicles.begin(); i != vehicles.end(); i++)
    {
        if (i->first == ("(Vehicle)" + name) && i->second) if (tmp = boost::dynamic_pointer_cast<Vehicle>(i->second)) break;
    }
    if (tmp == NULL) log->__format(LOG_ERROR, "Tried to access non-existent world object \"%s\" using type \"%s\"", name.c_str(), "Vehicle");
    return tmp;
}

void World::stepGraphics()
{
    base->stepGraphics();

    // Update Ogre's vehicles positions with Ode's positions.
    VehiclesIt j = vehicles.begin();
    for (; j != vehicles.end(); j++)
    {
        j->second->stepGraphics();
    }
    AreasIt i = areas.begin();
    for (; i != areas.end(); i++)
    {
        i->second->stepGraphics();
    }
    if (cameraDirector == true)
    {
        setActiveCamera( getArea("main")->getClosestCamera(getVehicle("main")->getPosition()));
    }
}
void World::stepPhysics()
{
    // Update Ogre's vehicles positions with Ode's positions.
    VehiclesIt j = vehicles.begin();
    for (; j != vehicles.end(); j++)
    {
        j->second->stepPhysics();
    }
    AreasIt i = areas.begin();
    for (; i != areas.end(); i++)
    {
        i->second->stepPhysics();
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
