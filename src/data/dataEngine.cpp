
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
    log->put ( LOG_INFO, "Creating camera..." );
    worldData->camera = new Camera;
    worldData->camera->setRotateLeft (0);
    worldData->camera->setRotateRight (0);
    worldData->camera->setRotateUp (0);
    worldData->camera->setRotateDown (0);
    worldData->camera->goBack = worldData->camera->goForward = worldData->camera->goLeft = worldData->camera->goRight = false;

    //create 2 cubes in the world data
    worldData->numberOfCubes = 200;
    log->format ( LOG_INFO, "Creating an array of %i cubes...", worldData->numberOfCubes );
    worldData->cubeList = new Cube[worldData->numberOfCubes];
    for (int i=0;i<worldData->numberOfCubes;i++)
    {
        worldData->cubeList[i].setMoveToXPositive(0);
        worldData->cubeList[i].setMoveToXNegative(0);
        worldData->cubeList[i].setMoveToYPositive(0);
        worldData->cubeList[i].setMoveToYNegative(0);
    }

    return ( 0 );
}

int DataEngine::unloadWorldData ( void )
{
    //unload the cubes from memory
    log->put ( LOG_INFO, "Unloading cubes from memory..." );
    delete[]( worldData->cubeList );
    log->put ( LOG_INFO, "Unloading camera1 from memory..." );
    delete ( worldData->camera->ogreCamera );
    delete ( worldData->camera );

    return ( 0 );
}

DataEngine::~DataEngine ( void )
{
    //finally stop the log engine
    delete log;
}
