/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "world.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"

void Wheel::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    double mass = 0.0;
    double radius = 0.0;
    double width = 0.0;
    powered = 0.0;
    if (n->hasAttributes ())
    {
        // get all the attributes of the node
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();

        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "radius")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the wheel physics radius: %s", attribute.c_str() );
                radius = stod (attribute);
            }
            if (attribute == "width")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the wheel physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "mass")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the wheel physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "powered")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the wheel power transmission: %s", attribute.c_str() );
                powered = stod (attribute);
            }
            attribute.clear();
        }
    }
    dMass tmpMass;
    dMassSetCylinderTotal (&tmpMass, mass, 3, radius, width);
    wheelID = dBodyCreate (World::getWorldPointer ()->worldID);
    wheelGeomID = dCreateCCylinder (World::getWorldPointer ()->spaceID, radius, width);
    dGeomSetBody (wheelGeomID, wheelID);
    setPosition (Vector3d (0, 0, 0) );
    setRotation (Vector3d (0, 0, 0) );
    torque = 0;
}

void Wheel::setPosition (Vector3d position)
{               
    dBodySetPosition (wheelID, position.x, position.y, position.z);
}

Vector3d Wheel::getPosition ()
{
    const dReal *temp = dBodyGetPosition (wheelID);
    return Vector3d (temp[0], temp[1], temp[2]);
}

void Wheel::setRotation (Vector3d rotation)
{
    dMatrix3 rot;
    dRFromEulerAngles (rot, rotation.x, rotation.y, rotation.z);
    dBodySetRotation (wheelID, rot);
}

Vector3d Wheel::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (wheelID);
    return Vector3d (temp[0], temp[1], temp[2], temp[3]);
}

void Wheel::stopPhysics ()
{
    dGeomDestroy (wheelGeomID);
    dBodyDestroy (wheelID);
}

void Wheel::stepPhysics ()
{
    const dReal * angularVelVector;
    dBodyAddRelTorque (wheelID, 0, 0, torque * powered);
    angularVelVector = dBodyGetAngularVel (wheelID);
    angularVel = angularVelVector[1];    
    torque = 0;
}

void Wheel::addTorque(double torque)
{
    this->torque = torque;
}

double Wheel::getAngularVel()
{
    return angularVel;
}
