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

#ifndef WHEEL_HPP
#   define WHEEL_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"
#   include <string>

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Wheel : public WorldObject
{
  private:
    static int instancesCount;
    std::string name;

  public:

    // data
    Wheel (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Wheel ();
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    std::string getName();

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID wheelID;
    dGeomID wheelGeomID;
    void setPosition (double posX, double posY, double posZ);
    void setRotation (double rotX, double rotY, double rotZ);

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();

    class Ogre::Entity;
    class Ogre::SceneNode;

    Ogre::Entity * wheelEntity;
    Ogre::SceneNode * wheelNode;
};
#endif
