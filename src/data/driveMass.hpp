/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DRIVE_HPP
#   define DRIVE_HPP
#   include "ode/objects.h"
#   include "driveJoint.hpp"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"

class DriveJoint;

class DriveMass : public WorldObject
{
  protected:
//    static int instancesCount;
    double inertia;
    double friction;
    double inputAngularVel;
    double prevAngularVel;
    double angularAcc;
    double outputAngularVel;
    double outputTorqueTransfer;
    double inputTorqueTransfer;
    
  public:

    // data
    DriveMass () 
      : inertia(0.0),
	friction(0.0),
	inputAngularVel(0.0),
	prevAngularVel(0.0),
	angularAcc(0.0),
	outputAngularVel(0.0),
	outputTorqueTransfer(0.0),
	inputTorqueTransfer(0.0)
  { };
    ~DriveMass () { } ;
    virtual void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;

    // physics
    virtual void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;
    virtual void stepPhysics () = 0;
    virtual void stopPhysics () = 0;
    double getInputTorque ()             { return inputTorqueTransfer; } ;
    double getOutputTorque ()            { return outputTorqueTransfer; } ;
    double getInputAngularVel ()         { return inputAngularVel; } ;
    double getOutputAngularVel ()        { return outputAngularVel; } ;
    void addInputTorque (double inputTorque)    { inputTorqueTransfer += inputTorque; } ;
    void addOutputTorque (double outputTorque)  { outputTorqueTransfer += outputTorque; } ;
};

#endif
