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

#include "main.hpp"

/******************************************************************************
*
* Functions
*
******************************************************************************/


int sdl_start (LogEngine &log)
{ //initialization functions for SDL
  //returns -2 on warning, -1 on error, 0 on success
/*    if (atexit (SDL_Quit) != 0){
        //warning message
        log.put(LOG_WARNING, "Cannot set exit function");
        return (-2);
    }*/
    return SDL_Init(SDL_INIT_TIMER);
}

void sdl_stop (void)
{ //exit functions for SDL
    SDL_Quit ();
}

void runGuiLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    while (systemData.canGuiLoopRun())
    {
        gui.step();
    }
}
void runSimLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    systemData.lastStatTime = systemData.currentPhysicsTime = SDL_GetTicks();
    while(systemData.canSimLoopRun())
    {
        systemData.currentSimLoopTime = SDL_GetTicks();
        if (systemData.currentSimLoopTime - systemData.lastStatTime >= 1000)
        {
            systemData.graphicsStepsPerSecond = systemData.graphicsSteps;
            systemData.physicsStepsPerSecond = systemData.physicsSteps;
            systemData.graphicsSteps = systemData.physicsSteps = 0;
            systemData.lastStatTime += 1000;
            log.format(LOG_VERBOSE,"Main Loop Stats: graphicsFps=%i - physicsFps=%i", systemData.graphicsStepsPerSecond, systemData.physicsStepsPerSecond);
        }
        //run the physics engine until the game time is in sync with the real time
        while ((systemData.currentSimLoopTime - systemData.currentPhysicsTime) >= systemData.physicsData.timeStep)
        {
            systemData.currentPhysicsTime += systemData.physicsData.timeStep;
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
    }
}

void initializeSimLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    data.loadWorldData ();
    if (graphics.start (&worldData, &systemData))
    {
        log.put(LOG_ERROR, "Could not start the graphics engine.");
        exit (-1);
    }
    input.start (&worldData, &systemData);
    physics.start (&worldData, &systemData);
}
void shutdownSimLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    graphics.stop ();
    data.unloadWorldData ();

    physics.stop ();
    input.stop ();

    systemData.enableGuiLoop();
}
void initializeGuiLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    sdl_start (log);
    gui.start (&worldData, &systemData);
}
void shutdownGuiLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    gui.stop ();
    sdl_stop();
}
void initializeMainLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    log.start(LOG_VERBOSE , "./logMain.txt");

    systemData.guiData.lastMenuIndex = 1; //goto main menu
    systemData.guiData.nextMenuIndex = 1; //goto main menu
    data.start (&worldData, &systemData);
    data.loadSystemData ();
    systemData.enableMainLoop ();
    systemData.enableGuiLoop ();
    systemData.disableSimLoop ();
}
void shutdownMainLoop(WorldData & worldData, SystemData & systemData, LogEngine & log, DataEngine & data, InputEngine & input, GraphicsEngine & graphics, PhysicsEngine & physics, GuiEngine&gui)
{
    data.unloadSystemData ();
    data.stop ();
    log.stop ();
}

int main (int argc, char **argv)
{
    //we declare the 'global' data
    SystemData      systemData;
    WorldData       worldData;
    //we declare the engines
    LogEngine       log;
    DataEngine      data;
    InputEngine     input;
    GraphicsEngine  graphics;
    PhysicsEngine   physics;
    GuiEngine       gui;

    initializeMainLoop(worldData,systemData,log,data,input,graphics,physics,gui);
    while (systemData.canMainLoopRun ())
    {
        if (systemData.canGuiLoopRun ())
        {
            initializeGuiLoop(worldData,systemData,log,data,input,graphics,physics,gui);
            runGuiLoop(worldData,systemData,log,data,input,graphics,physics,gui);
            shutdownGuiLoop(worldData,systemData,log,data,input,graphics,physics,gui);
        }
        if (systemData.canSimLoopRun ())
        {
            initializeSimLoop(worldData,systemData,log,data,input,graphics,physics,gui);
            runSimLoop(worldData,systemData,log,data,input,graphics,physics,gui);
            shutdownSimLoop(worldData,systemData,log,data,input,graphics,physics,gui);
        }
    }
    shutdownMainLoop(worldData,systemData,log,data,input,graphics,physics,gui);

    //and finally back to the OS
    return (0);
}
