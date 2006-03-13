/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef ODEOBJECT_HPP
#define ODEOBJECT_HPP
#include "log/logEngine.hpp"
#include "worldObject.hpp"
#include "ode/objects.h"
#include <map>
#include "vector3d.hpp"

class OdeObject;
class PartOdeData;
class BodyOdeData;
class WheelOdeData;
class BoneOdeData;
class Quaternion;
class WorldObject;

typedef std::map <std::string, OdeObject *> OdeObjects;
typedef std::map <std::string, OdeObject *>::iterator OdeObjectsIt;
typedef std::map <std::string, dGeomID> GeomIDs;
typedef std::map <std::string, dGeomID>::iterator GeomIDsIt;

class OdeObject
{
    protected:
        static unsigned int instancesCount;
        void updateId();
        std::string name;
        WorldObject * worldObject;
        dBodyID bodyID;
        GeomIDs geomIDs;
        std::string id;

    public:
        OdeObject (WorldObject * worldObject, PartOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, BodyOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, WheelOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, BoneOdeData data, std::string name);
        ~OdeObject ();
        Vector3d getPosition();
        Quaternion getRotation();
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
        dBodyID getBodyID();
        dGeomID getGeomID(std::string name);
};
class PartOdeData
{
    public:
        double mass;
        std::string shape;
        Vector3d size;
        double radius;
        double length;
        int directionAxis;

        PartOdeData()
        {
            mass = -1;
            shape = "none";
            size.x = size.y = size.z = 1;
            radius = 1;
            length = 1;
            directionAxis = 3;
        }
        ~PartOdeData() {};
};
class BodyOdeData
{
    public:
        double length;
        double width;
        double height;
        double mass;

        BodyOdeData()
        {
            mass = -1;
            width = height = length = 1;
        }
        ~BodyOdeData() {};
};
class WheelOdeData
{
    public:
        double radius;
        double width;
        double mass;

        WheelOdeData()
        {
            mass = width = radius = 0;
        }
        ~WheelOdeData() {};
};
class BoneOdeData
{
    public:
        double length;
        double radius;
        double mass;
        double density;
        bool useMass;
        BoneOdeData()
        {
            length = radius = mass = density = 0;
            useMass = true;
        }
        ~BoneOdeData() {};
};
#endif
