/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef OGREOBJECT_HPP
#define OGREOBJECT_HPP
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
        static unsigned int instancesCount;
        void updateId();
        std::string name;

        Ogre::Entity * entity;
        Ogre::SceneNode * node;
        OdeObject * odeObject;
        WorldObject * worldObject;
        std::string id;
    public:
        OgreObject (WorldObject * worldObject, OgreObjectData data, std::string name);
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
}  /////TODO///////////////look down!! ---v

/*void Body::setRenderDetail(int renderMode) {
    Ogre::SceneDetailLevel mode;
    switch (renderMode)
    {
    case 1:
        mode = Ogre::SDL_POINTS;
        break;
    case 2:
        mode = Ogre::SDL_WIREFRAME;
        break;
    case 3:
        mode = Ogre::SDL_SOLID;
        break;
    }
    bodyEntity->setRenderDetail(mode);
}
*/;
#endif
