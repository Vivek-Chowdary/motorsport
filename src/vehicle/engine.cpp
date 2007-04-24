/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include <SDL/SDL_keysym.h>
#include "pedal.hpp"

pEngine Engine::create(XmlTag * tag)
{
    pEngine tmp(new Engine(tag));
    return tmp;
}

Engine::Engine (XmlTag * tag)
    :DriveMass("engine")
{
    constructFromTag(tag);
}
void Engine::readCustomDataTag(XmlTag * tag)
{
    inputAngularVel = 0.0;
    prevAngularVel = 0.0;
    outputAngularVel = 0.0;
    angularAcc = 0.0;
    log->__format (LOG_CCREATOR, "Starting to parse an engine node");
    torqueLinearMultiplier = stod (tag->getAttribute("torqueLinearMultiplier"));
    inertia = stod (tag->getAttribute("engineInertia"));
    friction = stod (tag->getAttribute("engineFriction"));
    angularVelLimit = stod (tag->getAttribute("angularVelLimit"));
}

Engine::~Engine ()
{
}


void Engine::setGasPedal (pPedal pedal)
{
    gasPedal = pedal;
}

void Engine::stepPhysics ()
{
    WorldObject::stepPhysics();
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

    dt = System::get()->getDesiredPhysicsTimestep();
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
    //log->__format(LOG_DEVELOPER, "engineTorque=%f(Nm) angAcc=%f engspeed=%f(rad/s)", engineTorque, angularAcc, inputAngularVel);
    log->__format(LOG_WARNING, "EngineTorque: %f Gas: %f", engineTorque, gas);
    telemetryTorque = engineTorque;
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Engine::getOutputTorque()
{
    return telemetryTorque;
}
