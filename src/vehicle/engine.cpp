/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"


class Pedal;

Engine::Engine (WorldObject * container, XmlTag * tag)
    :DriveMass(container, "engine")
{
    inputAngularVel = 0.0;
    prevAngularVel = 0.0;
    outputAngularVel = 0.0;
    angularAcc = 0.0;
    log->__format (LOG_CCREATOR, "Starting to parse an engine node");
    if (tag->getName() == "engine")
    {
        setName (tag->getAttribute("name"));
        torqueLinearMultiplier = stod (tag->getAttribute("torqueLinearMultiplier"));
        inertia = stod (tag->getAttribute("engineInertia"));
        friction = stod (tag->getAttribute("engineFriction"));
        angularVelLimit = stod (tag->getAttribute("angularVelLimit"));

    }
}

Engine::~Engine ()
{
}


void Engine::setGasPedal (Pedal * pedal)
{
    gasPedal = pedal;
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
