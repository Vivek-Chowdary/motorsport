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

#ifndef BODY_HPP
#   define BODY_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Body : public WorldObject
{
  private:
    static int instancesCount;

  public:

    // data
    Body (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Body ();
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID bodyID;
    dGeomID bodyGeomID;
    void setPosition (double posX, double posY, double posZ);
    void getPosition (double & posX, double & posY, double & posZ);
    void setRotation (double rotX, double rotY, double rotZ);

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();

    class Ogre::Entity;
    class Ogre::SceneNode;

    Ogre::Entity * bodyEntity;
    Ogre::SceneNode * bodyNode;
};
#endif
