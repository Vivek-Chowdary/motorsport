/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "camera.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "track.hpp"
#include "SDL/SDL_keysym.h"
#include "vector3d.hpp"

void Camera::startPhysics (Vector3d position, Vector3d lookAt)
{
    positionID = 0;
    targetID = 0;
    positionOffset = new Vector3d (position);
    targetOffset = new Vector3d (lookAt);
}

void Camera::stopPhysics ()
{
    delete positionOffset;
    delete targetOffset;
}

void Camera::stepPhysics ()
{

    //TODO move camera movement routines to the physics engine, so that it's not framerate-dependent
    if (this == World::getWorldPointer()->getActiveCamera() )
    {
        // Move the camera
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_d)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_s)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        moveY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        //World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));
        *positionOffset += Vector3d(moveX/1000, moveY/1000, 0);

        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_w)]->getValue();
        moveX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        moveY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        //World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (moveX / 100, 0, moveZ / 100));
        *positionOffset += Vector3d(moveX/1000, moveY/1000, 0);

        // Rotate the camera
        float rotX = -SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(0)]->getValue() + 0.5;
        float rotY = SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(1)]->getValue() - 0.5;
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        *targetOffset += Vector3d (rotX/1000, rotY/1000, 0);
        //ogreCamera->yaw (rotX);
        //ogreCamera->pitch (rotY);
/*
        rotX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RIGHT)]->getValue();
        rotY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue();
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        *targetOffset += Vector3d (rotX/1000, rotY/1000, 0);
        //ogreCamera->yaw (rotX);
        //ogreCamera->pitch (rotY);

        rotX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_LEFT)]->getValue();
        rotY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue();
        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;

        rotX *= SystemData::getSystemDataPointer()->physicsTimeStep;
        rotY *= SystemData::getSystemDataPointer()->physicsTimeStep;
        *targetOffset += Vector3d (rotX/1000, rotY/1000, 0);
        //ogreCamera->yaw (rotX);
        //ogreCamera->pitch (rotY);
*/
    }
}

