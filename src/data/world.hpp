
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
#   include "SDL/SDL_types.h"
#   include "ode/objects.h"
#   include <vector>
#   include <string>
#   include "worldObject.hpp"
#   include "vehicle.hpp"

//forward declaration
class Camera;
class Track;

class World : public WorldObject
{
  private:
    static World *worldPointer;
    std::string name;
    std::string description;
    Camera * activeCamera;
    int activeTrackCameraIndex;
  public:
    static World *getWorldPointer ();
    void processXmlRootNode(XERCES_CPP_NAMESPACE::DOMNode * n);
    World (char * xmlFilename);
    ~World ();

    dWorldID worldID;
    dWorldID ghostWorldID;
    dSpaceID spaceID;
    dJointGroupID jointGroupID;

    std::vector < Track * > trackList;
    //std::vector < WorldObject * > vehicleList;
    std::vector < Vehicle * > vehicleList;

    void setActiveCamera (Camera * camera);
    Camera * getActiveCamera (void);
    int getActiveTrackCameraIndex();
};

#endif
