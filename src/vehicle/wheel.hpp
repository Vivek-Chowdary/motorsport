/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WHEEL_HPP
#define WHEEL_HPP
#include "ode/objects.h"
#include "driveMass.hpp"
#include "worldObject.hpp"
#include "paths.hpp"

// Forward declarations
class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}
SHARED_PTR(Suspension, pSuspension);

SHARED_PTR(Wheel, pWheel);
class Wheel : public DriveMass
{
  private:
    bool userDriver;
    double powered;
    pSuspension baseSuspension;
    pPedal brakePedal;
    Wheel (WorldObject * container, XmlTag * tag);
  public:
    static pWheel create(WorldObject * container, XmlTag * tag);
    ~Wheel ();
    void setSusp(pSuspension suspension) { baseSuspension = suspension; };
    void setUserDriver ();
    void setBrakePedal (pPedal pedal);
    void stepPhysics ();
    double getInputAngularVel();
};
#endif
