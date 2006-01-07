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
#include <map>
#include <string>
#include <vector>
#include "paths.hpp"

//forward declatation
namespace Ogre {
    class Entity;
    class SceneNode;
};
class Part;
class Location;
class Camera;

class Area : public WorldObject
{
    private:
        std::string description;
        std::string author;
        std::string contact;
        std::string license;
        void construct (XmlTag * tag);
    public:
        Area (WorldObject * container, std::string areaName);
        ~Area ();
        std::map <std::string, Location * > vehiclePositionMap;
        std::vector < Part * > partList;
        std::vector < Camera * > cameraList;

        dGeomID bodyGeomID;
        dBodyID areaBodyID;

        Ogre::SceneNode * planeNode;
        Ogre::Entity * planeEntity;
        Ogre::Entity * areaEntity;
        Ogre::SceneNode * areaNode;
        void setCastShadows(bool castShadows);
        void setRenderDetail(int renderMode);
};
#endif
