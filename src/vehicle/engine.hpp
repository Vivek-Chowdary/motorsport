/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef ENGINE_HPP
#define ENGINE_HPP
#include "driveMass.hpp"
#include "driveJoint.hpp"

// Forward declarations
SHARED_PTR(Pedal, pPedal, wPedal);

SHARED_PTR(Engine, pEngine, wEngine);
class Engine : public DriveMass
{
  private:
    double torqueLinearMultiplier;
    double telemetryTorque;
    double angularVelLimit;
    pPedal gasPedal;
    Engine (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pEngine create (XmlTag * tag);
    ~Engine ();
    void setGasPedal (pPedal pedal);
    void stepPhysics ();
    double getOutputTorque();
};
#endif
