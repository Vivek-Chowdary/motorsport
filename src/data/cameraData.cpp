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
