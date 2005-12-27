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

OdeObject::OdeObject (WorldObject * worldObject, OdeObjectData data, std::string identifier)
{
    this->worldObject = worldObject;
    this->identifier = identifier;
    std::string geomIdentifier = identifier + ",Geom(unique)";
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
        geomIDs[geomIdentifier] = dCreateBox (World::getWorldPointer ()->spaceID, data.dimensions.x, data.dimensions.y, data.dimensions.z);
        dMassSetBoxTotal (&dmass, data.mass, data.dimensions.x, data.dimensions.y, data.dimensions.z);
    }
    if (data.shape == "sphere")
    {
        geomIDs[geomIdentifier] = dCreateSphere (World::getWorldPointer ()->spaceID, data.radius);
        dMassSetSphereTotal (&dmass, data.mass, data.radius);
    }
    if (data.shape == "cappedCylinder")
    {
        geomIDs[geomIdentifier] = dCreateCCylinder (World::getWorldPointer ()->spaceID, data.radius, data.length);
        dMassSetCappedCylinderTotal (&dmass, data.mass, data.directionAxis, data.radius, data.length);
    }
    dGeomSetBody (geomIDs[geomIdentifier], bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::OdeObject (WorldObject * worldObject, VehicleBodyOdeObjectData data, std::string identifier)
{
    this->worldObject = worldObject;
    this->identifier = identifier;
    bodyID = NULL;
    // create dBody
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    
    // create transform spaces (in order to be able to 'offset' the collision geoms
    geomIDs["GeomSpace(A)"] = dCreateGeomTransform (World::getWorldPointer ()->spaceID);
    geomIDs["GeomSpace(B)"] = dCreateGeomTransform (World::getWorldPointer ()->spaceID);

    // avoid ODE removing objects itself. transform geoms now must be manually removed
    dGeomTransformSetCleanup (geomIDs["GeomSpace(A)"], 0);
    dGeomTransformSetCleanup (geomIDs["GeomSpace(B)"], 0);
    
    // create collision geoms
    geomIDs["Geom(A)"] = dCreateBox (0, data.length, data.width, data.height / 2.0);
    geomIDs["Geom(B)"] = dCreateBox (0, data.length / 2.0, data.width / 2.0, data.height / 2.0);

    // insert collision geoms into transformation spaces
    dGeomTransformSetGeom (geomIDs["GeomSpace(A)"], geomIDs["Geom(A)"]);
    dGeomTransformSetGeom (geomIDs["GeomSpace(B)"], geomIDs["Geom(B)"]);

    // apply offsets to the collision geoms
    dGeomSetPosition (geomIDs["Geom(A)"], 0, 0, - data.height / 4.0);
    dGeomSetPosition (geomIDs["Geom(B)"], -data.width / 6, 0, (data.height / 4.0) + 0.1);

    // associate the dBody with the 2 collision geoms via the transformation spaces
    dGeomSetBody (geomIDs["GeomSpace(A)"], bodyID);
    dGeomSetBody (geomIDs["GeomSpace(B)"], bodyID);

    // set dBody mass
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, data.mass, data.length, data.width, data.height);
    dBodySetMass (bodyID, &tmpMass);

    // make sure it's initialized with correct values.
    Quaternion finalRotation = Quaternion(0,0,0,0);
    dMatrix3 rot;
    finalRotation.getOdeMatrix (rot);
    dBodySetRotation (bodyID, rot);
    dBodySetPosition (bodyID, 0, 0 ,0);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
}
OdeObject::OdeObject (WorldObject * worldObject, VehicleWheelOdeObjectData data, std::string identifier)
{
    this->worldObject = worldObject;
    this->identifier = identifier;
    std::string geomIdentifier = identifier + ",Geom(unique)";
    bodyID = NULL;
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    if (data.mass <= 0)
    {
        worldObject->getLog()->__format (LOG_WARNING, "No mass has been defined for this part! Defaulting to 100kg.");
        data.mass = 100;
    }
    dMass dmass;
    dMassSetParameters (&dmass, data.mass,
                         0, 0, 0,
                         0.237, 0.237, 0.409,
                         0, 0, 0);
    geomIDs[geomIdentifier] = dCreateCCylinder (World::getWorldPointer ()->spaceID, data.radius, data.width);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
    dGeomSetBody (geomIDs[geomIdentifier], bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::~OdeObject ()
{
    //TODO: check if there's anything left to do here with ODE...
    worldObject->getLog()->__format (LOG_DEVELOPER, "Removing geoms");
    GeomIDsIt i = geomIDs.begin();
    for(;i != geomIDs.end(); i++)
    {
        worldObject->getLog()->__format (LOG_DEVELOPER, "Removing geom id=%s", i->first.c_str());
        dGeomDestroy (i->second);
        i->second = NULL;
        geomIDs.erase(i);
    }

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
    return Quaternion(tmp);
}
dBodyID OdeObject::getBodyID()
{
    return bodyID;
}
