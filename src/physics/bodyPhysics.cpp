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

void Body::startPhysics (float posX, float posY, float posZ, BodyPhysicsData * physics)
{
    dMass mass;
    dMassSetBoxTotal (&mass, physics->mass, physics->length, physics->width, physics->height);
    bodyID = dBodyCreate (WorldData::getWorldDataPointer ()->worldID);
    dBodySetPosition (bodyID, posX, posY, posZ);
    bodyGeomID = dCreateBox (WorldData::getWorldDataPointer ()->spaceID, physics->length, physics->width, physics->height);
    dGeomSetBody (bodyGeomID, bodyID);
}

void Body::stopPhysics ()
{
    dGeomDestroy (bodyGeomID);
    dBodyDestroy (bodyID);
}

void Body::stepPhysics ()
{
}
