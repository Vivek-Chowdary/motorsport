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

class Drive : public WorldObject
{
  protected:
//    static int instancesCount;
    double inertia;
    double friction;
    double angularVel;
    double prevAngularVel;
    double angularAcc;
    double revAngularVel;
    double torqueTransfer;
    double revTorqueTransfer;
    DriveJoint *inputJoint;
    DriveJoint *outputJoint;
    dBodyID bodyID;
    
  public:

    // data
    Drive () { } ;
    ~Drive () { } ;
    virtual void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;

    // physics
    virtual void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;
    virtual void stepPhysics () = 0;
    virtual void stopPhysics () = 0;
    dBodyID getBodyID ()                    { return bodyID; } ;
    double getTorque ()                     { return torqueTransfer; } ;
    double getRevTorque ()                  { return revTorqueTransfer; } ;
    double getAngularVel ()                 { return angularVel; } ;
    double getRevAngularVel ()              { return revAngularVel; } ;
    void setOutputPointer (DriveJoint *output)   { outputJoint = output; } ;
    void setInputPointer (DriveJoint *input)     { inputJoint = input; } ;
};

#endif

