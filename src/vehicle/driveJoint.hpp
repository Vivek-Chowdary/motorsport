/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DRIVEJOINT_HPP
#   define DRIVEJOINT_HPP
#   include "ode/objects.h"
#   include "driveMass.hpp"
#   include "worldObject.hpp"
#   include "tools/xercesc_fwd.hpp"

class DriveMass;
class Pedal;

class DriveJoint : public WorldObject
{
  protected:
    // data
//    static int instancesCount;
    double inputAngularVel;
    double outputAngularVel;
    double ratio;
    double outputTorqueTransfer;
    double inputTorqueTransfer;
    double relAngle;       // delta angle (radians) between the connected masses
                           //     accounting for the gear ratio 
    double prevRelAngle;
    double relAngularVel;
    double prevRelAngularVel;
    DriveMass *inputDrive;
    DriveMass *outputDrive;

    bool enabled;
        
  public:
    // data
    DriveJoint (std::string identifier);
    ~DriveJoint ();
    void enable () { enabled = 1; } ;
    void disable () { enabled = 0; } ;
    bool isEnabled () { return enabled; } ;
     
    // physics
    double getOutputTorque ()               { return outputTorqueTransfer; } ;
    double getInputTorque ()                { return inputTorqueTransfer; } ;
    double getRelAngle ()                   { return relAngle; } ;
    void setOutputPointer (DriveMass *output)   { outputDrive = output; } ;
    void setInputPointer (DriveMass *input)     { inputDrive = input; } ;
};

class Clutch : public DriveJoint
{
  private:
    // data
    static int instancesCount;
    Pedal * clutchPedal;

    // physics
    double coeffStaticFriction;
    double coeffDynamicFriction;
    double maxTorqueTransfer;
    double lockedParam;
    bool locked;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    // data
    Clutch (XERCES_CPP_NAMESPACE::DOMNode * n);
    Clutch ();
    ~Clutch ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void setClutchPedal(Pedal * pedal);

    // physics
    void stepPhysics ();
};

class Gear : public DriveJoint
{
  private:
    // data
    static int instancesCount;

    // physics
    double ratio;
    double springConstant;
    double dampConstant;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

  public:
    // data
    Gear (XERCES_CPP_NAMESPACE::DOMNode * n);
    Gear ();
    ~Gear ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void stepPhysics ();
};

class LSD : public DriveJoint
{
  private:
    // data
    static int instancesCount;
    DriveMass *outputDrive2;

    // physics
    double outputsRelAngle;
    double prevOutputsRelAngle;
    double outputsRelAngularVel;
    double prevOutputsRelAngularVel;
    double ratio;
    double springConstant;
    double dampConstant;
    double limitedSlipClutchFriction;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

  public:
    // data
    LSD (XERCES_CPP_NAMESPACE::DOMNode * n);
    LSD ();
    ~LSD ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void setOutputPointer2 (DriveMass *output)   { outputDrive2 = output; } ;
    
    // physics
    void stepPhysics ();
};

#endif

