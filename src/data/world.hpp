
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

#ifndef WORLD_HPP
#   define WORLD_HPP
#   include "SDL.h"
#   include "ode.h"
#   include <vector>
#   include "logEngine.hpp"
#   include "worldObject.hpp"
#   include "system.hpp"
#   include "cube.hpp"
#   include "body.hpp"
#   include "camera.hpp"

class World : public WorldObject
{
  private:
    static World *worldPointer;
    std::string name;
    std::string description;
  public:
    static World *getWorldPointer ();
    void processXmlRootNode(DOMNode * n);
    World (char * xmlFilename);
    ~World ();

    dWorldID worldID;
    dSpaceID spaceID;
    dJointGroupID jointGroupID;

    //Track * track;
    //Car * body;
    Body * body;

    std::vector < Cube * > cubeList;
    std::vector < Body * >bodyList;
    std::vector < Camera * >cameraList;
};

#endif
