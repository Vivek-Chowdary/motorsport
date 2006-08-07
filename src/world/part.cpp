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


pPart Part::create(const std::string & name)
{
    pPart part(new Part(name));
    return part;
}

void Part::readCustomDataTag(XmlTag * tag)
{
    setName(tag->getAttribute("name"));
}
Part::Part (const std::string & name)
    :WorldObject("part")
{
    setPath(Paths::part(name));
    setXmlPath(Paths::partXml(name));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "parts." + name);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("parts." + name);
    XmlTag * tag = new XmlTag (getXmlPath().c_str());
    std::string shape = "none";
    constructFromTag(tag);
    delete tag;
}

Part::~Part ()
{
}

void Part::stepPhysics ()
{
    WorldObject::stepPhysics();
    if (odeObjects.size() > 0)
    {
        dBodyID partID = odeObjects.begin()->second->getBodyID();
        // //////////////simplified air friction (test)(should be forces!)
        dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
        // ////////////////////////////////////simplified air friction
        const dReal * pos;
        pos = dBodyGetPosition(partID);
    }
}
