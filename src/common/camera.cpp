/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "xmlParser.hpp"
#include "ode/ode.h"
#include "log/logEngine.hpp"
#include "area.hpp"
#include "vector3d.hpp"

int Camera::instancesCount = 0;

Camera::Camera (XERCES_CPP_NAMESPACE::DOMNode * n)
    :WorldObject("Camera")
{
    instancesCount ++;
    processXmlRootNode (n);
}

void Camera::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startGraphics ();
    startPhysics (n);
    startInput ();
}

Camera::~Camera ()
{
    instancesCount--;
    stopGraphics();
    stopInput();
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

void Camera::startGraphics ()
{
    char name[20];
    sprintf (name, "Camera%i", instancesCount);

    ogreCamera = SystemData::getSystemDataPointer ()->ogreSceneManager->createCamera (name);
    ogreCamera->setFixedYawAxis (true, Ogre::Vector3 (0, 0, 1));
    ogreCamera->setNearClipDistance (0.100);
//    ogreCamera->setAutoAspectRatio(true);
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

void Camera::stopGraphics ()
{

}
void Camera::startInput ()
{

}

void Camera::stepInput ()
{

}

void Camera::stopInput ()
{

}
void Camera::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log->__format (LOG_DEVELOPER, "Creating a camera");
    positionOffset = new Vector3d (0, 0, 0);
    targetOffset = new Vector3d (0, 0, 0);
    positionID = 0;
    targetID = 0;
    index = "0";
    isFree = false;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                assignXmlString (index, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the position index: %s", index.c_str());
            }
            if (attribute == "position")
            {
                assignXmlString (attribute, attNode->getValue()); 
                log->__format (LOG_CCREATOR, "Found the position: %s", attribute.c_str());
                *positionOffset = Vector3d(attribute);
            }
            if (attribute == "target")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the target: %s", attribute.c_str());
                *targetOffset = Vector3d(attribute);
            }
        }
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

void Camera::setTargetID (dBodyID targetID)
{
    this->targetID = targetID;
}
