/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "location.hpp"
#include "logEngine.hpp"

pLocation Location::create(XmlTag * tag)
{
    pLocation loc(new Location(tag));
    return loc;
}
pLocation Location::create(std::string name, Vector3d position, Quaternion rotation)
{
    pLocation loc(new Location(name, position, rotation));
    return loc;
}
Location::Location (XmlTag * tag)
{
    pLogEngine log (LogEngine::create (LOG_DEVELOPER, "Location"));
    name = tag->getAttribute("name");
    position = Vector3d(tag->getAttribute("position"));
    rotation = Quaternion(tag->getAttribute("rotation"));
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
