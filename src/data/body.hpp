/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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
#   include "ode.h"
#   include "Ogre.h"
#   include "OgreNoMemoryMacros.h"
#   include "system.hpp"
#   include "worldObject.hpp"
class Body;
#   include "world.hpp"

struct BodyPhysicsData;
struct BodyGraphicsData;

class Body : public WorldObject
{
  private:
    //empty

  public:
    // data
    Body (char * xmlFilename);
    ~Body ();
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (DOMNode * n);

    // physics
    void startPhysics (DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID bodyID;
    dGeomID bodyGeomID;
    void setPosition (float posX, float posY, float posZ);

    // graphics
    void startGraphics (DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();
    Ogre::Entity * bodyEntity;
    Ogre::SceneNode * bodyNode;
};

struct BodyPhysicsData
{
    char *author;
    char *license;
    float length;
    float width;
    float height;
    float mass;
};

struct BodyGraphicsData
{
    char *author;
    char *license;
    char *material;
    char *mesh;
    char *ogreName;
};

#endif
