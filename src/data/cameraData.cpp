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

#include "camera.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "vector3d.hpp"

int Camera::instancesCount = 0;

Camera::Camera (float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ)
{
    instancesCount ++;
    
    startGraphics ();
    startPhysics (posX, posY, posZ, lookAtX, lookAtY, lookAtZ);
    startInput ();
}

Camera::~Camera ()
{
    instancesCount--;
    stopGraphics();
    stopInput();
    stopPhysics();
}

void Camera::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (positionID);
    ogreCamera->setPosition (Ogre::Vector3 (temp[0] + positionOffset->x, temp[1] + positionOffset->y, temp[2] + positionOffset->z));
}
void Camera::updateOgreTarget ()
{
    //const dReal *temp = dBodyGetQuaternion (bodyID);
    //bodyNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
    const dReal *temp = dBodyGetPosition (targetID);
    ogreCamera->lookAt (Ogre::Vector3 (temp[0] + targetOffset->x, temp[1] + targetOffset->y, temp[2] + targetOffset->z));
}

void Camera::setPositionID (dBodyID positionID)
{
    this->positionID = positionID;
}

void Camera::setTargetID (dBodyID targetID)
{
    this->targetID = targetID;
}
