
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
    log.start ( LOG_INFO, "logData.txt" );

    log.put ( LOG_INFO, "Setting up data pointers..." );
    //we tell the dataEngine where to find/store all the data in memory.
    worldData = WorldData::getWorldDataPointer(); //world data is for the simulated world data (cars,
    // track, weather, etc...)
    systemData = SystemData::getSystemDataPointer ();       //system data is for the rest of things (screen
    // resolution, 
    log.append ( LOG_INFO, "Ok" );
}

int DataEngine::loadWorldData ( void )
{
    //create the camera1 and initialize it
    log.put ( LOG_INFO, "Creating camera1..." );
    worldData->camera1 = new MospCamera;
    worldData->camera1->rotateDown =
        worldData->camera1->rotateUp =
        worldData->camera1->rotateLeft =
        worldData->camera1->rotateRight =
        worldData->camera1->goBack =
        worldData->camera1->goForward =
        worldData->camera1->goLeft = worldData->camera1->goRight = false;

    //create 2 cubes in the world data
    log.put ( LOG_INFO, "Creating an array of 2 cubes..." );
    worldData->numberOfCubes = 5;
    worldData->cubeList = new Cube[worldData->numberOfCubes];
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int DataEngine::loadSystemData ( void )
{
    //initialize main loop data
    systemData->physicsSteps = 0;
    systemData->physicsStepsPerSecond = 0;
    systemData->graphicsSteps = 0;
    systemData->graphicsStepsPerSecond = 0;

    //set screen properties
    log.put ( LOG_INFO, "Setting screen properties..." );
    systemData->graphicsData.enableStatistics (  );
    systemData->graphicsData.anisotropic = 1;
    systemData->graphicsData.filtering = Ogre::TFO_BILINEAR;
    systemData->graphicsData.width = 1024;
    log.format ( LOG_INFO, "w%i", systemData->graphicsData.width );
    systemData->graphicsData.height = 768;
    log.format ( LOG_INFO, "h%i", systemData->graphicsData.height );
    systemData->graphicsData.bpp = 0;
    log.format ( LOG_INFO, "b%i", systemData->graphicsData.bpp );

    log.put ( LOG_INFO, "Setting physics system data..." );
    systemData->physicsData.desiredStepsPerSecond = 500;
    systemData->physicsData.timeStep =
        1000 / systemData->physicsData.desiredStepsPerSecond;

    return ( 0 );
}

int DataEngine::unloadWorldData ( void )
{
    //unload the cubes from memory
    log.put ( LOG_INFO, "Unloading cubes from memory..." );
    delete[]( worldData->cubeList );
    log.put ( LOG_INFO, "Unloading camera1 from memory..." );
    delete ( worldData->camera1->ogreCamera );
    delete ( worldData->camera1 );
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int DataEngine::unloadSystemData ( void )
{
    log.put ( LOG_INFO, "Unloading window data from memory..." );
    delete ( systemData->graphicsData.ogreWindow );
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

DataEngine::~DataEngine ( void )
{
    //finally stop the log engine
    log.stop (  );
}
