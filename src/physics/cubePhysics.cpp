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

void Cube::startPhysics( float size, float posX, float posY, float posZ )
{
    dMass mass;
    dMassSetBox ( &mass, 1, size, size, size );
    cubeID = dBodyCreate ( WorldData::getWorldDataPointer()->worldID );
    dBodySetPosition (cubeID, posX, posY, posZ );
    cubeGeomID = dCreateBox(WorldData::getWorldDataPointer()->spaceID,size,size,size);
    dGeomSetBody (cubeGeomID, cubeID);

}

void Cube::stopPhysics ( )
{
    dGeomDestroy (cubeGeomID);
    dBodyDestroy (cubeID);
}


void Cube::stepPhysics()
{
    ////////////////simplified air friction (test)
    dBodySetAngularVel (cubeID,
    *(dReal*)(dBodyGetAngularVel(cubeID)+0),
    *(dReal*)(dBodyGetAngularVel(cubeID)+1),
    *(dReal*)(dBodyGetAngularVel(cubeID)+2)*(dReal)(0.01)/(SystemData::getSystemDataPointer()->physicsTimeStep * SystemData::getSystemDataPointer()->physicsTimeStep) );
    //////////////////////////////////////simplified air friction
    //applying user input [forces]
    float moveToX = 0, moveToY = 0;
    moveToX += getMoveToXPositive();
    moveToX -= getMoveToXNegative();
    moveToY += getMoveToYPositive();
    moveToY -= getMoveToYNegative();
//    moveToX /= 100;
//    moveToY /= 100;
    dBodyAddForce (cubeID, moveToX / (SystemData::getSystemDataPointer()->physicsTimeStep * SystemData::getSystemDataPointer()->physicsTimeStep), moveToY / (SystemData::getSystemDataPointer()->physicsTimeStep * SystemData::getSystemDataPointer()->physicsTimeStep),0.0f );
}
