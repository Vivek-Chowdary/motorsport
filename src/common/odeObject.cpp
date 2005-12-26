/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "odeObject.hpp"
#include "system.hpp"
#include "vector3d.hpp"
#include "quaternion.hpp"
#include "world.hpp"

OdeObject::OdeObject (WorldObject * worldObject, OdeObjectData data)
{
    this->worldObject = worldObject;
    geomID = NULL;
    bodyID = NULL;
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    if (data.mass <= 0)
    {
        worldObject->getLog()->__format (LOG_WARNING, "No mass has been defined for this part! Defaulting to 100kg.");
        data.mass = 100;
    }
    dMass dmass;
    if (data.shape == "box")
    {
        geomID = dCreateBox (World::getWorldPointer ()->spaceID, data.dimensions.x, data.dimensions.y, data.dimensions.z);
        dMassSetBoxTotal (&dmass, data.mass, data.dimensions.x, data.dimensions.y, data.dimensions.z);
    }
    if (data.shape == "sphere")
    {
        geomID = dCreateSphere (World::getWorldPointer ()->spaceID, data.radius);
        dMassSetSphereTotal (&dmass, data.mass, data.radius);
    }
    if (data.shape == "cappedCylinder")
    {
        geomID = dCreateCCylinder (World::getWorldPointer ()->spaceID, data.radius, data.length);
        dMassSetCappedCylinderTotal (&dmass, data.mass, data.directionAxis, data.radius, data.length);
    }
    dGeomSetBody (geomID, bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::~OdeObject ()
{
    //TODO: check if there's anything left to do here with ODE...
    dGeomDestroy (geomID);
    geomID = NULL;
    dBodyDestroy (bodyID);
    bodyID = NULL;
    this->worldObject = NULL;
}
void OdeObject::setPosition (Vector3d position)
{
    dBodySetPosition (bodyID, position.x, position.y, position.z);
}

void OdeObject::setRotation (Quaternion rotation)
{
    dMatrix3 rot;
    rotation.getOdeMatrix (rot);
    dBodySetRotation (bodyID, rot);
}
Vector3d OdeObject::getPosition()
{
    const dReal * tmp = dBodyGetPosition (bodyID);
    return Vector3d(tmp[0], tmp[1], tmp[2]);
}
Quaternion OdeObject::getRotation()
{
    const dReal * tmp = dBodyGetQuaternion (bodyID);
    return Quaternion(tmp[0], tmp[1], tmp[2], tmp[3]);
}
dBodyID OdeObject::getBodyID()
{
    return bodyID;
}
