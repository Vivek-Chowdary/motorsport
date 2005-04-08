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
#include "vector3d.hpp"

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
    Vector3d position (updateOgrePosition());
    Vector3d target (updateOgreTarget());
    double distance = target.distance(position);
    const double maxD = 150.;
    const double minD = 15.;
    const Ogre::Radian minF(5.  * 3.14159 / 180.);
    const Ogre::Radian maxF(45. * 3.14159 / 180.);
    if (distance > maxD)
    {
        ogreCamera->setFOVy (minF);
    } else if (distance < minD)
    {
        ogreCamera->setFOVy (maxF);
    } else {
        ogreCamera->setFOVy ( maxF - ( ( ( distance - minD ) / ( maxD - minD ) ) * (maxF - minF) ) );
    }
}

void Camera::stopGraphics ()
{

}
