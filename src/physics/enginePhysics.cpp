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

#include "engine.hpp"
#include "clutch.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Engine::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueLinearMultiplier = 0.0;
    engineFriction = 0.1;
    engineInertia = 1.0;
    rotationalVelocity = 0.0;
    rotationalAcceleration = 0.0;
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
            if (attribute == "torqueLinearMultiplier")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the engine torque linear multiplier: %s", attribute.c_str() );
                torqueLinearMultiplier = stod (attribute);
            }
            if (attribute == "engineInertia")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the engine inertia: %s", attribute.c_str() );
                engineInertia = stod (attribute);
            }
            if (attribute == "engineFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the engine friction: %s", attribute.c_str() );
                engineFriction = stod (attribute);
            }

            attribute.clear();
        }
    }
    torque = 0.0;
}

void Engine::stopPhysics ()
{
}

void Engine::stepPhysics ()
{
//    prevRotationalVelocity = rotationalVelocity;
    engineTorque = 0;
    engineTorque += torqueLinearMultiplier * SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_y)]->getValue();
    engineTorque -= torqueLinearMultiplier * SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_h)]->getValue();
    torque = engineTorque;
//    torque = engineTorque - engineInertia*rotationalAcceleration - engineFriction*rotationalVelocity;
    rotationalVelocity = pOutTorque->getAngularVel();
//    rotationalAcceleration = (engineTorque-pOutTorque->getTorque()-(engineFriction*rotationalVelocity))/engineInertia;
//    rotationalVelocity = prevRotationalVelocity+rotationalAcceleration*SystemData::getSystemDataPointer()->physicsTimeStep/1000;
    log->format(LOG_TRACE, "torqueMultiplier=%f inputAxis=%f outputTorque=%f(Nm) engspeed=%f(rad/s)", torqueLinearMultiplier, SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_y)]->getValue(), torque, rotationalVelocity);
}
double Engine::getTorque ()
{
    return torque;
}
double Engine::getRotationalVelocity ()
{
    return rotationalVelocity;
}
