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
std::string OgreObject::getId()
{
    return id;
}
OgreObject::OgreObject (WorldObject * worldObject, OgreObjectData data, std::string name, bool useMesh)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    this->useMesh = useMesh;
    std::string meshPath = data.meshPath;
    entity = NULL;
    node = NULL;
    odeObject = NULL;

    if (useMesh)
    {
        entity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (id, meshPath);
        entity->setCastShadows(true);
        for(unsigned int i = 0; i < entity->getNumSubEntities(); i++)
        {
            worldObject->getLog()->__format(LOG_CCREATOR, "Entity submesh #%i material: %s.", i, entity->getSubEntity(i)->getMaterialName().c_str() );
        }
    }
    node = static_cast<Ogre::SceneNode*>(SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->createChild());
    if (useMesh)
    {
        node->attachObject (entity);
        //entity->setRenderDetail(Ogre::SDL_WIREFRAME);
    }
}
OgreObject::~OgreObject ()
{
    if (useMesh)
    {
        worldObject->getLog()->__format (LOG_DEVELOPER, "Removing entity id=%s", entity->getName().c_str());
        node->detachObject(entity);
        if (entity->isAttached()) worldObject->getLog()->__format(LOG_WARNING, "Entity \"%s\" (%s) still attached somewhere!.", name.c_str(), entity->getName().c_str());
        SystemData::getSystemDataPointer()->ogreSceneManager->destroyEntity (entity);
        entity = NULL;
    }

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

void OgreObject::setOgreReference(OgreObject * ogreObject, Quaternion rotationDiff, Vector3d positionDiff, Vector3d scale)
{
    static_cast<Ogre::SceneNode*>(SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode())->removeChild(node);
    ogreObject->getNode()->addChild(node);
    Ogre::Vector3 * pd = new Ogre::Vector3(positionDiff.x, positionDiff.y, positionDiff.z);
    Ogre::Quaternion * rd = new Ogre::Quaternion (rotationDiff.w, rotationDiff.x, rotationDiff.y, rotationDiff.z);
    node->setPosition(*pd);
    node->setOrientation(*rd);
    node->setScale(scale.x, scale.y, scale.z);
}


Ogre::Entity * OgreObject::getEntity()
{
    return entity;
}
Ogre::SceneNode * OgreObject::getNode()
{
    return node;
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
