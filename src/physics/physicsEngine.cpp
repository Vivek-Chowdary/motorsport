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

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include "../system.hpp"
#include "../world.hpp"
#include "physicsEngine.hpp"
#include <math.h>

/******************************************************************************
*
* Functions
*
******************************************************************************/

int PhysicsEngine::start (WorldData *wrlData, SystemData *sysData)
{
    //first of all start the logger (automatically logs the start of itself)
    log.start(2, "logPhysics.txt");
    
    //get the direction of the graphics data
    log.put(2, "Setting up data pointers...");
    physicsData = &(sysData->physicsData);
    worldData = wrlData;
    log.append (2, "Ok");
    
	return (0);
}


int PhysicsEngine::step (void)
//makes the graphics engine draw one frame
{
    //mega-verbosity
    log.put(4, "Doing an step: calculating a physics step");

    //current physics engine just changes the color of the triangles
    //i could have implemented moving rectangles around the screen, with gravity
    // or whatever, but well, this is just a sample physics engine :)
    for (int currentRectangle = 0; currentRectangle < worldData->numberOfRectangles; currentRectangle++)
    {
        //do we have to increase or to decrease current color values?
        if (worldData->rectangleList[currentRectangle].colorSpeed == 0)
        {
            worldData->rectangleList[currentRectangle].red += 1 + Uint8 (physicsData->timeStep * 0.7);
            worldData->rectangleList[currentRectangle].green += 1 + Uint8 (physicsData->timeStep * 0.8);
            worldData->rectangleList[currentRectangle].blue += 1 + Uint8 (physicsData->timeStep * 0.9);
        }else{
            worldData->rectangleList[currentRectangle].red -= 1 + Uint8 (physicsData->timeStep * 0.7);
            worldData->rectangleList[currentRectangle].green -= 1 + Uint8 (physicsData->timeStep * 0.8);
            worldData->rectangleList[currentRectangle].blue -= 1 + Uint8 (physicsData->timeStep * 0.9);
        }
    }
    
    return (0);
}

int PhysicsEngine::stop (void)
{
    //finally stop the log engine
    log.stop ();
	
	return (0);
}
