/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "worldObject.hpp"

unsigned int WorldObject::instancesCount = 0;
WorldObjects WorldObject::worldObjects;

WorldObject::WorldObject (WorldObject * container, const std::string & name)
{
    this->base = dynamic_cast<WorldObject *>(this);
    this->name = name;
    this->container = container;

    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    this->id = numberString;
    instancesCount++;

    //FIXME what to use, id# or name?
    log = new LogEngine (LOG_DEVELOPER, this->getFullName());
    //log = new LogEngine (LOG_DEVELOPER, this->id);
    log->__format(LOG_CCREATOR, "Id: %s. Full Name: %s", id.c_str(), getFullName().c_str());
    worldObjects[this->id] = this;
}
WorldObject::~WorldObject ()
{
    WorldObjectsIt i = worldObjects.begin();
    for(;i != worldObjects.end(); i++)
    {
        if (this == i->second) worldObjects.erase(i);
    }
    delete log;
    log = NULL;
    base = NULL;
}
void WorldObject::logAll()
{
    LogEngine log (LOG_DEVELOPER, "WorldObjects");
    WorldObjectsIt i = worldObjects.begin();
    for(;i != worldObjects.end(); i++)
    {
        log.__format(LOG_DEVELOPER, "WorldObject id#%s.\t Full name: %s", i->second->getId().c_str(), i->second->getFullName().c_str());
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
}
std::string WorldObject::getFullName()
{
    if (container == NULL) return name;
    return container->getFullName() + "." + name;
}
LogEngine * WorldObject::getLog()
{
    return log;
}
void WorldObject::stepGraphics ()
{
    OgreObjectsIt i = ogreObjects.begin();
    for(;i != ogreObjects.end(); i++)
    {
        i->second->stepGraphics();
    }
}
