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

#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Gearbox::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueTransfer = 0.0;
    gearboxFriction = 0.0;
    gearboxInertia = 1.0;
    angularVel = 0.0;
    angularAcc = 0.0;
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
            if (attribute == "gearboxFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Friction: %s", attribute.c_str() );
                gearboxFriction = stod (attribute);
            }
            if (attribute == "gearboxInertia")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Inertia: %s", attribute.c_str() );
                gearboxInertia = stod (attribute);
            }
            if (attribute == "gear1Ratio")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gear 1 Ratio: %s", attribute.c_str() );
                gearRatio = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Gearbox::stopPhysics ()
{
}

void Gearbox::stepPhysics ()
{
    torqueTransfer = inputClass->getTorque()*gearRatio;
    angularVel = (pOutWheel1->getAngularVel()+pOutWheel2->getAngularVel())/2/gearRatio;
    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputClass->getTorque(), torqueTransfer,inputClass->getAngularVel(), angularVel);
}
double Gearbox::getTorque ()
{
    return torqueTransfer;
}

double Gearbox::getRevTorque ()
{
    return 0.0;
}

double Gearbox::getAngularVel ()
{
    return angularVel;
}

