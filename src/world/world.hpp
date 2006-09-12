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
SHARED_PTR_MAP(Camera, pCamera, Cameras, CamerasIt, wCamera);
SHARED_PTR_MAP(Vehicle, pVehicle, Vehicles, VehiclesIt, wVehicle);
SHARED_PTR_MAP(Area, pArea, Areas, AreasIt, wArea);
SHARED_PTR_MAP(Location, pLocation, Locations, LocationsIt, wLocation);
SHARED_PTR(World,pWorld, wWorld);

class World : public WorldObject
{
  private:
    static pWorld world;
    static std::string newWorld;
    Ogre::SceneNode * planeNode;
    Ogre::Entity * planeEntity;
    std::string description;
    pCamera activeCamera;
    bool cameraDirector;
    void setActiveCamera(pCamera camera);
    void processXmlRootNode (XmlTag * tag);
    World (std::string name);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pWorld get();
    static void setNewWorld(std::string name);
    static void destroy();
    ~World ();

    dWorldID worldID;
    dWorldID ghostWorldID;
    dSpaceID spaceID;
    dJointGroupID jointGroupID;

    void switchCameraDirector();
    void switchNextAreaCamera();
    void switchNextVehicleCamera();
    bool isActiveCamera(pCamera camera);

    void stepGraphics();
};

#endif
