/******************************************************************************
*
* Copyright (C) 2004 Benjamin Beacock (bbeacock@uoguelph.ca)
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

#include "clutch.hpp"
#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Clutch::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueTransfer = 0.0;
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
            if (attribute == "coeffStaticFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch static coefficient of friction: %s", attribute.c_str() );
                coeffStaticFriction = stod (attribute);
            }
            if (attribute == "coeffDynamicFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch dynamic coefficient of friction: %s", attribute.c_str() );
                coeffDynamicFriction = stod (attribute);
            }
            if (attribute == "maxTorqueTransfer")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the clutch maximum torque transfer: %s", attribute.c_str() );
                coeffDynamicFriction = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Clutch::stopPhysics ()
{
}

void Clutch::stepPhysics ()
{
    if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_g)]->getValue()) 
        torqueTransfer = 0.0;
    else
        torqueTransfer = inputClass->getTorque();
    rotationalVelocity = pOutTorque->getAngularVel();
    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f", torqueTransfer, torqueTransfer);
}

double Clutch::getTorque ()
{
    return torqueTransfer;
}

double Clutch::getRevTorque ()
{
    return 0.0;
}

double Clutch::getAngularVel ()
{
    return rotationalVelocity;
}
