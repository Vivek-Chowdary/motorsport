/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef ENGINE_HPP
#   define ENGINE_HPP
#   include "driveMass.hpp"
#   include "driveJoint.hpp"
#   include "tools/xercesc_fwd.hpp"

class Pedal;

class Engine : public DriveMass
{
  private:
    double torqueLinearMultiplier;
    double telemetryTorque;
    double angularVelLimit;
    Pedal * gasPedal;
  public:
    Engine (WorldObject * container, XmlTag * tag);
    ~Engine ();
    void setGasPedal (Pedal * pedal);
    void stepPhysics ();
    double getOutputTorque();
};
#endif
