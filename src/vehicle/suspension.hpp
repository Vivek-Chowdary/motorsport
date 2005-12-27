/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "tools/xercesc_fwd.hpp"
#include <string>
#include "quaternion.hpp"
#include "vector3d.hpp"
#include "vehicle.hpp"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}
class Vehicle;
class Wheel;

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
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    Suspension (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Suspension ();
    void attach (Wheel & wheel, Vehicle * vehicle);
    void setUserDriver ();
    void stepPhysics ();
    Vector3d getInitialWheelPosition ();
    Quaternion getInitialWheelRotation ();
};
#endif
