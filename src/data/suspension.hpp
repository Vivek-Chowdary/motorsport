/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
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
#   include "data/xercesc_fwd.hpp"
#   include <string>

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}
class Vehicle;
class Wheel;
class Vector3d;

class Suspension : public WorldObject
{
  private:
    static int instancesCount;
    std::string index;
    dJointID jointID;
    Vector3d * rotation;
    Vector3d * position;
  public:

    // data
    Suspension (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Suspension ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    std::string getIndex();
    void attach (Wheel & wheel, Vehicle & vehicle);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    void setPosition (Vector3d position);
    Vector3d getPosition ();
    void setRotation (Vector3d rotation);
    Vector3d getRotation ();
};
#endif
