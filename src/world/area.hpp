/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef AREA_HPP
#define AREA_HPP
#include "ode/objects.h"
#include "worldObject.hpp"
#include "paths.hpp"

//forward declatation
namespace Ogre {
    class Entity;
    class SceneNode;
};

SHARED_PTR_MAP(Location, pLocation, Locations, LocationsIt, wLocation);
SHARED_PTR_MAP(Camera, pCamera, Cameras, CameraIt, wCamera);
SHARED_PTR_MAP(Part, pPart, Parts, PartsIt, wPart);
SHARED_PTR_MAP(Area, pArea, Areas, AreasIt, wArea);
SHARED_PTR(Vehicle, pVehicle, wVehicle);

class Area : public WorldObject
{
    private:
        std::string description;
        std::string author;
        std::string contact;
        std::string license;
        void construct (XmlTag * tag);
        Area (std::string areaName);
        void readCustomDataTag(XmlTag * tag);
    public:
        static pArea create(std::string areaName);
        ~Area ();

        dBodyID areaBodyID;
        pCamera getClosestCamera(Vector3d location);

        void setCastShadows(bool castShadows);
        void setRenderDetail(int renderMode);
};
#endif
