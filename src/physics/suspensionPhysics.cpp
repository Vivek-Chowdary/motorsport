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

#include "suspension.hpp"
#include "world.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "vector3d.hpp"
#include "vehicle.hpp"
#include "body.hpp"
#include "wheel.hpp"


void Suspension::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    double springConstant = 0;
    double dampingConstant = 0;
    position = new Vector3d (0, 0, 0);
    rotation = new Vector3d (0, 0, 0);
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
            if (attribute == "position")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the position: %s", attribute.c_str());
                position = new Vector3d (stov3d (attribute));
            }
            if (attribute == "rotation")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the rotation: %s", attribute.c_str());
                rotation = new Vector3d (stov3d (attribute));
            }
            if (attribute == "springConstant")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the suspension spring constant: %s", attribute.c_str() );
                springConstant = stod (attribute);
            }
            if (attribute == "dampingConstant")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the suspension damping constant: %s", attribute.c_str() );
                dampingConstant = stod (attribute);
            }
            attribute.clear();
        }
    }
    jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
}

void Suspension::attach (Wheel & wheel, Vehicle & vehicle)
{
    dJointAttach (jointID, wheel.wheelID, vehicle.body->bodyID);
    Vector3d absPos = vehicle.getPosition();
    absPos -= (*position);
    setPosition (absPos);
    Vector3d absRot = vehicle.getRotation();
    absRot -= (*rotation);
//    setRotation (absRot);
//    dJointSetHingeAxis (jointID, 0,1,0);
}

void Suspension::setPosition (Vector3d position)
{
    dJointSetHingeAnchor (jointID, position.x, position.y, position.z);
}
Vector3d Suspension::getPosition ()
{
    dVector3 temp;
    dJointGetHingeAnchor (jointID, temp);
    return Vector3d (*(temp + 0), *(temp + 1), *(temp + 2));
}
Vector3d Suspension::getRotation ()
{
    dVector3 temp;
    dJointGetHingeAxis (jointID, temp);
    return Vector3d (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Suspension::setRotation (Vector3d rotation)
{
    //dMatrix3 rot;
    rotation.degreesToRadians();
    //dRFromEulerAngles (rot, rotation.x, rotation.y, rotation.z);
    //dJointSetHingeAxis (jointID, rot.x, rot.y, rot.z);
    dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
}

void Suspension::stopPhysics ()
{
    dJointDestroy (jointID);
}

void Suspension::stepPhysics ()
{
}
