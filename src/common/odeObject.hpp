/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef ODEOBJECT_HPP
#define ODEOBJECT_HPP
#include "tools/xercesc_fwd.hpp"
#include "log/logEngine.hpp"
#include "worldObject.hpp"
#include "ode/objects.h"
#include <map>
#include "vector3d.hpp"

class OdeObject;
class OdeObjectData;
class VehicleBodyOdeObjectData;
class Quaternion;
class WorldObject;

typedef std::map <std::string, OdeObject *> OdeObjects;
typedef std::map <std::string, OdeObject *>::iterator OdeObjectsIt;
typedef std::map <std::string, dGeomID> GeomIDs;
typedef std::map <std::string, dGeomID>::iterator GeomIDsIt;

class OdeObject
{
    protected:
        WorldObject * worldObject;
        dBodyID bodyID;
        GeomIDs geomIDs;
        std::string identifier;

    public:
        OdeObject (WorldObject * worldObject, OdeObjectData data, std::string identifier);
        OdeObject (WorldObject * worldObject, VehicleBodyOdeObjectData data, std::string identifier);
        ~OdeObject ();
        Vector3d getPosition();
        Quaternion getRotation();
        void setPosition (Vector3d position);
        void setRotation (Quaternion rotation);
        dBodyID getBodyID();
};
class OdeObjectData
{
    public:
        double mass;
        std::string shape;
        Vector3d dimensions;
        double radius;
        double length;
        int directionAxis;

        OdeObjectData()
        {
            mass = -1;
            shape = "none";
            dimensions.x = dimensions.y = dimensions.z = 1;
            radius = 1;
            length = 1;
            directionAxis = 3;
        }
        ~OdeObjectData() {};
};
class VehicleBodyOdeObjectData
{
    public:
        double length;
        double width;
        double height;
        double mass;

        VehicleBodyOdeObjectData()
        {
            mass = -1;
            width = height = length = 1;
        }
        ~VehicleBodyOdeObjectData() {};
};
#endif
