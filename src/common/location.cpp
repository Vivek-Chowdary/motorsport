/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
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
pLocation Location::create(const std::string & name, Vector3d position, Quaternion rotation)
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
Location::Location (const std::string & name, Vector3d position, Quaternion rotation)
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
void Location::addRotation (Vector3d origin, Quaternion diff)
{
    Vector3d rotMovement = diff.rotateObject(this->getPosition() - origin);
    this->setPosition (origin + rotMovement);

    Quaternion current = this->getRotation();
    Quaternion newrot = diff * current;
    setRotation (newrot);
}
void Location::addPosition (Vector3d diff)
{
    Vector3d current = this->getPosition();
    Vector3d newpos = current + diff;
    this->setPosition (newpos);
}
void Location::setPosition (Vector3d position)
{
    this->position = position;
}

void Location::setRotation (Quaternion rotation)
{
    this->rotation = rotation;
}
