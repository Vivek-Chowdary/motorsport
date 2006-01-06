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
#include "driveMass.hpp"
#include "worldObject.hpp"

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
  public:
    Clutch (WorldObject * container, XmlTag * tag);
    Clutch (WorldObject * container);
    ~Clutch ();
    void setClutchPedal(Pedal * pedal);
    void stepPhysics ();
};

class Gear : public DriveJoint
{
  private:
    double ratio;
    double springConstant;
    double dampConstant;
    void startPhysics (XmlTag * tag);
  public:
    Gear (WorldObject * container, XmlTag * tag);
    Gear (WorldObject * container);
    ~Gear ();
    void processXmlRootNode (XmlTag * tag);
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
    void startPhysics (XmlTag * tag);
  public:
    LSD (WorldObject * container, XmlTag * tag);
    LSD (WorldObject * container);
    ~LSD ();
    void processXmlRootNode (XmlTag * tag);
    void setOutputPointer2 (DriveMass *output)   { outputDrive2 = output; } ;
    void stepPhysics ();
};

#endif

