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

#include "common/portability/sdl.h"

#include "world.hpp"        //contains the IDF for the simulated/virtual world data
#include "system.hpp"       //contains the IDF for the system data
#include "logEngine.hpp"    //allows to easily log actions
#include "dataEngine.hpp"   //loads/saves data from memory into the virtual world
#include "inputEngine.hpp"  //process the queue of input events
#include "graphicsEngine.hpp"//displays the virtual and system data (sim+gui)
#include "physicsEngine.hpp"//calculates the physics of the world data

/******************************************************************************
*
* Functions
*
******************************************************************************/


int sdl_start (LogEngine *log)
{ //initialization functions for SDL
  //returns -2 on warning, -1 on error, 0 on success
    if (atexit (SDL_Quit) != 0){
        //warning message
        log->put(LOG_WARNING, "Cannot set exit function");
        return (-2);
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
    PhysicsEngine physics;

    //we start the engines (gentlemen, start yo.... err yes you get the idea ;)
    log.start(LOG_VERBOSE , "./logMain.txt");


    log.put(LOG_INFO, "Starting SDL...");
    sdl_start (&log);
    log.append (LOG_INFO, "Ok");

    /* //the physics engine isn't there yet, but it will also need to know
       // where's the world data (aka virtual world objects).
    log.put(LOG_INFO, "Starting the physics engine");
    physics.start (&worldData);
    log.put (true, 2, "Ok");
    */ //see world.hpp for more info on physics engine

    log.put(LOG_INFO, "Starting the data engine...");
    data.start (&worldData, &systemData);
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "Starting the input engine...");
    input.start (&worldData, &systemData);
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "Starting the physics engine...");
    physics.start (&worldData, &systemData);
    log.append (LOG_INFO, "Ok");

    //we must initialize some system data in order to start the graphics engine
    log.put(LOG_INFO, "Loading system data...");
    //data.loadSystemData (640, 480); //ideally, all system data should be read
                                    //from harddisk (xml files, or ini files),
                                    //and allow to be changed in runtime by the
                                    //user. that's why loadSystemData should not
                                    //have any parameters
    data.loadSystemData ();
    log.append (LOG_INFO, "Ok");

    //now we continue starting the engines
    log.put(LOG_INFO, "Starting the graphics engine...");
    if (graphics.start (&worldData, &systemData))
    {
        log.put(LOG_ERROR, "Could not start the graphics engine.");
        exit (-1);
    }
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "All engines started!");

    //we start the simulation --------------------------------------------------
    //first we load the virtual world objects
    log.put(LOG_INFO, "Loading virtual world objects...");
    data.loadWorldData ();
    log.append (LOG_INFO, "Ok");

    //then we set up the main loop
    log.put(LOG_INFO, "Starting simulation (main loop)...");
    systemData.startMainLoop ();
    log.append (LOG_INFO, "Ok");

    //then we finally start the main loop
    systemData.lastSecondTime = systemData.calculatedPhysicsTime = SDL_GetTicks();
    while (!systemData.isLoopDone())
    {
        //mega-verbosity
        log.put(LOG_TRACE,  "Doing a loop: calling engines");

        systemData.currentLoopTime = SDL_GetTicks();

        //check stps per second (updated every 1000 msecs.)
        if (systemData.currentLoopTime - systemData.lastSecondTime >= 1000)
        {
            systemData.graphicsStepsPerSecond = systemData.graphicsSteps;
            systemData.physicsStepsPerSecond = systemData.physicsSteps;
            systemData.graphicsSteps = systemData.physicsSteps = 0;
            systemData.lastSecondTime += 1000;
            log.format(LOG_VERBOSE,"Main Loop Stats: graphicsFps=%i - physicsFps=%i", systemData.graphicsStepsPerSecond, systemData.physicsStepsPerSecond);
        }

        //run the physics engine until the game time is in sync with the real time
        while ((systemData.currentLoopTime - systemData.calculatedPhysicsTime) >= systemData.physicsData.timeStep)
        {
            systemData.calculatedPhysicsTime += systemData.physicsData.timeStep;
            systemData.physicsSteps++;
            //now run the physics engine
            physics.step();
        }

        //sound.step ();
        graphics.step();//currently running at max. possible rate (1 time per loop)
        systemData.graphicsSteps++; //a graphics render is done in every loop
        //data.step (); //if we use a streaming engine i guess...
        input.step();   //currently running at max. possible rate (1 time per loop)
        //net.step ();//this modifies some worldData directly (car positions...)
        //ai.step (); //this works exactly like the input engine,
                      // except that it's the computer who creates the "input"
                      // for the car input parts (car pedals, car st.wheel,...)
        //gui.step (); //how does paraGui work?: this might not be the best way
                       // to do this
    }

    log.put(LOG_INFO, "Simulation interrupted by user request.");

    log.put(LOG_INFO, "Unloading virtual world objects...");
    data.unloadWorldData ();
    log.append (LOG_INFO, "Ok");
    //-------------------------------------------------------simulation finished

    //the simulation has finished: we stop all the engines
    log.put(LOG_INFO, "Stopping the graphics engine...");
    graphics.stop ();
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "Stopping the input engine...");
    input.stop ();
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "Stopping the physics engine...");
    physics.stop ();
    log.append (LOG_INFO, "Ok");

    log.put(LOG_INFO, "Stopping SDL...");
    sdl_stop ();
    log.append (LOG_INFO, "Ok");


    //we must free the memory used for systemdata before stopping the dataEngine
    log.put(LOG_INFO, "Unloading system data...");
    data.unloadSystemData ();
    log.append (LOG_INFO, "Ok");


    log.put(LOG_INFO, "Stopping the data engine...");
    data.stop ();
    log.append (LOG_INFO, "Ok");


    log.put(LOG_INFO, "All engines stopped!");
    log.stop ();

    //and finally back to the OS
    return (0);
}
