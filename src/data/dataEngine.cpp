
/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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

int DataEngine::start ( WorldData * wrlData, SystemData * sysData )
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ( LOG_INFO, "logData.txt" );

    log.put ( LOG_INFO, "Setting up data pointers..." );
    //we tell the dataEngine where to find/store all the data in memory.
    worldData = wrlData;        //world data is for the simulated world data (cars,
    // track, weather, etc...)
    systemData = sysData;       //system data is for the rest of things (screen
    // resolution, 
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int DataEngine::loadWorldData ( void )
{
    //create 2 rectangles in the world data
    log.put ( LOG_INFO, "Creating an array of 2 rectangles..." );
    worldData->numberOfRectangles = 2;
    worldData->rectangleList = new Rectangle[2];
    log.append ( LOG_INFO, "Ok" );

    //initializating rectangle values
    log.put ( LOG_INFO, "Assigning initial rectangle 1 values..." );
    worldData->rectangleList[0].setVisible ( 1 );
    worldData->rectangleList[0].setSize ( 80, 80 );
    worldData->rectangleList[0].setPosition ( 40, 80 );
    worldData->rectangleList[0].red = 255;
    worldData->rectangleList[0].green = 0;
    worldData->rectangleList[0].blue = 0;
    worldData->rectangleList[0].colorSpeed = 0;
    log.append ( LOG_INFO, "Ok" );

    log.put ( LOG_INFO, "Assigning initial rectangle 2 values..." );
    worldData->rectangleList[1].setVisible ( 1 );
    worldData->rectangleList[1].setSize ( 160, 160 );
    worldData->rectangleList[1].setPosition ( 350, 240 );
    worldData->rectangleList[1].red = 0;
    worldData->rectangleList[1].green = 255;
    worldData->rectangleList[1].blue = 255;
    worldData->rectangleList[0].colorSpeed = 1;
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
    systemData->graphicsData.width = 640;
    log.format ( LOG_INFO, "w%i", systemData->graphicsData.width );
    systemData->graphicsData.height = 480;
    log.format ( LOG_INFO, "h%i", systemData->graphicsData.height );
    systemData->graphicsData.bpp = 0;
    log.format ( LOG_INFO, "b%i", systemData->graphicsData.bpp );
    systemData->graphicsData.flags =
        SDL_SWSURFACE /*SDL_HWSURFACE */  | SDL_DOUBLEBUF | SDL_HWACCEL;
    log.append ( LOG_INFO, "Ok" );

    //set window properties
    systemData->graphicsData.titleLength = 256;
    systemData->graphicsData.iconLength = 16;

    log.put ( LOG_INFO, "Allocating memory for window properties..." );
    systemData->graphicsData.title =
        new char[systemData->graphicsData.titleLength];
    systemData->graphicsData.icon =
        new char[systemData->graphicsData.iconLength];

    log.append ( LOG_INFO, "Ok" );

    log.put ( LOG_INFO, "Setting initial window properties..." );
    strncpy ( systemData->graphicsData.title, "Motorsport",
              systemData->graphicsData.titleLength );
    strncpy ( systemData->graphicsData.icon, "Motorsport",
              systemData->graphicsData.iconLength );
    log.append ( LOG_INFO, "Ok" );

    //set physics parameters
    systemData->physicsData.desiredStepsPerSecond = 500;
    systemData->physicsData.timeStep =
        1000 / systemData->physicsData.desiredStepsPerSecond;

    return ( 0 );
}

int DataEngine::unloadWorldData ( void )
{
    //unload the rectangles from memory
    log.put ( LOG_INFO, "Unloading rectangles from memory..." );
    delete[]( worldData->rectangleList );
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int DataEngine::unloadSystemData ( void )
{
    //unload the rectangles from memory
    log.put ( LOG_INFO, "Unloading window data from memory..." );
    delete[]( systemData->graphicsData.title );
    delete[]( systemData->graphicsData.icon );
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int DataEngine::stop ( void )
{
    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}
