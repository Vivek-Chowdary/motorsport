/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "camera.hpp"
#include <cstdio>
#include "system.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "world.hpp"
#include "axis.hpp"
#include "SDL/SDL_keysym.h"

void Camera::startGraphics (/*float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ*/)
{
    char name[20];
    sprintf (name, "Camera%i", instancesCount);

    ogreCamera = SystemData::getSystemDataPointer ()->ogreSceneManager->createCamera (name);
    ogreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
/*    ogreCamera->setPosition (Ogre::Vector3 (posX, posY, posZ));
    ogreCamera->lookAt (Ogre::Vector3 (lookAtX, lookAtY, lookAtZ));*/
    
    // Set near clip plane at 1 mm. of distance.
    ogreCamera->setNearClipDistance (0.100);
}

void Camera::stepGraphics ()
{
    updateOgrePosition();
    updateOgreTarget();
}

void Camera::stopGraphics ()
{

}
