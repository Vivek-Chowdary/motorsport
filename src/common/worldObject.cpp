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
WorldObjects WorldObject::worldObjects;

WorldObject::WorldObject (WorldObject * container, const std::string & name)
{
    this->base = dynamic_cast<WorldObject *>(this);
    this->name = name;
    this->container = container;
    if (container == NULL)
    {
        this->path = "/unknownPath/";
        this->xmlPath = path + "unknownFilepath";
    }
    else
    {
        this->path = container->getPath();
        this->xmlPath = container->getPath();
    }

    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;

    //FIXME what to use, id# or name?
    log = LogEngine::create (LOG_DEVELOPER, this->getFullName());
    //log = new LogEngine (LOG_DEVELOPER, this->id);
    log->__format(LOG_CCREATOR, "Id #%s. Full Name: %s", id.c_str(), getFullName().c_str());
    worldObjects[this->id] = this;
}
WorldObject::~WorldObject ()
{
    log->__format(LOG_CCREATOR, "Deleting myself. Id #%s. Full Name: %s", id.c_str(), getFullName().c_str());

    worldObjects.erase(this->id);
    base = NULL;
}
void WorldObject::logAll()
{
    pLogEngine log (LogEngine::create(LOG_DEVELOPER, "WorldObjects"));
    WorldObjectsIt i = worldObjects.begin();
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
    if (container == NULL) return name;
    return container->getFullName() + "." + name;
}
std::string WorldObject::getPath()
{
    return path;
}
void WorldObject::setPath(std::string path)
{
    this->path = path;;
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
WorldObject * WorldObject::getContainer()
{
    return container;
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
