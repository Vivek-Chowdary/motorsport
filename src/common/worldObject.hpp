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
#include "logEngine.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"
#include "xmlTag.hpp"
#include <string>
#include <map>

//forward declatations
class Quaternion;
class Vector3d;
class OgreObject;
typedef std::map <std::string, OgreObject *> OgreObjects;
typedef std::map <std::string, OgreObject *>::iterator OgreObjectsIt;
class OdeObject;
typedef std::map <std::string, OdeObject *> OdeObjects;
typedef std::map <std::string, OdeObject *>::iterator OdeObjectsIt;
class WorldObject;
typedef std::map <std::string, WorldObject *> WorldObjects;
typedef std::map <std::string, WorldObject *>::iterator WorldObjectsIt;

class WorldObject
{
    private:
        virtual void hack(){};
        static unsigned int instancesCount;
        static WorldObjects worldObjects;
        std::string id;
        std::string name;
        std::string path;
        std::string xmlPath;
    protected:
        LogEngine * log;
        WorldObject * base;
        WorldObject * container;
        OgreObjects ogreObjects;
        OdeObjects odeObjects;
    public:
        WorldObject (WorldObject * container, const std::string & name);
        virtual ~WorldObject ();
        WorldObject * getContainer();
        std::string getId();
        std::string getName();
        std::string getFullName();
        std::string getPath();
        std::string getXmlPath();
        void setName(std::string name);
        void setPath(std::string path);
        void setXmlPath(std::string xmlPath);

        static void logAll();
        LogEngine * getLog();

        void stepGraphics ();

        OdeObject * getMainOdeObject();
        
        Vector3d getPosition ();
        Quaternion getRotation ();
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
        void applyRotation (Quaternion rotation);
};
#endif
