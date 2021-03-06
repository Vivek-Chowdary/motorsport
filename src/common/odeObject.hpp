/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef ODEOBJECT_HPP
#define ODEOBJECT_HPP

#include "log/logEngine.hpp"
#include "worldObject.hpp"
#include <ode/objects.h>
#include "shared.hpp"
#include "vector3d.hpp"

class Quaternion;
class WorldObject;

SHARED_PTR_MAP(OdeObject, pOdeObject, OdeObjects, OdeObjectsIt, wOdeObject);
SHARED_PTR(MeshOdeData, pMeshOdeData, wMeshOdeData);
SHARED_PTR(PartOdeData, pPartOdeData, wPartOdeData);
SHARED_PTR(BodyOdeData, pBodyOdeData, wBodyOdeData);
SHARED_PTR(BoneOdeData, pBoneOdeData, wBoneOdeData);

SHARED_PTR_MAP(OgreObject, pOgreObject, OgreObjects, OgreObjectsIt, wOgreObject);
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
        OdeObject (WorldObject * worldObject, const std::string & name);
        OdeObject (WorldObject * worldObject, pMeshOdeData data, const std::string & name);
        OdeObject (WorldObject * worldObject, pPartOdeData data, const std::string & name);
        OdeObject (WorldObject * worldObject, pBodyOdeData data, const std::string & name);
        OdeObject (WorldObject * worldObject, pBoneOdeData data, const std::string & name);
        ~OdeObject ();
        Vector3d getPosition();
        Quaternion getRotation();
        void addPosition (Vector3d diff);
        void addRotation (Vector3d origin, Quaternion diff);
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
        dBodyID getBodyID();
        dGeomID getGeomID(const std::string & name);
        std::string getId();
};
class MeshOdeData
{
    public:
        std::string shape;
        pOgreObject ogreChild;
        MeshOdeData()
        {
            shape = "none";
        }
        ~MeshOdeData() {};
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
