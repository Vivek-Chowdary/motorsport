/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Engine::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueLinearMultiplier = 0.0001;
    friction = 0.1;
    inertia = 1.0;
    angularVel = 0.0;
    prevAngularVel = 0.0;
    revAngularVel = 0.0;
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
                inertia = stod (attribute);
            }
            if (attribute == "engineFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the engine friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Engine::stopPhysics ()
{
}

void Engine::stepPhysics ()
{
    double dtoverJe;
    double engineTorque;
    
    prevAngularVel = angularVel;
  
    engineTorque = 0;
    engineTorque += torqueLinearMultiplier * SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_y)]->getValue();
    engineTorque -= torqueLinearMultiplier * SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_h)]->getValue();
    
//    angularVel = (engineTorque - pOutTorque->getRevTorque() - engineInertia*rotationalAcceleration)/friction;        
    dtoverJe=(SystemData::getSystemDataPointer()->physicsTimeStep/1000.0)/inertia;

//    rotationalAcceleration = (angularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000;
//    torque = engineTorque - inertia*rotationalAcceleration - friction*angularVel;
    angularVel = (dtoverJe*(engineTorque-outputJoint->getRevTorque())+prevAngularVel)/(1+(dtoverJe*friction));
    angularAcc = (angularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    revAngularVel = angularVel;
//    torqueTransfer = engineTorque - inertia*angularAcc - friction*angularVel;
//  rotationalAcceleration = (engineTorque-pOutTorque->getTorque()-(friction*angularVel))/inertia;

//    angularVel = prevAngularVel+rotationalAcceleration*SystemData::getSystemDataPointer()->physicsTimeStep/1000;

    log->format(LOG_TRACE, "engineTorque=%f(Nm) angAcc=%f engspeed=%f(rad/s)", engineTorque, angularAcc, angularVel);
}
