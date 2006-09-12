/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef OGREOBJECT_HPP
#define OGREOBJECT_HPP
#include "log/logEngine.hpp"
#include "worldObject.hpp"
#include <shared.hpp>

namespace Ogre {
    class Entity;
    class SceneNode;
}
class WorldObject;
class Quaternion;
class Vector3d;

SHARED_PTR_MAP(OdeObject, pOdeObject, OdeObjects, OdeObjectsIt, wOdeObject);
SHARED_PTR_MAP(OgreObject, pOgreObject, OgreObjects, OgreObjectsIt, wOgreObject);
SHARED_PTR(OgreObjectData, pOgreObjectData, wOgreObjectData);

class OgreObject
{
    protected:
        static unsigned int instancesCount;
        void updateId();
        std::string name;

        Ogre::Entity * entity;
        bool useMesh;
        Ogre::SceneNode * node;
        pOdeObject odeObject;
        WorldObject * worldObject;
        std::string id;
    public:
        OgreObject (WorldObject * worldObject, pOgreObjectData data, std::string name, bool useMesh = true);
        ~OgreObject ();
        std::string getId();
        void setOdeReference(pOdeObject odeObject);       
        void setOgreReference(pOgreObject ogreObject, Quaternion rotationDiff, Vector3d positionDiff, Vector3d scale);
        void stepGraphics();
        Ogre::SceneNode * getNode();
        Ogre::Entity * getEntity();
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
