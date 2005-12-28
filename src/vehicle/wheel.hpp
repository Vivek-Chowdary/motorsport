/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "tools/xercesc_fwd.hpp"
#include <string>
#include "paths.hpp"

//forward declarations
class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Wheel : public DriveMass
{
  private:
    bool userDriver;
    double powered;
    dBodyID baseID;
    dJointID suspJointID;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    Pedal * brakePedal;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    Wheel (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Wheel ();
    void setRefBody(dBodyID inputID) { baseID = inputID; };
    void setSuspJoint(dJointID injointID) { suspJointID = injointID; };
    void setUserDriver ();
    void setBrakePedal (Pedal * pedal);
    void stepPhysics ();
    double getInputAngularVel();
};
#endif
