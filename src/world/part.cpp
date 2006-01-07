/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "part.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "xmlTag.hpp"
#include "logEngine.hpp"
#include "system.hpp"
#include "ode/ode.h"
#include "world.hpp"
#include "area.hpp"
#include "SDL/SDL_keysym.h"

Part::Part (WorldObject * container, const std::string & name)
    :WorldObject(container, name)
{
    setPath(Paths::part(name));
    setXmlPath(Paths::partXml(name));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "parts." + name);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("parts." + name);
    XmlTag * tag = new XmlTag (getXmlPath().c_str());
    OgreObjectData ogreData;
    OdeObjectData data;
    std::string shape = "none";
    if (tag->getName() == "part")
    {
        setName(tag->getAttribute("name"));
        ogreData.meshPath = tag->getAttribute("mesh");
        data.mass = stod(tag->getAttribute("mass"));
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            data.shape = t->getName();
            if (data.shape == "box")
            {
                data.dimensions = Vector3d(t->getAttribute("dimensions"));
            }
            if (data.shape == "sphere")
            {
                data.radius = stod(t->getAttribute("radius"));
            }
            if (data.shape == "cappedCylinder")
            {
                data.radius = stod(t->getAttribute("radius"));
                data.length = stod(t->getAttribute("length"));
                if (t->getAttribute("directionAxis") == "x") data.directionAxis = 1;
                if (t->getAttribute("directionAxis") == "y") data.directionAxis = 2;
                if (t->getAttribute("directionAxis") == "z") data.directionAxis = 3;
            }
        }
    }
    ogreData.meshPath = getPath() + ogreData.meshPath;
    OgreObject * ogreObject = new OgreObject(this, ogreData, getId());
    ogreObjects[getId()] = ogreObject;

    if (data.shape == "none") log->__format(LOG_ERROR, "No physics shape specified for this part.");
    odeObjects[getId()] = new OdeObject(this, data, getId());
    ogreObjects.begin()->second->setOdeReference(odeObjects.begin()->second);
    delete tag;
}

Part::~Part ()
{
}

void Part::stepPhysics ()
{
    dBodyID partID = odeObjects.begin()->second->getBodyID();
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    if (this == World::getWorldPointer ()->areaList[0]->partList[0])
    {
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_l)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_i)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_j)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_k)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_SPACE)]->getValue() * 500;
        dBodyAddForce (partID, 0, 0, moveZ);
    }
    const dReal * pos;
    pos = dBodyGetPosition(partID);
    log->__format(LOG_DEVELOPER, "part:x=%f y=%f z=%f", pos[0], pos[1], pos[2]);
}
