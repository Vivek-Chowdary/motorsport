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
#include <cstdio>
#include "system.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "world.hpp"
#include "axis.hpp"

void Camera::startGraphics (float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ)
{
    char name[20];
    sprintf (name, "Camera%i", instancesCount);

    ogreCamera = SystemData::getSystemDataPointer ()->ogreSceneManager->createCamera (name);
    ogreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
    ogreCamera->setPosition (Ogre::Vector3 (posX, posY, posZ));
    ogreCamera->lookAt (Ogre::Vector3 (lookAtX, lookAtY, lookAtZ));
    
    // Set near clip plane at 1 mm. of distance.
    ogreCamera->setNearClipDistance (0.100);
}

void Camera::stepGraphics ()
{
    if (this == World::getWorldPointer()->getActiveCamera() )
    {
        float rotX = -SystemData::getSystemDataPointer()->axisMap[2000000]->getValue() + 0.5;
        float rotY = SystemData::getSystemDataPointer()->axisMap[2000001]->getValue() - 0.5;
        //       SystemData::getSystemDataPointer()->axisMap[0000000XXX]->getValue() //keyboard... TODO
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        ogreCamera->yaw (rotX);
        ogreCamera->pitch (rotY);
    }
}

void Camera::stopGraphics ()
{

}
