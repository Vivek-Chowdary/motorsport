/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "body.hpp"
#include "world.hpp"
#include "system.hpp"
#include "axis.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

void Body::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    double length = 1;
    double width = 1;
    double height = 1;
    double mass = 1;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "length")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the body physics length: %s", attribute.c_str() );
                length = stod (attribute);
            }
            if (attribute == "width")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the body physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "height")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the body physics height: %s", attribute.c_str() );
                height = stod (attribute);
            }
            if (attribute == "mass")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the body physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            attribute.clear();
        }
    }
    // create dBody
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    
    // create transform spaces (in order to be able to 'offset' the collision geoms
    geomSpace = dCreateGeomTransform (World::getWorldPointer ()->spaceID);
    geomSpace2 = dCreateGeomTransform (World::getWorldPointer ()->spaceID);

    // bind geoms with transformation spaces
    dGeomTransformSetCleanup (geomSpace, 1);
    dGeomTransformSetCleanup (geomSpace2, 1);
    
    // create collision geoms
    bodyGeomID = dCreateBox (0, length, width, height / 2.0);
    bodyGeom2ID = dCreateBox (0, length / 2.0, width / 2.0, height / 2.0);

    // insert collision geoms into transformation spaces
    dGeomTransformSetGeom (geomSpace, bodyGeomID);
    dGeomTransformSetGeom (geomSpace2, bodyGeom2ID);

    // apply offsets to the collision geoms
    dGeomSetPosition (bodyGeomID, 0, 0, - height / 4.0);
    dGeomSetPosition (bodyGeom2ID, -width / 6, 0, (height / 4.0) + 0.1);

    // associate the dBody with the 2 collision geoms via the transformation spaces
    dGeomSetBody (geomSpace, bodyID);
    dGeomSetBody (geomSpace2, bodyID);

    // set dBody mass
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, mass, length, width, height);
    dBodySetMass (bodyID, &tmpMass);

    // make sure it's initialized with correct values.
    setPosition(Vector3d(0, 0, 0));
    setRotation(Vector3d(0, 0, 0));
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
}

void Body::setPosition (Vector3d position)
{               
    dBodySetPosition (bodyID, position.x, position.y, position.z);
}
Vector3d Body::getPosition ()
{
    const dReal *temp = dBodyGetPosition (bodyID);
    return Vector3d (temp[0], temp[1], temp[2]);
}

void Body::setRotation (Vector3d rotation)
{
    dMatrix3 rot;
    dRFromEulerAngles (rot, rotation.x, rotation.y, rotation.z);
    dBodySetRotation (bodyID, rot);
}
Vector3d Body::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (bodyID);
    return Vector3d (temp[0], temp[1], temp[2], temp[3]);
}

void Body::stopPhysics ()
{
    dGeomDestroy (geomSpace);
    dGeomDestroy (geomSpace2);
    dBodyDestroy (bodyID);
}

void Body::stepPhysics ()
{
    if (this == World::getWorldPointer ()->vehicleList[0]->body)
    {
        double moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_BACKSPACE)]->getValue() * 50000;
        moveZ += SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RETURN)]->getValue() * 12200;
        moveZ -= SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RSHIFT)]->getValue() * 10000;
        dBodyAddForce (bodyID, 0, 0, moveZ);
    }
}
