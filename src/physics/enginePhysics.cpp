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
    inputAngularVel = 0.0;
    prevAngularVel = 0.0;
    outputAngularVel = 0.0;
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
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the engine torque linear multiplier: %s", attribute.c_str() );
                torqueLinearMultiplier = stod (attribute);
            }
            if (attribute == "engineInertia")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the engine inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "engineFriction")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the engine friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
        }
    }
}

void Engine::stopPhysics ()
{
}

void Engine::stepPhysics ()
{
//    double dtoverJe;
    double engineTorque;
    
    double gas = 0;
    // TODO: move this value inversion to axis filters!
    if ( userDriver )
    {
        gas = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,2)]->getValue();
        if (gas == 0) {
            gas = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP9)]->getValue() * 3 / 3;
            if (gas == 0) {
                gas = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP6)]->getValue() * 2 / 3;
                if (gas == 0) {
                    gas = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP3)]->getValue() * 1 / 3;
                    if (gas == 0) {
                        gas = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue() * 3 / 3;
        }   }   }   }
    }
    double brake = 0;
    // TODO: move this value inversion to axis filters!
    if ( userDriver )
    {
        brake = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,1)]->getValue();
        if (brake == 0) {
            brake = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP8)]->getValue() * 3 / 3;
            if (brake == 0) {
                brake = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP5)]->getValue() * 2 / 3;
                if (brake == 0) {
                    brake = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP2)]->getValue() * 1 / 3;
                    if (brake == 0) {
                        brake = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue() * 3 / 3;
        }   }   }   }
    }
    engineTorque = 0;
    engineTorque += torqueLinearMultiplier * gas;
    engineTorque -= torqueLinearMultiplier * brake;
    
    double dt;
    double torqueSum;
    
    dt = SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;

    prevAngularVel = inputAngularVel;

//    inputTorqueTransfer = inputJoint->getOutputTorque();
//    outputTorqueTransfer += outputJoint->getInputTorque();

    torqueSum = outputTorqueTransfer + engineTorque;
    
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);

    outputAngularVel = inputAngularVel;


/*    dtoverJe=(SystemData::getSystemDataPointer()->physicsTimeStep/1000.0)/inertia;

    inputAngularVel = (dtoverJe*(engineTorque+outputJoint->getInputTorque())+prevAngularVel)/(1+(dtoverJe*friction));
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    outputAngularVel = inputAngularVel;
*/
    log->format(LOG_DEVELOPER, "engineTorque=%f(Nm) angAcc=%f engspeed=%f(rad/s)", engineTorque, angularAcc, inputAngularVel);
    telemetryTorque = engineTorque;
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Engine::getOutputTorque()
{
    return telemetryTorque;
}
