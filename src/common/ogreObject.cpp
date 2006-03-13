/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "ogreObject.hpp"
#include "system.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "vector3d.hpp"
#include "quaternion.hpp"
#include "odeObject.hpp"

unsigned int OgreObject::instancesCount = 0;
void OgreObject::updateId()
{
    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;
}
OgreObject::OgreObject (WorldObject * worldObject, OgreObjectData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    std::string meshPath = data.meshPath;
    entity = NULL;
    node = NULL;
    odeObject = NULL;

    bool error = true;
    try { SystemData::getSystemDataPointer()->ogreSceneManager->getEntity (id); }
    catch (Ogre::Exception & e) { error = false; }
    if (error) worldObject->getLog()->__format(LOG_ERROR, "Entity \"#%s\" (%s) already exists!.", id.c_str(), name.c_str());

    entity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (id, meshPath);
    entity->setCastShadows(true);
    for(unsigned int i = 0; i < entity->getNumSubEntities(); i++)
    {
        worldObject->getLog()->__format(LOG_CCREATOR, "Entity submesh #%i material: %s.", i, entity->getSubEntity(i)->getMaterialName().c_str() );
    }
    node = static_cast<Ogre::SceneNode*>(SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->createChild());
    node->attachObject (entity);
}
OgreObject::~OgreObject ()
{
    worldObject->getLog()->__format (LOG_DEVELOPER, "Removing entity id=%s", entity->getName().c_str());
    node->detachObject(entity);
    if (entity->isAttached()) worldObject->getLog()->__format(LOG_WARNING, "Entity \"%s\" (%s) still attached somewhere!.", name.c_str(), entity->getName().c_str());
    SystemData::getSystemDataPointer()->ogreSceneManager->removeEntity (entity);
    entity = NULL;

    worldObject->getLog()->__format (LOG_DEVELOPER, "Removing node id=%s", node->getName().c_str());
    SystemData::getSystemDataPointer()->ogreSceneManager->destroySceneNode (node->getName());
    node = NULL;   

    odeObject = NULL;
    this->worldObject = NULL;
}
void OgreObject::setOdeReference(OdeObject * odeObject)
{
    this->odeObject = odeObject;
}
void OgreObject::stepGraphics()
{
    if (odeObject)
    {
        Vector3d position = odeObject->getPosition();
        node->setPosition(position.x, position.y, position.z);
        Quaternion rotation = odeObject->getRotation();
        node->setOrientation(rotation.w, rotation.x, rotation.y, rotation.z);
    }
}
