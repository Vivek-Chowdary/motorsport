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

#include <stdlib.h>
#ifdef WIN32
	#include "SDL/sdl.h"
#else										
	#include <SDL/SDL.h>
#endif

//#include "main.hpp"
#include "world.hpp"        //contains the IDF for the simulated/virtual world data
#include "system.hpp"       //contains the IDF for the system data
#include "logEngine.hpp"    //allows to easily log actions
#include "dataEngine.hpp"   //loads/saves data from memory into the virtual world
#include "inputEngine.hpp"  //process the queue of input events
#include "graphicsEngine.hpp"//displays the virtual and system data (sim+gui)

/******************************************************************************
*
* Functions
*
******************************************************************************/


int sdl_start (LogEngine *log)
{ //initialization functions for SDL
  	if (atexit (SDL_Quit) != 0){
        //warning message
        log->put(1,"Cannot set exit function");
    }
	return (0);
}

void sdl_stop (void)
{ //exit functions for SDL
    SDL_Quit ();
}

int main (int argc, char **argv)
{
    //we declare the 'global' data
    SystemData systemData;
    WorldData worldData;

	//we declare the engines
	LogEngine log;
	DataEngine data;
	InputEngine input;
	GraphicsEngine graphics;

	//we start the engines (gentlemen, start yo.... err yes you get the idea ;)
	log.start (-1, "./logMain.txt", false);

	log.put (0, "Starting SDL...");
	sdl_start (&log);
    log.put (-1,"Ok");

    /* //the physics engine isn't there yet, but it will also need to know
       // where's the world data (aka virtual world objects).
    log.put (0, "Starting the physics engine");
    physics.start (&worldData);
    log.put (-1,"Ok");
    */ //see world.hpp for more info on physics engine

    log.put (0, "Starting the data engine...");
	data.start (&worldData, &systemData);
    log.put (-1,"Ok");

    log.put (0, "Starting the input engine...");
	input.start (&(systemData.inputData),&systemData,&worldData);
    log.put (-1,"Ok");

    //we must initialize some system data in order to start the graphics engine
    log.put (0, "Loading system data...");
    //data.loadSystemData (640, 480); //ideally, all system data should be read
                                    //from harddisk (xml files, or ini files),
                                    //and allow to be changed in runtime by the
                                    //user. that's why loadSystemData should not
                                    //have any parameters
    data.loadSystemData ();
    log.put (-1,"Ok");
	
    //now we continue starting the engines
    log.put (0, "Starting the graphics engine...");
	if (graphics.start (&(systemData.graphicsData),&worldData)) exit (-1);
    log.put (-1,"Ok");

    log.put (0, "All engines started!");

    //we start the simulation --------------------------------------------------
    //first we load the virtual world objects
    log.put (0, "Loading virtual world objects...");
    data.loadWorldData ();
    log.put (-1,"Ok");

    //then we set up the main loop
    log.put (0, "Starting simulation (main loop)...");
    systemData.startMainLoop ();
    log.put (-1,"Ok");

    //then we finally start the main loop
    while (!systemData.isLoopDone ())
	{
        //mega-verbosity
        //log.put (0, "Doing a loop: calling gfx and input engines");

		graphics.step ();
		input.step ();
	}
    log.put (0, "Stopping simulation (main loop)...");
    log.put (-1,"Ok");

    log.put (0, "Unloading virtual world objects...");
	data.unloadWorldData ();
    log.put (-1,"Ok");
    //-------------------------------------------------------simulation finished
    
	//the simulation has finished: we stop all the engines
	log.put (0, "Stopping the graphics engine...");
	graphics.stop ();
    log.put (-1,"Ok");

	log.put (0, "Stopping the input engine...");
	input.stop ();
    log.put (-1,"Ok");

    log.put (0, "Stopping SDL...");
    sdl_stop ();
    log.put (-1,"Ok");


    //we must free the memory used for systemdata before stopping the dataEngine
    log.put (0, "Unloading system data...");
    data.unloadSystemData ();
    log.put (-1,"Ok");


	log.put (0, "Stopping the data engine...");
	data.stop ();
    log.put (-1,"Ok");


    log.put (0, "All engines stopped!");
    log.stop ();

	//and finally back to the OS
	return (0);
}
