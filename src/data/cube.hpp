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

#ifndef CUBE_HPP
#   define CUBE_HPP
#   include "ode.h"
#   include "Ogre.h"
#   include "OgreNoMemoryMacros.h"
#   include "system.hpp"
#   include "worldObject.hpp"
class Cube;
#   include "world.hpp"

struct CubePhysicsData;
struct CubeGraphicsData;

class Cube : public WorldObject
{
  private:
    float moveToXPositive;
    float moveToXNegative;
    float moveToYPositive;
    float moveToYNegative;

  public:
    // data
    Cube (char * xmlFilename);
    ~Cube ();
    void updateOgrePosition ();
    void updateOgreOrientation ();
    float getMoveToXPositive ();
    float getMoveToXNegative ();
    float getMoveToYPositive ();
    float getMoveToYNegative ();
    void processXmlRootNode (DOMNode * n);

    // physics
    void startPhysics (DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID cubeID;
    dGeomID cubeGeomID;
    void setPosition (float posX, float posY, float posZ);

    // graphics
    void startGraphics (DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();
    Ogre::Entity * cubeEntity;
    Ogre::SceneNode * cubeNode;

    // input
    void startInput ();
    void stepInput ();
    void stopInput ();
    void setMoveToXPositive (float multiplier);
    void setMoveToXNegative (float multiplier);
    void setMoveToYPositive (float multiplier);
    void setMoveToYNegative (float multiplier);
};

struct CubePhysicsData
{
    char *author;
    char *license;
    int size;
};

struct CubeGraphicsData
{
    char *author;
    char *license;
    char *material;
    char *mesh;
    char *ogreName;
};

#endif
