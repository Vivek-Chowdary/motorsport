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

OgreObject::OgreObject (WorldObject * worldObject, std::string meshPath, std::string identifier)
{
    this->worldObject = worldObject;
    this->identifier = identifier;
    entity = NULL;
    node = NULL;
    odeObject = NULL;
    //TODO: make the identifier unique, based on worldObject OgreObjects container index... or something.
    std::string name = worldObject->getIdentifier() + identifier;
    entity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name.c_str(), meshPath.c_str());
    node = static_cast<Ogre::SceneNode*>(SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->createChild());
    node->attachObject (entity);
}
OgreObject::~OgreObject ()
{
    node->removeAndDestroyAllChildren();
    if (entity->isAttached())
    {
        this->worldObject->getLog()->__format(LOG_WARNING, "Could not detach Ogre entity \"%s\".", this->entity->getName().c_str());
    }
    SystemData::getSystemDataPointer()->ogreSceneManager->removeEntity (entity);
    delete entity;
    entity = NULL;
    delete node;
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
