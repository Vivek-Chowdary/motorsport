/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
