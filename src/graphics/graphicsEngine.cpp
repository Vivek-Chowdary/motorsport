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
#include "graphicsEngine.hpp"
#include <stdlib.h>


/******************************************************************************
*
* Functions
*
******************************************************************************/

int GraphicsEngine::start (WorldData *wrlData, SystemData *sysData)
{
    //first of all start the logger (automatically logs the start of itself)
    log.start(3, "logGraphics.txt");
    
    //get the direction of the graphics data
    log.put(2, "Setting up data pointers...");
    graphicsData = &(sysData->graphicsData);
    worldData = wrlData;
    systemData = sysData;
    log.append (2, "Ok");
    
	//initialization of SDL_VIDEO
	log.put(2, "Initializing SDL Video...");	
	if (SDL_Init (SDL_INIT_VIDEO) == -1)
    {
        log.put(0, "SDL Video initialization falied:");
        log.append (0, SDL_GetError ());
        return (-1);
    }
    log.append (2, "Ok");

    //setting video mode
	log.put(2, "Setting SDL Video Mode...");
	(graphicsData->screen) = SDL_SetVideoMode (graphicsData->width,
                                               graphicsData->height,
                                               graphicsData->bpp,
                                               graphicsData->flags);
    if (graphicsData->screen == NULL)
    {
        log.put(0, "Couldn't set desired SDL Video Mode:");
        log.append (0, SDL_GetError ());
        return (-1);
    }
    log.append (2, "Ok");

    log.put(2, "Setting SDL window color depth...");
    graphicsData->fmt = graphicsData->screen->format;
    log.append (2, "Ok");
	
	return (0);
}


int GraphicsEngine::step (void)
//makes the graphics engine draw one frame
{
    
    //write engines rate stats on window title (if they have just been updated)
    if (!systemData->graphicsSteps)
    {
        log.put(3, "Doing an step: updating engine rates");
        //be careful with the string size of char* title!!
        sprintf(graphicsData->title, "Motorsport [ graphicsFps=%3i | physicsFps=%3i(%3ifps|%3ims) ]", systemData->graphicsStepsPerSecond, systemData->physicsStepsPerSecond,  systemData->physicsData.desiredStepsPerSecond, systemData->physicsData.timeStep);
        SDL_WM_SetCaption (graphicsData->title,graphicsData->icon);
        log.append (3, "Ok");
    }

    log.put(4, "Doing an step: drawing world objects");

/*    //blank the screen
    log.put(4, "Doing an step: blanking the screen...");
    if (SDL_FillRect (graphicsData->screen,
                      NULL,
                      SDL_MapRGB (graphicsData->fmt, (Uint8) 0, (Uint8) 0, (Uint8) 0)
                     ) == -1)
    {
        log.put(0, "Error while executing SDL_FillRect");
    }
    log.append (4, "Ok");
*/
	//draw all the rectangles in worldData
    log.put(4, "Drawing rectangles on screen...");
    for (int currentRectangle = 0; currentRectangle < worldData->numberOfRectangles; currentRectangle++)
    {
        //only draw the objects the user wants to see(visible rectangles)
        if (worldData->rectangleList[currentRectangle].isVisible ())
        {
            if (SDL_FillRect (graphicsData->screen,
                              worldData->rectangleList[currentRectangle].sdlRectangle (),
                              SDL_MapRGB (graphicsData->fmt,
                                          worldData->rectangleList[currentRectangle].red,
                                          worldData->rectangleList[currentRectangle].green,
                                          worldData->rectangleList[currentRectangle].blue)
                              ) == -1)
            {
                //this is just an example of warning. this should prolly be an error(level 0)
                log.put(1, "Error while executing SDL_FillRect");
            }
        }
    }
    log.append (4, "Ok");
    
    //update the screen
    SDL_UpdateRect (graphicsData->screen, 0, 0, 640, 480);

    return (0);
}

int GraphicsEngine::stop (void)
{
    //finally stop the log engine
    log.stop ();
	
	return (0);
}
