/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#include "body.hpp"

std::vector < Body * >Body::bodyList;

Body::Body (int bodyNumber, float posX, float posY, float posZ)
{
    BodyData *bodyData = new BodyData;
    bodyData->body = this;
    bodyData->physics = new BodyPhysicsData;
    bodyData->physics->mass = 100;
    bodyData->physics->length = 1;
    bodyData->physics->width = 1;
    bodyData->physics->height = 1;
    bodyData->graphics = new BodyGraphicsData;
    processXmlFile ("../data/body.xml", &Body::processBodyDataFile, (void *) bodyData);

    startPhysics (posX, posY, posZ, bodyData->physics);
    startGraphics (bodyNumber, bodyData->graphics);

    // delete [](bodyData->name);
    // delete [](bodyData->description);
    // delete [](bodyData->physics->author);
    // delete [](bodyData->physics->license);
    delete bodyData->physics;
    // delete [](bodyData->graphics->author);
    // delete [](bodyData->graphics->license);
    delete[](bodyData->graphics->material);
    delete[](bodyData->graphics->mesh);
    delete[](bodyData->graphics->ogreName);
    delete bodyData->graphics;
    delete bodyData;
}

Body::~Body ()
{
    stopPhysics ();
    stopGraphics ();
}


void Body::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (bodyID);  // need to allocate memory first??
    bodyNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Body::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (bodyID);    // need to allocate memory first??
    bodyNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}
