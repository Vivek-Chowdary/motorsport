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
#   include "ode.h"
#   include "Ogre.h"
#   include "OgreNoMemoryMacros.h"
#   include <vector>
#   include "system.hpp"
#   include "world.hpp"
#   include "domParser.hpp"
#   include "logEngine.hpp"

struct BodyData;
struct BodyPhysicsData;
struct BodyGraphicsData;

class Body
{
  private:
    //empty
  public:
      Body (int bodyNumber, float posX, float posY, float posZ);
     ~Body ();

    // physics
    void startPhysics (float posX, float posY, float posZ, BodyPhysicsData * physics);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID bodyID;
    dGeomID bodyGeomID;
    void processBodyPhysicsDataNode (DOMNode * n, BodyPhysicsData * physics);

    // graphics
    void startGraphics (int bodyNumber, BodyGraphicsData * graphics);
    void stepGraphics ();
    void stopGraphics ();
    Ogre::Entity * bodyEntity;
    Ogre::SceneNode * bodyNode;
    void processBodyGraphicsDataNode (DOMNode * n, BodyGraphicsData * graphics);

    // input
    //empty

    // data
    static std::vector < Body * >bodyList;
    void updateOgrePosition ();
    void updateOgreOrientation ();
    static void processBodyDataFile (DOMNode * n, void *data);
};

struct BodyData
{
    Body *body;
    char *name;
    char *description;
    BodyPhysicsData *physics;
    BodyGraphicsData *graphics;
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
