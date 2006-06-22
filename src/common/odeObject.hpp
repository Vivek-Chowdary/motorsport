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
#include <shared.hpp>
#include "vector3d.hpp"

class Quaternion;
class WorldObject;

SHARED_PTR_MAP(OdeObject, pOdeObject, OdeObjects, OdeObjectsIt);
SHARED_PTR(PartOdeData, pPartOdeData);
SHARED_PTR(BodyOdeData, pBodyOdeData);
SHARED_PTR(WheelOdeData, pWheelOdeData);
SHARED_PTR(BoneOdeData, pBoneOdeData);

SHARED_MAP(dGeomID, GeomIDs, GeomIDsIt);

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
        OdeObject (WorldObject * worldObject, pPartOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, pBodyOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, pWheelOdeData data, std::string name);
        OdeObject (WorldObject * worldObject, pBoneOdeData data, std::string name);
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
        dReal mass;
        std::string shape;
        Vector3d size;
        dReal radius;
        dReal length;
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
        dReal length;
        dReal width;
        dReal height;
        dReal mass;

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
        dReal radius;
        dReal width;
        dReal mass;

        WheelOdeData()
        {
            mass = width = radius = 0;
        }
        ~WheelOdeData() {};
};
class BoneOdeData
{
    public:
        dReal length;
        dReal radius;
        dReal mass;
        dReal density;
        bool useMass;
        BoneOdeData()
        {
            length = radius = mass = density = 0;
            useMass = true;
        }
        ~BoneOdeData() {};
};
#endif
