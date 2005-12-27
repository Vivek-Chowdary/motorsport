/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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

OgreObject::OgreObject (WorldObject * worldObject, OgreObjectData data, std::string identifier)
{
    this->worldObject = worldObject;
    this->identifier = identifier;
    std::string meshPath = data.meshPath;
    entity = NULL;
    node = NULL;
    odeObject = NULL;

    bool error = true;
    try { SystemData::getSystemDataPointer()->ogreSceneManager->getEntity (identifier); }
    catch (Ogre::Exception & e) { error = false; }
    if (error) worldObject->getLog()->__format(LOG_WARNING, "Entity \"%s\" already exists!.", identifier.c_str());

    entity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (identifier, meshPath);
    entity->setCastShadows(true);
    for(unsigned int i = 0; i < entity->getNumSubEntities(); i++)
    {
        worldObject->getLog()->__format(LOG_WARNING, "Entity submesh #%i material: %s.", i, entity->getSubEntity(i)->getMaterialName().c_str() );
    }
    node = static_cast<Ogre::SceneNode*>(SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->createChild());
    node->attachObject (entity);

}
OgreObject::~OgreObject ()
{
    node->detachObject(entity);
    if (entity->isAttached()) worldObject->getLog()->__format(LOG_WARNING, "Entity \"%s\" still attached somewhere!.", entity->getName().c_str());
    SystemData::getSystemDataPointer()->ogreSceneManager->removeEntity (entity);
    entity = NULL;

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