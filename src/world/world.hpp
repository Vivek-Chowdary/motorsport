/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
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
#include "worldObject.hpp"

//forward declaration
SHARED_PTR_MAP(Camera, pCamera, Cameras, CamerasIt);
SHARED_PTR_MAP(Vehicle, pVehicle, Vehicles, VehiclesIt);
SHARED_PTR_MAP(Area, pArea, Areas, AreasIt);

class World : public WorldObject
{
  private:
    static World *worldPointer;
    std::string description;
    pCamera activeCamera;
    void processXmlVehicleListNode (XmlTag * tag);
    void processXmlRootNode (XmlTag * tag);
  public:
    static World *getWorldPointer ();
    World (WorldObject * container, std::string name);
    ~World ();

    dWorldID worldID;
    dWorldID ghostWorldID;
    dSpaceID spaceID;
    dJointGroupID jointGroupID;

    Areas areas;
    Vehicles vehicles;

    void setActiveCamera (pCamera camera);
    pCamera getActiveCamera();
    pCamera activeAreaCamera;
    pCamera activeVehicleCamera;
    pCamera getActiveAreaCamera();
    pCamera getActiveVehicleCamera();
};

#endif
