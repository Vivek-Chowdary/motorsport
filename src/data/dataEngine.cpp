
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

DataEngine::DataEngine ( )
{
    //first of all start the logger (automatically logs the start of itself)
    log = new LogEngine ( LOG_INFO, "DAT" );

    log->put ( LOG_INFO, "Setting up data pointers..." );
    //we tell the dataEngine where to find/store all the data in memory.
    worldData = WorldData::getWorldDataPointer(); //world data is for the simulated world data (cars,
    // track, weather, etc...)
    systemData = SystemData::getSystemDataPointer ();       //system data is for the rest of things (screen
    // resolution, 
}

int DataEngine::loadWorldData ( void )
{
    //create the camera and initialize it
    int numberOfCameras = 1;
    log->put ( LOG_INFO, "Creating camera..." );
    for (int i=0; i<numberOfCameras; i++)
    {
        Camera * cameraPointer = new Camera;
        Camera::cameraList.push_back (cameraPointer);
        cameraPointer->setRotateLeft (0);
        cameraPointer->setRotateRight (0);
        cameraPointer->setRotateUp (0);
        cameraPointer->setRotateDown (0);
        cameraPointer->goBack = cameraPointer->goForward = cameraPointer->goLeft = cameraPointer->goRight = false;
    }

    //create 2 cubes in the world data
    int numberOfCubes = 200;
    log->format ( LOG_INFO, "Creating an array of %i cubes...", numberOfCubes );
    for (int i=0; i<numberOfCubes; i++)
    {
        Cube * cubePointer = new Cube;
        Cube::cubeList.push_back (cubePointer);
        cubePointer->setMoveToXPositive(0);
        cubePointer->setMoveToXNegative(0);
        cubePointer->setMoveToYPositive(0);
        cubePointer->setMoveToYNegative(0);
    }

    return ( 0 );
}

int DataEngine::unloadWorldData ( void )
{
    //unload the cubes from memory
    log->put ( LOG_INFO, "Unloading cubes from memory..." );
    Cube::cubeList.clear();
    log->put ( LOG_INFO, "Unloading cameras from memory..." );
    Camera::cameraList.clear();

    return ( 0 );
}

DataEngine::~DataEngine ( void )
{
    //finally stop the log engine
    delete log;
}
