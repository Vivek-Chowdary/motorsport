/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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
    log->put (LOG_TRACE, "Parsing body physics.");
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
            if (attribute == "length")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the body physics length: %s", attribute.c_str() );
                length = stod (attribute);
            }
            if (attribute == "width")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the body physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "height")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the body physics height: %s", attribute.c_str() );
                height = stod (attribute);
            }
            if (attribute == "mass")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the body physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            attribute.clear();
        }
    }
    log->put (LOG_TRACE, "Finished body physics.");
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, mass, length, width, height);
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);
    bodyGeomID = dCreateBox (World::getWorldPointer ()->spaceID, length, width, height);
    dGeomSetBody (bodyGeomID, bodyID);
}

void Body::setPosition (double posX, double posY, double posZ)
{               
    dBodySetPosition (bodyID, posX, posY, posZ);
}

void Body::setRotation (double rotX, double rotY, double rotZ)
{
    dMatrix3 rot;
    dRFromEulerAngles (rot, rotX, rotY, rotZ);
    //dRFromEulerAngles (rot, phi, theta, psi);
    dBodySetRotation (bodyID, rot);
}

void Body::stopPhysics ()
{
    dGeomDestroy (bodyGeomID);
    dBodyDestroy (bodyID);
}

void Body::stepPhysics ()
{
}