/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "finalDrive.hpp"
#include "world.hpp"
#include "system.hpp"
#include "ode/ode.h"
#include "SDL/SDL_keysym.h"

pFinalDrive FinalDrive::create(XmlTag * tag)
{
    pFinalDrive tmp(new FinalDrive(tag));
    return tmp;
}

FinalDrive::FinalDrive (XmlTag * tag)
    :DriveMass("finaldrive")
{
    constructFromTag(tag);
}
void FinalDrive::readCustomDataTag(XmlTag * tag)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    prevAngularVel = 0.0;
    angularAcc = 0.0;
    inertia = 0.1;
    friction = 0.001;
    
    friction = stod (tag->getAttribute("diffFriction"));
    inertia = stod (tag->getAttribute("diffInertia"));
    finalDriveRatio = stod (tag->getAttribute("ratio"));
}

FinalDrive::~FinalDrive ()
{
}

void FinalDrive::stepPhysics ()
{
    WorldObject::stepPhysics();
    double dt;
    double torqueSum;
    
    dt = System::get()->getDesiredPhysicsTimestep();

    prevAngularVel = inputAngularVel;

//    inputTorqueTransfer += inputJoint->getOutputTorque();
//    outputTorqueTransfer += outputJoint->getInputTorque()/finalDriveRatio;

    torqueSum = inputTorqueTransfer + outputTorqueTransfer/finalDriveRatio;
    
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);

    outputAngularVel = inputAngularVel/finalDriveRatio;

/*    double dtoverJe;

    prevAngularVel = inputAngularVel;

    dtoverJe=(System::get()->getDesiredPhysicsTimestep())/inertia;

    inputTorqueTransfer = inputJoint->getOutputTorque();
    outputTorqueTransfer = (outputJoint->getInputTorque() + outputJoint2->getInputTorque())/finalDriveRatio;

    inputAngularVel = (dtoverJe*(outputTorqueTransfer+inputTorqueTransfer)+prevAngularVel)/(1+(dtoverJe*friction));

    // calculate angular acceleration (for reference)
    angularAcc = (inputAngularVel-prevAngularVel)/System::get()->getDesiredPhysicsTimestep();
    outputAngularVel = inputAngularVel/finalDriveRatio;
*/
    //log->__format(LOG_DEVELOPER, "angVel=%f angAcc=%f torque=%f", inputAngularVel, angularAcc, outputTorqueTransfer);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}
