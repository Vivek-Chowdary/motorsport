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
#include "inputEngine.hpp"
#ifdef WIN32
	#include "sdl.h"
#else										
	#include <SDL/SDL.h>
#endif


/******************************************************************************
*
* Functions
*
******************************************************************************/

int InputEngine::start (WorldData *wrlData, SystemData *sysData)
{
    //first of all start the logger (automatically logs the start of itself)
    log.start (false, 3, "logInput.txt");

    //we store the pointers to the 'global' data, so that we access it easily.
    //we might have to change this and only allow changing data through
    // some dataEngine functions, which will act as an interface to the data
    // stored in memory with the IDF.
    log.put (false, 2, "Setting up data pointers...");
    inputData = &(sysData->inputData);
    systemData = sysData;
    worldData = wrlData;
    log.put (true, 2, "Ok");

	return (0);
}


int InputEngine::step (void)   //processes user input queue
{
    //mega-verbosity
    //log.put (true, 3, "Doing an step: checking for input events...");
    
    SDL_Event event; //used to store the current event in the queue
    
    //sdl has a queue of events. this queue of events is processed as 'steps',
    // called from the mainLoop. every step of the inputEngine will see what
    // the user wants (what input events has he made), and act accordingly.
    // there can be more than one event in each step, that's why there's a
    // 'while' (after all, it's a queue ;)
    while (SDL_PollEvent (&event))
    {
        switch (event.type) //identify the kind of event
        {
            case SDL_KEYDOWN:
                //this is the user pressing a key in the keyboard
                log.put (false, 3, "New SDL_KEYDOWN event: finding out what key's been pressed.");
                processInput (event.key.keysym.sym);
                break;
            case SDL_QUIT:
                //this can be the user cliking to close the window
                log.put (false, 3, "New SDL_QUIT event: notifying to stop mainLoop...");
                systemData->stopMainLoop ();
                log.put (true, 3, "Ok");
                break;
            default:
                //this can be mouse movement, joystick input,...
                //log.put (1, "Processing an unknown event: doing nothing");
                break;
        }
    }
    //finished processing the events queue
    
    return (0);
}

int InputEngine::stop (void)
{

    //finally stop the log engine
    log.stop ();
	
	return (0);
}


void InputEngine::processInput (SDLKey keySymbol)
{
    //in the future, the player will be able to assign, at least, *which* events
    // (keypress, joystick axis modification....) do *what* in the virtual world
    // data.
    // if people want, we can also allow the user to assign *which* events do
    // *what* in the system data. this means the user could be able to navigate
    // through the menus with whatever they want. for example, instead of using
    // the mouse to click gui buttons, the use the steering wheel to select the
    // button and the gas pedal to 'click' the button.
    switch (keySymbol)
    {
        //the user wants to modify system data using the inputEngine. in this
        // example, he notifies to the main program that he wants to exit the
        // simulation, therefore exiting the program
        // when we get the real gui (paragui?) working, it will notify that the
        // user wants to go to X menu, or go to the replay system, or pause the
        // simulation, save the game, etc...
        //we indicate that the user wants to exit the simulation
        case SDLK_RETURN:
        case SDLK_ESCAPE:
            log.put (false, 3, "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop...");
            systemData->stopMainLoop ();
            log.put (true, 3, "Ok");
            break;
        
        //the user wants to modify world data using the inputEngine. in this
        // example, the inputEngine makes the rectangles visible/invisible.
        // when we get ode working, the inputEngine will change some properties
        // of the world objects, which in that case will be for example
        // worldObjectList.gasPedal.modifyAngle (angle+10)
        // the same goes for steering wheel, etc.
        // the physics engine will *then* calculate the rest of reactions of
        // other objects of te virtual world. in this case, it would put more
        // air+fuel in the cylinders, which would therefore accelerate the rpms,
        // therefore modifying.... etc...
        case SDLK_RIGHT:
            log.put (false, 3, "Processing a SDLK_RIGHT keypress...");
            worldData->rectangleList[0].setVisible(0);
            log.put (true, 3, "rectangle0 is now invisible.");
            break;

        case SDLK_LEFT:
            log.put (false, 3, "Processing a SDLK_LEFT keypress...");
            worldData->rectangleList[0].setVisible(1);
            log.put (true, 3, "rectangle0 is now visible.");
            break;

        case SDLK_UP:
            log.put (false, 3, "Processing a SDLK_UP keypress...");
            worldData->rectangleList[1].setVisible(0);
            log.put (true, 3, "rectangle1 is now invisible.");
            break;

        case SDLK_DOWN:
            log.put (false, 3, "Processing a SDLK_DOWN keypress...");
            worldData->rectangleList[1].setVisible(1);
            log.put (true, 3, "rectangle1 is now visible.");
            break;

        //this is left for non-assigned input events.
        case 'G':
            log.put (false, 3, "Processing a 'G' keypress: doing nothing...");
            // something
            log.put (true, 3, "Ok");
            break;

        case 'S':
            log.put (false, 3, "Processing a 'S' keypress: doing nothing...");
            // something
            log.put (true, 3, "Ok");
            break;

        default:
            log.put (false, 3, "Processing an unknown keypress: doing nothing...");
            // something
            log.put (true, 3, "Ok");
            break;
    }
}
