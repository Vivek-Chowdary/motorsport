
/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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

#include "world.hpp"

WorldData * WorldData::worldDataPointer = NULL;

WorldData * WorldData::getWorldDataPointer ( )
{
    if (!worldDataPointer)
    {
        new WorldData ();
    }
    return ( worldDataPointer );
}

WorldData::WorldData ( )
{
    if (worldDataPointer)
    {
        delete this;
    }else{
        worldDataPointer = this;
    }
}

WorldData::~WorldData ( )
{
    delete worldDataPointer;
    worldDataPointer = NULL;
}

void Cube::updateOgrePosition()
{
    const dReal * temp = dBodyGetPosition(cubeID);//need to allocate memory first??
    cubeNode->setPosition( *(temp+0), *(temp+1), *(temp+2));
}

void Cube::setMoveToXPositive ( float multiplier)
{
    moveToXPositive = multiplier;
}
void Cube::setMoveToXNegative ( float multiplier)
{
    moveToXNegative = multiplier;
}

void Cube::setMoveToYPositive ( float multiplier)
{
    moveToYPositive = multiplier;
}
void Cube::setMoveToYNegative ( float multiplier)
{
    moveToYNegative = multiplier;
}

float Cube::getMoveToXPositive ( )
{
    return moveToXPositive;
}
float Cube::getMoveToXNegative ( )
{
    return moveToXNegative;
}

float Cube::getMoveToYPositive ( )
{
    return moveToYPositive;
}
float Cube::getMoveToYNegative ( )
{
    return moveToYNegative;
}
void Cube::updateOgreOrientation()
{
    const dReal * temp = dBodyGetQuaternion(cubeID);//need to allocate memory first??
    cubeNode->setOrientation( *(temp+0), *(temp+1), *(temp+2), *(temp+3));
}

void Camera::setRotateLeft ( int multiplier )
{
    rotateLeft = multiplier;
}
void Camera::setRotateRight ( int multiplier )
{
    rotateRight = multiplier;
}

void Camera::setRotateUp ( int multiplier )
{
    rotateUp = multiplier;
}
void Camera::setRotateDown ( int multiplier )
{
    rotateDown = multiplier;
}

int Camera::getRotateUp (  )
{
    return rotateUp;
}
int Camera::getRotateDown (  )
{
    return rotateDown;
}

int Camera::getRotateLeft (  )
{
    return rotateLeft;
}
int Camera::getRotateRight (  )
{
    return rotateRight;
}
