/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DRIVEJOINT_HPP
#define DRIVEJOINT_HPP
#include "ode/objects.h"
#include "driveMass.hpp"
#include "worldObject.hpp"
#include "tools/xercesc_fwd.hpp"

class DriveMass;
class Pedal;

class DriveJoint : public WorldObject
{
  protected:
    double inputAngularVel;
    double outputAngularVel;
    double ratio;
    double outputTorqueTransfer;
    double inputTorqueTransfer;
    double relAngle;       // delta angle (radians) between the connected masses accounting for the gear ratio 
    double prevRelAngle;
    double relAngularVel;
    double prevRelAngularVel;
    DriveMass *inputDrive;
    DriveMass *outputDrive;
    bool enabled;
  public:
    DriveJoint (WorldObject * container, std::string name);
    ~DriveJoint ();
    void enable () { enabled = 1; } ;
    void disable () { enabled = 0; } ;
    bool isEnabled () { return enabled; } ;
    double getOutputTorque ()               { return outputTorqueTransfer; } ;
    double getInputTorque ()                { return inputTorqueTransfer; } ;
    double getRelAngle ()                   { return relAngle; } ;
    void setOutputPointer (DriveMass *output)   { outputDrive = output; } ;
    void setInputPointer (DriveMass *input)     { inputDrive = input; } ;
};

class Clutch : public DriveJoint
{
  private:
    Pedal * clutchPedal;
    double coeffStaticFriction;
    double coeffDynamicFriction;
    double maxTorqueTransfer;
    double lockedParam;
    bool locked;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    Clutch (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    Clutch (WorldObject * container, std::string name);
    ~Clutch ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void setClutchPedal(Pedal * pedal);
    void stepPhysics ();
};

class Gear : public DriveJoint
{
  private:
    double ratio;
    double springConstant;
    double dampConstant;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    Gear (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    Gear (WorldObject * container, std::string name);
    ~Gear ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
};

class LSD : public DriveJoint
{
  private:
    DriveMass *outputDrive2;
    double outputsRelAngle;
    double prevOutputsRelAngle;
    double outputsRelAngularVel;
    double prevOutputsRelAngularVel;
    double ratio;
    double springConstant;
    double dampConstant;
    double limitedSlipClutchFriction;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    LSD (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    LSD (WorldObject * container, std::string name);
    ~LSD ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void setOutputPointer2 (DriveMass *output)   { outputDrive2 = output; } ;
    void stepPhysics ();
};

#endif

