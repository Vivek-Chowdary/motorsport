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

#include "cube.hpp"

void Cube::startPhysics (float posX, float posY, float posZ, CubePhysicsData * physics)
{
/*    dMass mass;

    dMassSetBox ( &mass, 1, physicsData->size, physicsData->size,
                  physicsData->size );
    cubeID = dBodyCreate ( WorldData::getWorldDataPointer (  )->worldID );
    dBodySetPosition ( cubeID, posX, posY, posZ );
    cubeGeomID =
        dCreateBox ( WorldData::getWorldDataPointer (  )->spaceID,
                     physicsData->size, physicsData->size,
                     physicsData->size );
    dGeomSetBody ( cubeGeomID, cubeID );*/

    dMass mass;

    dMassSetBox (&mass, 1, physics->size, physics->size, physics->size);
    cubeID = dBodyCreate (WorldData::getWorldDataPointer ()->worldID);
    dBodySetPosition (cubeID, posX, posY, posZ);
    cubeGeomID = dCreateBox (WorldData::getWorldDataPointer ()->spaceID, physics->size, physics->size, physics->size);
    dGeomSetBody (cubeGeomID, cubeID);
}

void Cube::stopPhysics ()
{
    dGeomDestroy (cubeGeomID);
    dBodyDestroy (cubeID);
}

void Cube::stepPhysics ()
{
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (cubeID, (*(dReal *) (dBodyGetAngularVel (cubeID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (cubeID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (cubeID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    float moveToX = 0, moveToY = 0;

    moveToX += getMoveToXPositive ();
    moveToX -= getMoveToXNegative ();
    moveToY += getMoveToYPositive ();
    moveToY -= getMoveToYNegative ();
    moveToX /= 200;
    moveToY /= 200;
    dBodyAddForce (cubeID, moveToX, moveToY, 0.0f);
}
