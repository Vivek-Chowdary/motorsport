/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
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

// Forward declarations
SHARED_PTR(Pedal, pPedal, wPedal);
SHARED_PTR(DriveMass, pDriveMass, wDriveMass);

SHARED_PTR(DriveJoint, pDriveJoint, wDriveJoint);
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
    pDriveMass inputDrive;
    pDriveMass outputDrive;
    bool enabled;
  public:
    DriveJoint (std::string name);
    ~DriveJoint ();
    void enable () { enabled = 1; } ;
    void disable () { enabled = 0; } ;
    bool isEnabled () { return enabled; } ;
    double getOutputTorque ()               { return outputTorqueTransfer; } ;
    double getInputTorque ()                { return inputTorqueTransfer; } ;
    double getRelAngle ()                   { return relAngle; } ;
    void setOutputPointer (pDriveMass output)   { outputDrive = output; } ;
    void setInputPointer (pDriveMass input)     { inputDrive = input; } ;
};

SHARED_PTR(Clutch, pClutch, wClutch);
class Clutch : public DriveJoint
{
  private:
    pPedal clutchPedal;
    double coeffStaticFriction;
    double coeffDynamicFriction;
    double maxTorqueTransfer;
    double lockedParam;
    bool locked;
    Clutch (XmlTag * tag);
    Clutch ();
    void readCustomDataTag(XmlTag * tag);
  public:
    static pClutch create (XmlTag * tag);
    static pClutch create ();
    ~Clutch ();
    void setClutchPedal(pPedal pedal);
    void stepPhysics ();
};

SHARED_PTR(Gear, pGear, wGear);
class Gear : public DriveJoint
{
  private:
    double ratio;
    double springConstant;
    double dampConstant;
    void startPhysics (XmlTag * tag);
    Gear (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pGear create (XmlTag * tag);
    static pGear create ();
    ~Gear ();
    void processXmlRootNode (XmlTag * tag);
    void stepPhysics ();
};

SHARED_PTR(LSD, pLSD, wLSD);
class LSD : public DriveJoint
{
  private:
    pDriveMass outputDrive2;
    double outputsRelAngle;
    double prevOutputsRelAngle;
    double outputsRelAngularVel;
    double prevOutputsRelAngularVel;
    double ratio;
    double springConstant;
    double dampConstant;
    double limitedSlipClutchFriction;
    void startPhysics (XmlTag * tag);
    LSD (XmlTag * tag);
    LSD ();
    void readCustomDataTag(XmlTag * tag);
  public:
    static pLSD create (XmlTag * tag);
    static pLSD create ();
    ~LSD ();
    void processXmlRootNode (XmlTag * tag);
    void setOutputPointer2 (pDriveMass output)   { outputDrive2 = output; } ;
    void stepPhysics ();
};

#endif

