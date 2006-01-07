/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SUSPENSION_HPP
#define SUSPENSION_HPP
#include "ode/objects.h"
#include "worldObject.hpp"
#include "quaternion.hpp"
#include "vector3d.hpp"
#include "location.hpp"
#include "vehicle.hpp"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Suspension : public WorldObject
{
  private:
    dJointID jointID;
    Quaternion rotation;
    Vector3d position;
    double springConstant;
    double dampingConstant;
    double steeringAngle;
    bool userDriver;
    void stopPhysics ();
  public:
    Suspension (WorldObject * container, XmlTag * tag);
    ~Suspension ();
    void attach (WorldObject * base, WorldObject * object);
    void setUserDriver ();
    void stepPhysics ();
    dJointID getJointID();
    Vector3d getSecondLinkPosition ();
    Quaternion getSecondLinkRotation ();
};

#endif

