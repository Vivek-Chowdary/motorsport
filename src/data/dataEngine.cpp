
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

#include "dataEngine.hpp"

DataEngine::DataEngine ()
{
    // first of all start the logger (automatically logs the start of itself)
    DataData *data = new DataData;
    data->data = this;
    processXmlFile ("dataConfig.xml", &DataEngine::processDataConfigFile, (void *) data);

    log = new LogEngine (data->localLogLevel, data->localLogName);
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    log->put (LOG_INFO, "Setting up data pointers...");
    // we tell the dataEngine where to find/store all the data in memory.
    worldData = WorldData::getWorldDataPointer ();  // world data is for the simulated world data (cars,
    // track, weather, etc...)
    systemData = SystemData::getSystemDataPointer ();   // system data is for the rest of things (screen
    // resolution, 

    log->put (LOG_INFO, "Unloading temporary parsing data from memory");
    delete[](data->localLogName);
    delete data;
}

int DataEngine::loadWorldData (void)
{
    // create the camera and initialize it
    int numberOfCameras = 4;
    log->format (LOG_INFO, "Creating %i cameras", numberOfCameras);
    for (int i = 0; i < numberOfCameras; i++)
    {
        Camera *cameraPointer = new Camera (i, -2000, -2000, 500, 0, 0, 0);
        Camera::cameraList.push_back (cameraPointer);
    }
    log->put (LOG_INFO, "Setting camera viewport");

    Ogre::Viewport * vp = systemData->ogreWindow->addViewport (Camera::cameraList[0]->ogreCamera);
    vp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));

    // Create the skybox
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    systemData->ogreSceneManager->setSkyBox (true, "skybox", 5000, true, rotationToZAxis);

    // Create the cubes
    int numberOfCubes = 200;
    log->format (LOG_INFO, "Creating an array of %i cubes", numberOfCubes);
    for (int i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_VERBOSE, "Adding cube number %i", i);
        const int separation = 250;
        Cube *cubePointer;
        cubePointer = new Cube (i, i % 10 * separation, i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)));
        dBodySetAngularVel (cubePointer->cubeID, float (random () % 10) / 10000.0, float (random () % 10) / 10000.0, float (random () % 10) / 10000.0);
        Cube::cubeList.push_back (cubePointer);
    }

    return (0);
}

int DataEngine::unloadWorldData (void)
{
    // unload the cubes from memory
    log->put (LOG_INFO, "Unloading cubes from memory...");
    Cube::cubeList.clear ();
    log->put (LOG_INFO, "Unloading cameras from memory...");
    Camera::cameraList.clear ();

    return (0);
}

DataEngine::~DataEngine (void)
{
    // finally stop the log engine
    delete log;

//    processConfigFile ( "mainConfig.xml" );
}
