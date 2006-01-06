/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DRIVE_HPP
#define DRIVE_HPP
#include "ode/objects.h"
#include "driveJoint.hpp"
#include "worldObject.hpp"

class DriveJoint;

class DriveMass : public WorldObject
{
  protected:
    double inertia;
    double friction;
    double inputAngularVel;
    double prevAngularVel;
    double angularAcc;
    double outputAngularVel;
    double outputTorqueTransfer;
    double inputTorqueTransfer;
  public:
    DriveMass (WorldObject * container, std::string name);
    ~DriveMass ();
    double getInputTorque ()             { return inputTorqueTransfer; } ;
    double getOutputTorque ()            { return outputTorqueTransfer; } ;
    double getInputAngularVel ()         { return inputAngularVel; } ;
    double getOutputAngularVel ()        { return outputAngularVel; } ;
    void addInputTorque (double inputTorque)    { inputTorqueTransfer += inputTorque; } ;
    void addOutputTorque (double outputTorque)  { outputTorqueTransfer += outputTorque; } ;
};

#endif
