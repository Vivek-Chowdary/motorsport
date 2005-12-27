/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#define WORLDOBJECT_HPP
#include "tools/xercesc_fwd.hpp"
#include "log/logEngine.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"

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
    protected:
        LogEngine * log;
        WorldObject * base;
        WorldObject * container;
        OgreObjects ogreObjects;
        OdeObjects odeObjects;
    public:
        WorldObject (WorldObject * container, const std::string & name);
        virtual ~WorldObject ();
        std::string getId();
        std::string getName();
        void setName(std::string name);
        std::string getFullName();

        static void logAll();
        LogEngine * getLog();

        void WorldObject::stepGraphics ();

        OdeObject * getMainOdeObject();
        
        Vector3d getPosition ();
        Quaternion getRotation ();
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
};
#endif
