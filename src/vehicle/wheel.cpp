/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
    inputAngularVel = dJointGetHinge2Angle2Rate (suspJointID)*powered*-1;
    //inputAngularVel = dJointGetHingeAngleRate (suspJointID); // old kart suspension code

    // calculate angular acceleration      
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();

    // tire rolling resistance
    //inputTorqueTransfer -= 0.1*inputAngularVel;

    // first, get the axis of the suspension
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (suspJointID, odeTAxis);
    Vector3d tAxis (odeTAxis);

    // accumulate torques:
    double brake = brakePedal->getNormalizedAngle();
    if (!userDriver) brake = 1;
    if (inputAngularVel > 0) brake *= -1;
    double maxBrakeTorque = 1250;
    double brakeTorque = brake * maxBrakeTorque;
    int initialSign = inputTorqueTransfer>0?1:-1;
    inputTorqueTransfer += brakeTorque;
    int finalSign = inputTorqueTransfer>0?1:-1;
    if (initialSign != finalSign)
    {
        inputTorqueTransfer = 0;
    }

    // then, scale it by desired torque in the desired direction of the axis
    tAxis.scalarMultiply (inputTorqueTransfer * powered);


    ///TODO TODO TODO TODO TODO
    // use feedback thingy in ODE in order to find out torques
    ///////////////////////////////////////////////////////////////////////
    // get accumulated torque
    const dReal * odeTorque = dBodyGetTorque (baseID);
    Vector3d accumulatedTorque (odeTorque);
    // show acc torque
    log->__format(LOG_DEVELOPER, "Accumulated torque = (%f, %f, %f)", accumulatedTorque.x, accumulatedTorque.y, accumulatedTorque.z);
    // compare it to our torque
    ///////////////////////////////////////////////////////////////////////
    
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
