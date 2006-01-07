/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "world.hpp"
#include "ode/ode.h"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"
#include "vehicle.hpp"
#include "suspension.hpp"

Wheel::Wheel (WorldObject * container, XmlTag * tag)
    :DriveMass(container, "wheel")
{
    log->__format (LOG_DEVELOPER, "Starting to parse a wheel node");
    VehicleWheelOdeObjectData data;
    OgreObjectData ogreData;
    powered = 0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    prevAngularVel = 0.0;
    angularAcc = 0.0;
    inputTorqueTransfer = 0.0;
    outputTorqueTransfer = 0.0;
    inertia = 1.0;
    friction = 0.1;
    if (tag->getName() == "wheel")
    {
        setName (     tag->getAttribute("name"));
        data.radius = stod(tag->getAttribute("radius"));
        data.width = stod(tag->getAttribute("width"));
        data.mass = stod(tag->getAttribute("mass"));
        powered = stod(tag->getAttribute("powered"));
        ogreData.meshPath = tag->getAttribute("mesh");
    }
     
    odeObjects[getId()] = new OdeObject(this, data, getId());
    ogreData.meshPath = getPath() + ogreData.meshPath;
    OgreObject * ogreObject = new OgreObject(this, ogreData, getId());
    ogreObjects[getId()] = ogreObject;
    ogreObjects.begin()->second->setOdeReference(getMainOdeObject());
}

Wheel::~Wheel ()
{
}

void Wheel::setBrakePedal (Pedal * pedal)
{
    brakePedal = pedal;
}

void Wheel::setUserDriver ()
{
    userDriver = true;
}
void Wheel::stepPhysics ()
{
    prevAngularVel = inputAngularVel;

    // use hinge's angular rate as angular velocity of wheel (rad/s)
    inputAngularVel = dJointGetHinge2Angle2Rate (baseSuspension->getJointID())*powered*-1;
    //inputAngularVel = dJointGetHingeAngleRate (baseSuspension->getJointID()); // old kart suspension code

    // calculate angular acceleration      
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();

    // tire rolling resistance
    //inputTorqueTransfer -= 0.1*inputAngularVel;

    // first, get the axis of the suspension
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (baseSuspension->getJointID(), odeTAxis);
    Vector3d tAxis (odeTAxis);

    // accumulate torques:
    double brake = brakePedal->getNormalizedAngle();
    if (!userDriver) brake = 1;
    if (inputAngularVel > 0) brake *= -1;
    double maxBrakeTorque = 1250;
    double brakeTorque = brake * maxBrakeTorque;
    if ((-brakeTorque) > inputTorqueTransfer)
    {
        dJointSetHinge2Param(baseSuspension->getJointID(), dParamVel, 0);
    }
    inputTorqueTransfer += brakeTorque;

    // then, scale it by desired torque in the desired direction of the axis
    tAxis.scalarMultiply (inputTorqueTransfer * powered);

    // finally, apply the torques
    dBodyAddTorque (getMainOdeObject()->getBodyID(), tAxis.x, tAxis.y, tAxis.z);

    log->__format(LOG_DEVELOPER, "%s:angVel=%f angAcc=%f torque=%f powered=%f axis=(%f,%f,%f)",getName().c_str(), inputAngularVel, angularAcc, inputTorqueTransfer, powered, tAxis.x, tAxis.y, tAxis.z);
    
    // zero the accumulators
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Wheel::getInputAngularVel()
{
    return inputAngularVel;
}
