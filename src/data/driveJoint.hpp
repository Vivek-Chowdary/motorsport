/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DRIVEJOINT_HPP
#   define DRIVEJOINT_HPP
#   include "ode/objects.h"
#   include "drive.hpp"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"

class Drive;

class DriveJoint : public WorldObject
{
  protected:
    // data
//    static int instancesCount;
    double angularVel;
    double revAngularVel;
    double ratio;
    double torqueTransfer;
    double revTorqueTransfer;
    Drive *inputDrive;
    Drive *outputDrive;
    
    // physics
    virtual void stopPhysics () = 0;
  public:
    // data
    DriveJoint () { } ;
    ~DriveJoint () { } ;
    virtual void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;

    // physics
    virtual void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n) = 0;
    virtual void stepPhysics () = 0;
    double getTorque ()                     { return torqueTransfer; } ;
    double getRevTorque ()                  { return revTorqueTransfer; } ;
    void setOutputPointer (Drive *output)   { outputDrive = output; } ;
    void setInputPointer (Drive *input)     { inputDrive = input; } ;
};

#endif

