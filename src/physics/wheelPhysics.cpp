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
    log->put (LOG_TRACE, "Parsing wheel physics.");
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
                log->format (LOG_TRACE, "\tFound the wheel physics radius: %s", attribute.c_str() );
                radius = stod (attribute);
            }
            if (attribute == "width")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the wheel physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "mass")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the wheel physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            attribute.clear();
        }
    }
    log->put (LOG_TRACE, "Finished wheel physics.");
    dMass tmpMass;
    dMassSetCylinderTotal (&tmpMass, mass, 3, radius, width);
    wheelID = dBodyCreate (World::getWorldPointer ()->worldID);
    wheelGeomID = dCreateCCylinder (World::getWorldPointer ()->spaceID, radius, width);
    dGeomSetBody (wheelGeomID, wheelID);
}

void Wheel::setPosition (double posX, double posY, double posZ)
{               
    dBodySetPosition (wheelID, posX, posY, posZ);
}

void Wheel::setRotation (double rotX, double rotY, double rotZ)
{
    dMatrix3 rot;
    dRFromEulerAngles (rot, rotX, rotY, rotZ);
    dBodySetRotation (wheelID, rot);
}

void Wheel::stopPhysics ()
{
    dGeomDestroy (wheelGeomID);
    dBodyDestroy (wheelID);
}

void Wheel::stepPhysics ()
{
}

void Wheel::addTorque(double torque)
{
    dBodyAddRelTorque (wheelID, 0, 0, torque);
}
