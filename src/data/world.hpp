/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLD_HPP
#   define WORLD_HPP
#   include "SDL/SDL_types.h"
#   include "ode/objects.h"
#   include <vector>
#   include <string>
#   include "worldObject.hpp"

//forward declaration
class Camera;
class Track;
class Vehicle;

class World : public WorldObject
{
  private:
    static World *worldPointer;
    std::string name;
    std::string description;
    Camera * activeCamera;
    int activeTrackCameraIndex;
    int activeVehicleCameraIndex;
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
    std::vector < Vehicle * > vehicleList;

    void setActiveCamera (Camera * camera);
    Camera * getActiveCamera (void);
    Camera * activeTrackCamera;
    Camera * activeVehicleCamera;
    int getActiveTrackCameraIndex();
    int getActiveVehicleCameraIndex();
};

#endif
