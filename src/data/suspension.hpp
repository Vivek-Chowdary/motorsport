/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

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
    Vector3d * position;
    Vector3d * rotation;
    dJointID jointID;
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
