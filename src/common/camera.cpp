/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "camera.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "vector3d.hpp"
#include <cstdio>
#include "system.hpp"
#include "world.hpp"
#include "axis.hpp"
#include "SDL/SDL_keysym.h"
#include "ode/ode.h"
#include "log/logEngine.hpp"
#include "area.hpp"
#include "vector3d.hpp"

Camera::Camera (WorldObject * container, XmlTag * tag)
    :WorldObject(container, "camera")
{
    ogreCamera = SystemData::getSystemDataPointer ()->ogreSceneManager->createCamera (getId());
    ogreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
    ogreCamera->setNearClipDistance (0.100);
//    ogreCamera->setAutoAspectRatio(true);
    log->__format (LOG_DEVELOPER, "Creating a camera");
    positionOffset = new Vector3d (0, 0, 0);
    targetOffset = new Vector3d (0, 0, 0);
    positionID = 0;
    targetID = 0;
    isFree = false;
    if (tag->getName() == "camera")
    {
        setName (     tag->getAttribute("name"));
        *positionOffset = Vector3d (tag->getAttribute("position"));
        std::string t = "";
        if ( (t = tag->getAttribute("target")) != "")
        {
            *targetOffset = Vector3d (t);
        }
        /*
        else
        {
            //TODO create a target based on "rotation" tag.
            log->__format(LOG_ERROR, "Camera has no readable target!");
        }
        */
    }
}

Camera::~Camera ()
{
    stopPhysics();
}

void Camera::updateOgreRotation ()
{
    const dReal *temp = dBodyGetQuaternion (positionID);
    Ogre::Quaternion tempRot (temp[0], temp[1], temp[2], temp[3]);
    ogreCamera->setFixedYawAxis (true, tempRot.zAxis());
}

Vector3d Camera::updateOgrePosition ()
{
    const dReal *temp = dBodyGetQuaternion (positionID);
    Ogre::Quaternion tempRot(temp[0], temp[1], temp[2], temp[3]);
    Ogre::Matrix3 rot;
    tempRot.ToRotationMatrix (rot);

    Ogre::Vector3 tempPos(positionOffset->x, positionOffset->y, positionOffset->z);
    Ogre::Vector3 pos = rot * tempPos;

    temp = dBodyGetPosition (positionID);
    pos += Ogre::Vector3 (temp[0], temp[1], temp[2]);

    ogreCamera->setPosition (pos);
    return Vector3d (pos.x, pos.y, pos.z);
}
Vector3d Camera::updateOgreTarget ()
{
    const dReal *temp = dBodyGetQuaternion (targetID);
    Ogre::Quaternion tempRot(temp[0], temp[1], temp[2], temp[3]);
    Ogre::Matrix3 rot;
    tempRot.ToRotationMatrix (rot);

    Ogre::Vector3 tempPos(targetOffset->x, targetOffset->y, targetOffset->z);
    Ogre::Vector3 pos = rot * tempPos;

    temp = dBodyGetPosition (targetID);
    pos += Ogre::Vector3 (temp[0], temp[1], temp[2]);

    ogreCamera->lookAt (pos);
    return Vector3d (pos.x, pos.y, pos.z);
}

void Camera::stepGraphics ()
{
    updateOgreRotation();
    if (isFree)
    {
       // empty 
    } else {
        Vector3d position (updateOgrePosition());
        Vector3d target (updateOgreTarget());
        double distance = target.distance(position);
        const double maxD = 150.;
        const double minD = 15.;
        const Ogre::Radian minF(5.  * 3.14159 / 180.);
        const Ogre::Radian maxF(45. * 3.14159 / 180.);
        Ogre::Radian newFov;
        if (distance > maxD)
        {
            newFov = minF;
        } else if (distance < minD)
        {
            newFov = maxF;
        } else {
            newFov = ( maxF - ( ( ( distance - minD ) / ( maxD - minD ) ) * (maxF - minF) ) );
        }
        ogreCamera->setFOVy (newFov);
        // ogreCamera->setLodBias (1.0);
    }
}

void Camera::stopPhysics ()
{
    delete positionOffset;
    delete targetOffset;
}

void Camera::stepPhysics ()
{
    if (this == World::getWorldPointer()->getActiveCamera() )
    {
        static bool wasPressed = false;
        if (SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_x)]->getValue() == 1)
        {
            if (!wasPressed)
            {
                isFree = !isFree;
            }
            wasPressed = true;
        } else {
            wasPressed = false;
        }
        if (isFree)
        {
            // Move the camera
            Ogre::Vector3 test;
            float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_f)]->getValue();
            float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_e)]->getValue();
            float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_r)]->getValue();
            moveX *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveY *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveZ *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            ogreCamera->moveRelative (Ogre::Vector3 (moveX / 200, moveZ / 500, -moveY / 200));

            moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_s)]->getValue();
            moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_d)]->getValue();
            moveZ = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_w)]->getValue();
            moveX *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveY *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveZ *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            ogreCamera->moveRelative (Ogre::Vector3 (moveX / 200, moveZ / 500, -moveY / 200));

            // Rotate the camera
            float rotX = -SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(0)]->getValue() + 0.5;
            float rotY = SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis(1)]->getValue() - 0.5;
            if (rotX < 0.05 && rotX > -0.05) rotX = 0;
            if (rotY < 0.05 && rotY > -0.05) rotY = 0;
            rotX *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            rotY *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            ogreCamera->yaw (Ogre::Radian(rotX / 500));
            ogreCamera->pitch(Ogre::Radian(rotY / 500));
        } else {
            // Move the camera
            float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_w)]->getValue();
            float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_s)]->getValue();
            float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_r)]->getValue();
            moveX *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveY *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveZ *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            *positionOffset += Vector3d(moveX/1000, moveY/1000, moveZ/1000);

            moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_d)]->getValue();
            moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_e)]->getValue();
            moveZ = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_f)]->getValue();
            moveX *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveY *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            moveZ *= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * 1000;
            *positionOffset += Vector3d(moveX/1000, moveY/1000, moveZ/1000);

        }
    }
}

void Camera::setPositionID (dBodyID positionID)
{
    this->positionID = positionID;
}

void Camera::setTarget (OdeObject * odeObject)
{
    this->targetID = odeObject->getBodyID();
}
