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
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, mass, length, width, height);
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    bodyGeomID = dCreateBox (World::getWorldPointer ()->spaceID, length, width, height);
    dGeomSetBody (bodyGeomID, bodyID);
    dBodySetMass (bodyID, &tmpMass);
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
    dGeomDestroy (bodyGeomID);
    dBodyDestroy (bodyID);
}

void Body::stepPhysics ()
{
    if (this == World::getWorldPointer ()->vehicleList[0]->body)
    {
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RETURN)]->getValue() * 20000;
        dBodyAddForce (bodyID, 0, 0, moveZ);
    }
}
