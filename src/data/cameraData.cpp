/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
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

int Camera::instancesCount = 0;

Camera::Camera (XERCES_CPP_NAMESPACE::DOMNode * n)
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

void Camera::updateOgrePosition ()
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
}
void Camera::updateOgreTarget ()
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
}

