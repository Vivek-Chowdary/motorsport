/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef OGREOBJECT_HPP
#define OGREOBJECT_HPP
#include "tools/xercesc_fwd.hpp"
#include "log/logEngine.hpp"
#include "worldObject.hpp"
#include <map>

namespace Ogre {
    class Entity;
    class SceneNode;
}
class OdeObject;
class OgreObject;
class OgreObjectData;
class WorldObject;

typedef std::map <std::string, OgreObject *> OgreObjects;
typedef std::map <std::string, OgreObject *>::iterator OgreObjectsIt;

class OgreObject
{
    protected:
        Ogre::Entity * entity;
        Ogre::SceneNode * node;
        OdeObject * odeObject;
        WorldObject * worldObject;
        std::string identifier;
    public:
        OgreObject (WorldObject * worldObject, OgreObjectData data, std::string identifier);
        ~OgreObject ();
        void setOdeReference(OdeObject * odeObject);       
        void stepGraphics();
};
class OgreObjectData
{
    public:
        std::string meshPath;
        OgreObjectData()
        {
            meshPath = "none";
        }
        ~OgreObjectData() {};
};
#endif
