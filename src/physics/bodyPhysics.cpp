/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#include "body.hpp"
#include "world.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"


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
    setPosition(Vector3d(0, 0, 0));
    setRotation(Vector3d(0, 0, 0));
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
}
