/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#define WORLDOBJECT_HPP
#include "system.hpp"
#include "logEngine.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"
#include "xmlTag.hpp"
#include <shared.hpp>

//forward declatations
class Quaternion;
class Vector3d;
SHARED_PTR_MAP(OgreObject, pOgreObject, OgreObjects, OgreObjectsIt, wOgreObject);
SHARED_PTR_MAP(OdeObject, pOdeObject, OdeObjects, OdeObjectsIt, wOdeObject);
SHARED_PTR_MAP(WorldObject, pWorldObject, WorldObjects, WorldObjectsIt, wWorldObject);
typedef std::map <std::string, WorldObject *> WorldObjectsC;
typedef std::map <std::string, WorldObject *>::iterator WorldObjectsCIt;

class WorldObject: public boost::enable_shared_from_this <WorldObject>
{
    private:
        virtual void hack(){};
        static unsigned int instancesCount;
        static WorldObjectsC worldObjects;
        std::string id;
        std::string name;
        std::string path;
        std::string xmlPath;
    protected:
        pLogEngine log;
        WorldObject * base;
        wWorldObject container;
        OgreObjects ogreObjects;
        OdeObjects odeObjects;
        WorldObjects objects;
        WorldObject (const std::string & name);
    public:
        virtual ~WorldObject ();
        void setContainer(pWorldObject container);
        pWorldObject getContainer();
        std::string getId();
        std::string getName();
        std::string getFullName();
        std::string getPath();
        std::string getXmlPath();
        void setName(std::string name);
        void setPath(std::string path);
        void setXmlPath(std::string xmlPath);

        static void logAll();
        pLogEngine getLog();

        void stepGraphics ();

        pOdeObject getMainOdeObject();
        
        Vector3d getPosition ();
        Quaternion getRotation ();
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
        void applyRotation (Quaternion rotation);
};
#endif
