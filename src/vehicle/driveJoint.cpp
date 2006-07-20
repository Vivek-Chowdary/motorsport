/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "driveJoint.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"

DriveJoint::DriveJoint (std::string name)
    :WorldObject(name)
{
}
DriveJoint::~DriveJoint ()
{
}
pClutch Clutch::create (XmlTag * tag)
{
    pClutch tmp(new Clutch(tag));
    return tmp;
}

pClutch Clutch::create ()
{
    pClutch tmp(new Clutch());
    return tmp;
}

void Clutch::readCustomDataTag(XmlTag * tag)
{
    coeffStaticFriction = stod(tag->getAttribute("coeffStaticFriction"));
    coeffDynamicFriction = stod(tag->getAttribute("coeffDynamicFriction"));
    maxTorqueTransfer = stod(tag->getAttribute("maxTorqueTransfer"));
}
Clutch::Clutch (XmlTag * tag)
    :DriveJoint("clutch")
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    lockedParam = 1;
    constructFromTag(tag);
}

Clutch::Clutch ()
    :DriveJoint("clutch")
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    maxTorqueTransfer = 1000;
    lockedParam = 1;
}

Clutch::~Clutch ()
{
}

void Clutch::setClutchPedal(pPedal pedal)
{
    clutchPedal = pedal;
}


pGear Gear::create (XmlTag * tag)
{
    pGear tmp (new Gear(tag));
    return tmp;
}
pGear Gear::create ()
{
    pGear tmp (new Gear(NULL));
    return tmp;
}

Gear::Gear (XmlTag * tag)
    :DriveJoint("gear")
{
/*/
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    ratio = 1.0;
    springConstant = 1000;
    relAngle = 0.0;
//*/
    ratio = 1.0;
    springConstant = 300.0;
    dampConstant = 8;
    relAngle = 0.0;
    prevRelAngle = 0.0;
    relAngularVel = 0.0;
    prevRelAngularVel = 0.0;
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
//*/

    constructFromTag(tag);
}
void Gear::readCustomDataTag(XmlTag * tag)
{
}

Gear::~Gear ()
{
}

pLSD LSD::create (XmlTag * tag)
{
    pLSD tmp (new LSD(tag));
    return tmp;
}
pLSD LSD::create ()
{
    pLSD tmp (new LSD());
    return tmp;
}
LSD::LSD (XmlTag * tag)
    :DriveJoint("lsd")
{
    constructFromTag(tag);
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    ratio = 1.0;
    springConstant = 1000;
    relAngle = 0.0;
}

void LSD::readCustomDataTag(XmlTag * tag)
{
}
LSD::LSD ()
    :DriveJoint("lsd")
{
    constructFromTag(NULL);
    ratio = 1.0;
    springConstant = 300.0;
    dampConstant = 8;
    relAngle = 0.0;
    prevRelAngle = 0.0;
    relAngularVel = 0.0;
    prevRelAngularVel = 0.0;
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    outputsRelAngle = 0.0;
    prevOutputsRelAngle = 0.0;
    outputsRelAngularVel = 0.0;
    prevOutputsRelAngularVel = 0.0;
}

LSD::~LSD ()
{
}

void Clutch::stepPhysics ()
{
    WorldObject::stepPhysics();
    if(enabled) {
        double clutch = clutchPedal->getNormalizedAngle();
        if(clutch) {
            outputTorqueTransfer = 0*(inputDrive->getOutputAngularVel()-outputDrive->getInputAngularVel());
            inputTorqueTransfer = outputTorqueTransfer*-1;
        } else
        {
            outputTorqueTransfer = lockedParam*(inputDrive->getOutputAngularVel()-outputDrive->getInputAngularVel());
            //log->__format(LOG_DEVELOPER, "torqueTransfer=%f", outputTorqueTransfer);

            if(outputTorqueTransfer > maxTorqueTransfer)
            {
                outputTorqueTransfer = maxTorqueTransfer;
            } else if(outputTorqueTransfer < -1*maxTorqueTransfer)
            {
                outputTorqueTransfer = -1 * maxTorqueTransfer;
            }
            inputTorqueTransfer = outputTorqueTransfer*-1;
        }       

        inputDrive->addOutputTorque(inputTorqueTransfer);
        outputDrive->addInputTorque(outputTorqueTransfer);   

        // rotationalVelocity = pOutTorque->getInputAngularVel();
        //log->__format(LOG_DEVELOPER, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer,inputDrive->getOutputAngularVel(),outputDrive->getInputAngularVel());
    }
}

void Gear::stepPhysics ()
{ 
    WorldObject::stepPhysics();
    if(enabled) {
        double dt;
        dt = System::get()->getDesiredPhysicsTimestep();
        prevRelAngle = relAngle;
        prevRelAngularVel = relAngularVel;

        relAngularVel = inputDrive->getOutputAngularVel()/ratio - outputDrive->getInputAngularVel();

        // trapezoidal integration
        relAngle = prevRelAngle + dt / 2 * (prevRelAngularVel + relAngularVel); 

        //    relAngle = relAngle + inputDrive->getOutputAngularVel()*dt/ratio - outputDrive->getInputAngularVel()*dt;

        outputTorqueTransfer = springConstant*relAngle+dampConstant*relAngularVel;

        inputTorqueTransfer = -1*outputTorqueTransfer/ratio;

        inputDrive->addOutputTorque(inputTorqueTransfer);
        outputDrive->addInputTorque(outputTorqueTransfer);   

        //log->__format(LOG_DEVELOPER, "inTorque=%f outTorque=%f relVel=%f relAngle=%f", inputTorqueTransfer, outputTorqueTransfer,relAngularVel,relAngle);
    }
    else {
        relAngle = 0;
    }
}

void LSD::stepPhysics ()
{
    WorldObject::stepPhysics();
    if(enabled) {
        double dt;
        dt = System::get()->getDesiredPhysicsTimestep();
        prevRelAngle = relAngle;
        prevRelAngularVel = relAngularVel;

        relAngularVel = inputDrive->getOutputAngularVel()/ratio - (outputDrive->getInputAngularVel()+outputDrive2->getInputAngularVel())/2;

        // trapezoidal integration
        relAngle = prevRelAngle + dt / 2 * (prevRelAngularVel + relAngularVel); 

        //    relAngle = relAngle + inputDrive->getOutputAngularVel()*dt/ratio - outputDrive->getInputAngularVel()*dt;

        outputTorqueTransfer = springConstant*relAngle+dampConstant*relAngularVel;

        inputTorqueTransfer = -1*outputTorqueTransfer/ratio;

        inputDrive->addOutputTorque(inputTorqueTransfer);
        outputDrive->addInputTorque(outputTorqueTransfer/2);
        outputDrive2->addInputTorque(outputTorqueTransfer/2);   

        //log->__format(LOG_DEVELOPER, "inTorque=%f outTorque=%f relVel=%f relAngle=%f", inputTorqueTransfer, outputTorqueTransfer,relAngularVel,relAngle);
    }
    else {
        relAngle = 0;
    }
}
