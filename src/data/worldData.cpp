
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

#include "world.hpp"

World *World::worldPointer = NULL;

World *World::getWorldPointer ()
{
    if (worldPointer == 0)
    {
        worldPointer = new World(/*"../data/defaultWorld.xml"*/);
    }
    return (worldPointer);
}

void World::processXmlRootNode (DOMNode * n)
{
    //empty for now
}

World::World ()
{
    log = new LogEngine(LOG_TRACE, "WRL");
    worldPointer = this;
    
    log->put (LOG_INFO, "Creating ODE world");
    worldID = dWorldCreate ();
    spaceID = dHashSpaceCreate (0);
    jointGroupID = dJointGroupCreate (0);

    log->put ( LOG_INFO, "Setting ODE world gravity");
    dWorldSetGravity (worldID, 0,0,-0.000098);
    
    log->put (LOG_INFO, "Creating the ode plane");
    dCreatePlane (spaceID, 0, 0, 1, 0);

    // Create the bodies
    int numberOfBodies = 2;
    log->format (LOG_INFO, "Creating an array of %i bodies", numberOfBodies);
    for (int i = 0; i < numberOfBodies; i++)
    {
        log->format (LOG_VERBOSE, "Adding body number %i", i);
        const int separation = 5;
        Body *bodyPointer;
        bodyPointer = new Body ("../data/body.xml");
        bodyPointer->setPosition (- separation - i / 10 % 10 * separation, - separation - i % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)));
        bodyList.push_back (bodyPointer);
    }

    // Create the cubes
    int numberOfCubes = 20;
    log->format (LOG_INFO, "Creating an array of %i cubes", numberOfCubes);
    for (int i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_VERBOSE, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ("../data/cube.xml");
        cubePointer->setPosition (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation);
        cubeList.push_back (cubePointer);
    }

    // create the camera and initialize it
    int numberOfCameras = 4;
    log->format (LOG_INFO, "Creating %i cameras", numberOfCameras);
    for (int i = 0; i < numberOfCameras; i++)
    {
        Camera *cameraPointer = new Camera (i, -20, -20, 5, 0, 0, 0);
        cameraList.push_back (cameraPointer);
    }
    log->put (LOG_INFO, "Setting camera viewport");
    Ogre::Viewport * vp = SystemData::getSystemDataPointer()->ogreWindow->addViewport (cameraList[0]->ogreCamera);
    log->put (LOG_INFO, "Setting bg color");
    vp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));

    // Create the skybox
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    SystemData::getSystemDataPointer()->ogreSceneManager->setSkyBox (true, "skyboxMaterial", 5000, true, rotationToZAxis);
}

World::~World ()
{
    // unload the bodies from memory
    log->put (LOG_INFO, "Unloading bodies from memory...");
    int size = bodyList.size ();
    for (int i = 0; i < size; i++)
    {
        delete bodyList[i];
    }
    bodyList.clear ();
    
    // unload the cubes from memory
    log->put (LOG_INFO, "Unloading cubes from memory...");
    size = cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        delete cubeList[i];
    }
    cubeList.clear ();
    
    // unload the cameras from memory
    log->put (LOG_INFO, "Unloading cameras from memory...");
    size = cameraList.size ();
    for (int i = 0; i < size; i++)
    {
        delete cameraList[i];
    }
    cameraList.clear ();

    log->put (LOG_INFO, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->put (LOG_INFO, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    log->put (LOG_INFO, "Destroying ODE joints group");
    dJointGroupDestroy (jointGroupID);

    worldPointer = NULL;
}
