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

    //TODO move camera movement routines to the physics engine, so that it's not framerate-dependent
/*    if (this == World::getWorldPointer()->getActiveCamera() )
    {
        // Move the camera
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_d)]->getValue();
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_s)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        moveZ *= SystemData::getSystemDataPointer()->physicsTimeStep;
        World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));

        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue();
        moveZ = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_w)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        moveZ *= SystemData::getSystemDataPointer()->physicsTimeStep;
        World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));

        // Rotate the camera
        float rotX = -SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(0)]->getValue() + 0.5;
        float rotY = SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(1)]->getValue() - 0.5;
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        ogreCamera->yaw (rotX);
        ogreCamera->pitch (rotY);

        rotX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RIGHT)]->getValue();
        rotY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue();
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        ogreCamera->yaw (rotX);
        ogreCamera->pitch (rotY);

        rotX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_LEFT)]->getValue();
        rotY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue();
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        ogreCamera->yaw (rotX);
        ogreCamera->pitch (rotY);
    }*/
}

void Camera::stopGraphics ()
{

}
