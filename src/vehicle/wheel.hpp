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

//forward declarations
class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}
class Suspension;

class Wheel : public DriveMass
{
  private:
    bool userDriver;
    double powered;
    WorldObject * baseObject;
    Suspension * baseSuspension;
    Pedal * brakePedal;
  public:
    Wheel (WorldObject * container, XmlTag * tag);
    ~Wheel ();
    void setRefObject(WorldObject * object) { baseObject = object; };
    void setSusp(Suspension * suspension) { baseSuspension = suspension; };
    void setUserDriver ();
    void setBrakePedal (Pedal * pedal);
    void stepPhysics ();
    double getInputAngularVel();
};
#endif
