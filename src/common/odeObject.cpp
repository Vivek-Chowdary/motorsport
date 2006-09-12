/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "odeObject.hpp"
#include "system.hpp"
#include "vector3d.hpp"
#include "quaternion.hpp"
#include "world.hpp"

unsigned int OdeObject::instancesCount = 0;

void OdeObject::updateId()
{
    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;
}
OdeObject::OdeObject (WorldObject * worldObject, pPartOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    bodyID = dBodyCreate (World::get()->worldID);
 
    //TODO make this optional via xml or something similar   
    /*/set autodisable
    dBodySetAutoDisableFlag (bodyID, 1);
    dBodySetAutoDisableLinearThreshold (bodyID, 0.15);
    dBodySetAutoDisableAngularThreshold (bodyID, 0.20);
    dBodySetAutoDisableSteps (bodyID, 1);
    //*/
    
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    dMass dmass;
    if (data->shape == "box")
    {
        geomIDs[name] = dCreateBox (World::get()->spaceID, data->size.x, data->size.y, data->size.z);
        dMassSetBoxTotal (&dmass, data->mass, data->size.x, data->size.y, data->size.z);
    }
    if (data->shape == "sphere")
    {
        geomIDs[name] = dCreateSphere (World::get()->spaceID, data->radius);
        dMassSetSphereTotal (&dmass, data->mass, data->radius);
    }
    if (data->shape == "cappedCylinder")
    {
        geomIDs[name] = dCreateCCylinder (World::get()->spaceID, data->radius, data->length);
        dMassSetCappedCylinderTotal (&dmass, data->mass, data->directionAxis, data->radius, data->length);
    }
    dGeomSetBody (geomIDs[name], bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::OdeObject (WorldObject * worldObject, pBodyOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    // create dBody
    bodyID = dBodyCreate (World::get()->worldID);
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    
    // create transform spaces (in order to be able to 'offset' the collision geoms
    geomIDs["GeomSpace(A)"] = dCreateGeomTransform (World::get()->spaceID);
    geomIDs["GeomSpace(B)"] = dCreateGeomTransform (World::get()->spaceID);

    // avoid ODE removing objects itself. transform geoms now must be manually removed
    dGeomTransformSetCleanup (geomIDs["GeomSpace(A)"], 0);
    dGeomTransformSetCleanup (geomIDs["GeomSpace(B)"], 0);
    
    // create collision geoms
    geomIDs["Geom(A)"] = dCreateBox (0, data->length, data->width, data->height / 2.0f);
    geomIDs["Geom(B)"] = dCreateBox (0, data->length / 2.0f, data->width / 2.0f, data->height / 2.0f);

    // insert collision geoms into transformation spaces
    dGeomTransformSetGeom (geomIDs["GeomSpace(A)"], geomIDs["Geom(A)"]);
    dGeomTransformSetGeom (geomIDs["GeomSpace(B)"], geomIDs["Geom(B)"]);

    // apply offsets to the collision geoms
    dGeomSetPosition (geomIDs["Geom(A)"], 0.f, 0.f, - data->height / 4.0f);
    dGeomSetPosition (geomIDs["Geom(B)"], -data->width / 6.f, 0.f, (data->height / 4.0f) + 0.1f);

    // associate the dBody with the 2 collision geoms via the transformation spaces
    dGeomSetBody (geomIDs["GeomSpace(A)"], bodyID);
    dGeomSetBody (geomIDs["GeomSpace(B)"], bodyID);

    // set dBody mass
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, data->mass, data->length, data->width, data->height);
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
OdeObject::OdeObject (WorldObject * worldObject, pWheelOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    bodyID = dBodyCreate (World::get()->worldID);
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    dMass dmass;
    dMassSetParameters (&dmass, data->mass,
                         0., 0., 0.,
                         0.237f, 0.237f, 0.409f,
                         0., 0., 0.);
    geomIDs[name] = dCreateCCylinder (World::get()->spaceID, data->radius, data->width);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
    dGeomSetBody (geomIDs[name], bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::OdeObject (WorldObject * worldObject, pBoneOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;

    bodyID = dBodyCreate (World::get()->worldID);
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    dMass dmass;
    if (data->useMass)
    {
        dMassSetBoxTotal(&dmass, data->mass, data->radius, data->radius, data->length);
    }
    else
    {
        dMassSetBox(&dmass, data->density, data->radius, data->radius, data->length);
    }
    dBodySetMass (bodyID, &dmass);

    geomIDs[name] = dCreateBox (World::get()->spaceID, data->radius, data->radius, data->length);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
    dGeomSetBody (geomIDs[name], bodyID);
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
    }
	geomIDs.clear();
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
dGeomID OdeObject::getGeomID(std::string name)
{
    return geomIDs[name];
}
