/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "location.hpp"
#include "logEngine.hpp"

Location::Location (XmlTag * tag)
{
    LogEngine * log = new LogEngine (LOG_DEVELOPER, "Location");
    // if (tag->getName() == "location")
    {
        name = tag->getAttribute("name");
        position = Vector3d(tag->getAttribute("position"));
        rotation = Quaternion(tag->getAttribute("rotation"));
    }
    delete log;
}
Location::Location (std::string name, Vector3d position, Quaternion rotation)
{
    this->name = name;
    this->position = position;
    this->rotation = rotation;
}

Location::~Location ()
{
}

Vector3d Location::getPosition ()
{
    return position;
}

Quaternion Location::getRotation ()
{
    return rotation;
}

std::string Location::getName ()
{
    return name;
}
