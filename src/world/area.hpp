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
#include <vector>
#include "paths.hpp"

//forward declatation
namespace Ogre {
    class Entity;
    class SceneNode;
};
class Part;
class Location;

SHARED_PTR_MAP(Camera, pCamera, Cameras, CameraIt);
SHARED_PTR_MAP(Area, pArea, Areas, AreasIt);

class Area : public WorldObject
{
    private:
        std::string description;
        std::string author;
        std::string contact;
        std::string license;
        void construct (XmlTag * tag);
        Area (WorldObject * container, std::string areaName);
    public:
        static pArea create(WorldObject * container, std::string areaName);
        ~Area ();
        std::map <std::string, Location * > vehiclePositionMap;
        std::vector < Part * > partList;
        Cameras cameras;

        dBodyID areaBodyID;

        Ogre::SceneNode * planeNode;
        Ogre::Entity * planeEntity;
        void setCastShadows(bool castShadows);
        void setRenderDetail(int renderMode);
};
#endif
