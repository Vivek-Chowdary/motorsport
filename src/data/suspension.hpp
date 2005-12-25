/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SUSPENSION_HPP
#   define SUSPENSION_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include "tools/xercesc_fwd.hpp"
#   include <string>
#   include "quaternion.hpp"
#   include "vector3d.hpp"

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
    // data
    static int instancesCount;
    std::string index;
    dJointID jointID;
    Quaternion rotation;
    Vector3d position;
    double springConstant;
    double dampingConstant;
    double steeringAngle;
    bool userDriver;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    // data
    Suspension (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Suspension ();
    std::string getIndex();
    void attach (Wheel & wheel, Vehicle & vehicle);
    void setUserDriver ();

    // physics
    void stepPhysics ();
//    void setPosition (Vector3d position);
//    Vector3d getPosition ();
    Vector3d getInitialWheelPosition ();
//    void applyRotation (Quaternion rotation);
//    Quaternion getRotation ();
    Quaternion getInitialWheelRotation ();
};
#endif
