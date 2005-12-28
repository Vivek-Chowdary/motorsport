/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"


class Pedal;

Engine::Engine (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n)
    :DriveMass(container, name)
{
    log->__format (LOG_CCREATOR, "Starting to parse an engine node");
    processXmlRootNode (n);
}

Engine::~Engine ()
{
}


void Engine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Engine::setGasPedal (Pedal * pedal)
{
    gasPedal = pedal;
}

void Engine::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueLinearMultiplier = 0.0001;
    friction = 0.1;
    inertia = 1.0;
    inputAngularVel = 0.0;
    prevAngularVel = 0.0;
    outputAngularVel = 0.0;
    angularAcc = 0.0;
    angularVelLimit = 700;
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
                log->__format (LOG_CCREATOR, "Found the engine torque linear multiplier: %s", attribute.c_str() );
                torqueLinearMultiplier = stod (attribute);
            }
            if (attribute == "engineInertia")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the engine inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "engineFriction")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the engine friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            if (attribute == "angularVelLimit")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the engine angular velocity limit: %s", attribute.c_str() );
                angularVelLimit = stod (attribute);
            }
        }
    }
}

void Engine::stepPhysics ()
{
//    double dtoverJe;
    double engineTorque;
    double gas = gasPedal->getNormalizedAngle();

    if (inputAngularVel > angularVelLimit)
    {
        engineTorque = 0;
    } else {
        engineTorque = ((torqueLinearMultiplier/2) + (inputAngularVel * (torqueLinearMultiplier/2) / angularVelLimit)) * gas;
    }
    
    double dt;
    double torqueSum;
    
    dt = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    prevAngularVel = inputAngularVel;
//    inputTorqueTransfer = inputJoint->getOutputTorque();
//    outputTorqueTransfer += outputJoint->getInputTorque();
    torqueSum = outputTorqueTransfer + engineTorque;
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);
    outputAngularVel = inputAngularVel;
/*    dtoverJe=dt/inertia;
    inputAngularVel = (dtoverJe*(engineTorque+outputJoint->getInputTorque())+prevAngularVel)/(1+(dtoverJe*friction));
    angularAcc = (inputAngularVel-prevAngularVel)/ dt;
    outputAngularVel = inputAngularVel; */
    log->__format(LOG_DEVELOPER, "engineTorque=%f(Nm) angAcc=%f engspeed=%f(rad/s)", engineTorque, angularAcc, inputAngularVel);
    telemetryTorque = engineTorque;
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Engine::getOutputTorque()
{
    return telemetryTorque;
}
