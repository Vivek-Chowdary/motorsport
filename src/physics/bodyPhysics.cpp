/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "ode/ode.h"
#include "math.h"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

void Body::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    double length = 1;
    double width = 1;
    double height = 1;
    double mass = 1;
    dragCoefficient = 0.3;
    frontalArea = 0;
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
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics length: %s", attribute.c_str() );
                length = stod (attribute);
            }
            if (attribute == "width")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "height")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics height: %s", attribute.c_str() );
                height = stod (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "frontalArea")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body frontal area: %s", attribute.c_str() );
                frontalArea = stod (attribute);
            }
            if (attribute == "dragCoefficient")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the body drag coefficient: %s", attribute.c_str() );
                dragCoefficient = stod (attribute);
            }
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
    Quaternion finalRotation = Quaternion(0,0,0,0);
    dMatrix3 rot;
    finalRotation.getOdeMatrix (rot);
    dBodySetRotation (bodyID, rot);
    dBodySetPosition (bodyID, 0, 0 ,0);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);

    // set the air drag variables correctly
    if (frontalArea == 0)
    {
        frontalArea = width * height * 0.6;
    }
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

void Body::applyRotation (Quaternion rotation)
{
    setPosition ( rotation.rotateObject(getPosition()) );
    dMatrix3 rot;
    Quaternion finalRotation = rotation * getRotation();
    finalRotation.getOdeMatrix (rot);
    dBodySetRotation (bodyID, rot);
}
Quaternion Body::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (bodyID);
    return Quaternion (temp);
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
    
    // apply simple air drag forces:
    const double airDensity = 1.225;
    //  f = Cx              * 0.5 * airDensity * v^2     * area;
    //  f = dragCoefficient * 0.5 * 1.225      * vel*vel * frontalArea;
    Vector3d velocity (dBodyGetLinearVel (bodyID));
    double velModule = velocity.distance();
    Vector3d normalizedVel (velocity);
    normalizedVel.scalarDivide(velModule);
    normalizedVel.scalarMultiply(-1);
    Vector3d force (normalizedVel);
    force.scalarMultiply (0.5 * dragCoefficient * airDensity * frontalArea * velModule * velModule);

    dBodyAddForce (bodyID, force.x, force.y, force.z);

    log->__format(LOG_DEVELOPER, "Body air drag force = (%f, %f, %f)", force.x, force.y, force.z);
}
