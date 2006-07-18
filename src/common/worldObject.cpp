/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "worldObject.hpp"
#include "quaternion.hpp"
#include "vector3d.hpp"

unsigned int WorldObject::instancesCount = 0;
WorldObjectsC WorldObject::worldObjects;

WorldObject::WorldObject (const std::string & name)
{
    this->base = dynamic_cast<WorldObject *>(this);
    this->name = name;
    this->path = "/unknownPath/";
    this->xmlPath = path + "unknownFilepath";

    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;

    //FIXME what to use, id# or name?
    log = LogEngine::create (LOG_DEVELOPER, this->getFullName());
    log->__format(LOG_CCREATOR, "New world object! Id #%s.", id.c_str());
    worldObjects[this->id] = this;
}
void WorldObject::setContainer(pWorldObject container)
{
    std::string oldName = getFullName();
    this->container = container;
    this->path = container->getPath();
    this->xmlPath = container->getPath();
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        dBodySetData (j->second->getBodyID(), (void*) container.get());
    }
    log->setName(this->getFullName());
    log->__format(LOG_CCREATOR, "New container! Old full name: %s. New full name: %s", id.c_str(), oldName.c_str(), getFullName().c_str());

}

WorldObject::~WorldObject ()
{
    log->__format(LOG_CCREATOR, "Deleting myself. Id #%s.", id.c_str());
    container.reset();
    worldObjects.erase(this->id);
    base = NULL;
}
void WorldObject::logAll()
{
    pLogEngine log (LogEngine::create(LOG_DEVELOPER, "WorldObjects"));
    WorldObjectsCIt i = worldObjects.begin();
    for(;i != worldObjects.end(); i++)
    {
        if (i->second != NULL)
        log->__format(LOG_DEVELOPER, "WorldObject id#%s.\t Full name: %s", i->second->getId().c_str(), i->second->getFullName().c_str());
    }
}
std::string WorldObject::getId()
{
    return id;
}
std::string WorldObject::getName()
{
    return name;
}
void WorldObject::setName(std::string name)
{
    this->name = name;
    log->setName(getFullName());
}
std::string WorldObject::getFullName()
{
    pWorldObject pcontainer = container.lock();
    if (pcontainer == NULL) return name;
    return pcontainer->getFullName() + "." + name;
}
std::string WorldObject::getPath()
{
    if (path == "/unknownPath/") return System::get()->getCurrentPath();
    return path;
}
void WorldObject::setPath(std::string path)
{
    this->path = path;
}
std::string WorldObject::getXmlPath()
{
    return xmlPath;
}
void WorldObject::setXmlPath(std::string xmlPath)
{
    this->xmlPath = xmlPath;
}
pLogEngine WorldObject::getLog()
{
    return log;
}
pWorldObject WorldObject::getContainer()
{
    pWorldObject pcontainer = container.lock();
    return pcontainer;
}
pOdeObject WorldObject::getMainOdeObject()
{
    //TODO: check what is returned when odeObjects is empty. prolly a reset shared pointer, but i'm not sure
    //if (odeObjects.empty()) return NULL;
    return odeObjects.begin()->second;
}
void WorldObject::stepGraphics ()
{
    OgreObjectsIt i = ogreObjects.begin();
    for(;i != ogreObjects.end(); i++)
    {
        i->second->stepGraphics();
    }
}
void WorldObject::setPosition (Vector3d position)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition(position);
    }
}
Vector3d WorldObject::getPosition ()
{
    return odeObjects.begin()->second->getPosition();
}
Quaternion WorldObject::getRotation ()
{
    return odeObjects.begin()->second->getRotation();
}
void WorldObject::setRotation (Quaternion rotation)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setRotation(rotation);
    }
}
void WorldObject::applyRotation (Quaternion rotation)
{
    OdeObjectsIt j = odeObjects.begin();
    for(;j != odeObjects.end(); j++)
    {
        j->second->setPosition ( rotation.rotateObject(j->second->getPosition()) );
        Quaternion finalRotation = rotation * j->second->getRotation();
        j->second->setRotation (finalRotation);
    }
}
