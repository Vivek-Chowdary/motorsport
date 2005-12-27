/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLD_HPP
#define WORLD_HPP
#include "SDL/SDL_types.h"
#include "ode/objects.h"
#include <vector>
#include <string>
#include "worldObject.hpp"

//forward declaration
class Camera;
class Area;
class Vehicle;

class World : public WorldObject
{
  private:
    static World *worldPointer;
    std::string description;
    Camera * activeCamera;
    int activeAreaCameraIndex;
    int activeVehicleCameraIndex;
    void processXmlRootNode(XERCES_CPP_NAMESPACE::DOMNode * n);
    void World::processXmlVehicleListNode (XERCES_CPP_NAMESPACE::DOMNode * vehicleListNode);
  public:
    static World *getWorldPointer ();
    World (WorldObject * container, std::string name);
    ~World ();

    dWorldID worldID;
    dWorldID ghostWorldID;
    dSpaceID spaceID;
    dJointGroupID jointGroupID;

    std::vector < Area * > areaList;
    std::vector < Vehicle * > vehicleList;

    void setActiveCamera (Camera * camera);
    Camera * getActiveCamera ();
    Camera * activeAreaCamera;
    Camera * activeVehicleCamera;
    int getActiveAreaCameraIndex();
    int getActiveVehicleCameraIndex();
};

#endif
