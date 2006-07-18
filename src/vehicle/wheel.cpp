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

pWheel Wheel::create(XmlTag * tag)
{
    pWheel tmp(new Wheel(tag));
    return tmp;
}

Wheel::Wheel (XmlTag * tag)
    :DriveMass("wheel")
{
    log->__format (LOG_DEVELOPER, "Starting to parse a wheel node");
    pWheelOdeData data(new WheelOdeData);
    pOgreObjectData ogreData(new OgreObjectData);
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
        data->radius = stod(tag->getAttribute("radius"));
        data->width = stod(tag->getAttribute("width"));
        data->mass = stod(tag->getAttribute("mass"));
        powered = stod(tag->getAttribute("powered"));
        //create main mesh
        ogreData->meshPath = getPath() + ogreData->meshPath;
        pOgreObject ogreObject (new OgreObject(this, ogreData, getId(), false));
        ogreObjects[ogreObject->getId()] = ogreObject;
        odeObjects[getId()] = pOdeObject(new OdeObject(this, data, getId()));
        ogreObjects[ogreObject->getId()]->setOdeReference(getMainOdeObject());
        //create child meshes
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        { 
            if (t->getName() == "mesh")
            {
                pOgreObjectData childData(new OgreObjectData);
                childData->meshPath = getPath() + t->getAttribute("file");
                Vector3d posDiff (t->getAttribute("position"));
                Vector3d scale (t->getAttribute("scale"));
                Quaternion rotDiff (t->getAttribute("rotation"));
                pOgreObject ogreChild (new OgreObject(this, childData, getId()));
                ogreObjects[ogreChild->getId()] = ogreChild;
                ogreChild->setOgreReference(ogreObjects[ogreObject->getId()], rotDiff, posDiff, scale);
            }
        }
    }
}

Wheel::~Wheel ()
{
}

void Wheel::setBrakePedal (pPedal pedal)
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
    inputAngularVel = baseSuspension->getRate()*powered*-1;
    //inputAngularVel = dJointGetHingeAngleRate (baseSuspension->getJointID()); // old kart suspension code

    // calculate angular acceleration      
    angularAcc = (inputAngularVel-prevAngularVel)/System::get()->getDesiredPhysicsTimestep();

    // tire rolling resistance
    //inputTorqueTransfer -= 0.1*inputAngularVel;

    // first, get the axis of the suspension
    Vector3d tAxis = baseSuspension->getAxis();
    
    // accumulate torques:
    double brake = brakePedal->getNormalizedAngle();
    if (!userDriver) brake = 1;
    if (inputAngularVel > 0) brake *= -1;
    double maxBrakeTorque = 1250;
    double brakeTorque = brake * maxBrakeTorque;
    if ((-brakeTorque) > inputTorqueTransfer)
    {
        baseSuspension->setVelocity(0.0);
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
