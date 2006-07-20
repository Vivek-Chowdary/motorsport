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
    pPartOdeData data(new PartOdeData);
    data->mass = stod(tag->getAttribute("mass"));
    XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        data->shape = t->getName();
        if (data->shape == "box")
        {
            data->size = Vector3d(t->getAttribute("size"));
        }
        if (data->shape == "sphere")
        {
            data->radius = stod(t->getAttribute("radius"));
        }
        if (data->shape == "cappedCylinder")
        {
            data->radius = stod(t->getAttribute("radius"));
            data->length = stod(t->getAttribute("length"));
            if (t->getAttribute("directionAxis") == "x") data->directionAxis = 1;
            if (t->getAttribute("directionAxis") == "y") data->directionAxis = 2;
            if (t->getAttribute("directionAxis") == "z") data->directionAxis = 3;
        }
    }
    if (data->shape == "none") log->__format(LOG_ERROR, "No physics shape specified for this part.");
    odeObjects[getId()] = pOdeObject(new OdeObject(this, data, getId()));
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
    dBodyID partID = odeObjects.begin()->second->getBodyID();
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    const dReal * pos;
    pos = dBodyGetPosition(partID);
}
