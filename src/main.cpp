
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

#include "main.hpp"

int sdl_start ( LogEngine & log )
{                               //initialization functions for SDL
    //returns -2 on warning, -1 on error, 0 on success
    if (atexit (SDL_Quit) != 0){
        //warning message
        log.put(LOG_WARNING, "Cannot set exit function");
        return (-2);
    }
    return SDL_Init ( SDL_INIT_TIMER );
}

void sdl_stop ( void )
{                               //exit functions for SDL
    SDL_Quit (  );
}

int main ( int argc, char **argv )
{
    //we declare the 'global' data
    new SystemData();
    new WorldData();
    DataEngine * data = new DataEngine();
    data->loadSystemData (  );
    data->loadWorldData (  );
    //we declare the engines
    LogEngine * log = new LogEngine();
    InputEngine * input = new InputEngine();
    GraphicsEngine * graphics = new GraphicsEngine();
    PhysicsEngine * physics = new PhysicsEngine();
    GuiEngine * gui = new GuiEngine();

    log->start ( LOG_VERBOSE, "./logMain.txt" );
    //sdl_start ( *log );
    
    SystemData * systemData = SystemData::getSystemDataPointer();
    systemData->enableMainLoop (  );
    systemData->lastStatTime = systemData->currentPhysicsTime =
        SDL_GetTicks (  );
    while ( systemData->canMainLoopRun (  ) )
    {
        systemData->currentMainLoopTime = SDL_GetTicks (  );
        if ( systemData->currentMainLoopTime - systemData->lastStatTime >= 1000 )
        {
            systemData->graphicsStepsPerSecond = systemData->graphicsSteps;
            systemData->physicsStepsPerSecond = systemData->physicsSteps;
            systemData->graphicsSteps = systemData->physicsSteps = 0;
            systemData->lastStatTime += 1000;
            log->format ( LOG_VERBOSE,
                          "Main Loop Stats: graphicsFps=%i - physicsFps=%i",
                          systemData->graphicsStepsPerSecond,
                          systemData->physicsStepsPerSecond );
        }
        //run the physics engine until the game time is in sync with the real time
        while ( ( systemData->currentMainLoopTime -
                  systemData->currentPhysicsTime ) >=
                systemData->physicsData.timeStep )
        {
            systemData->currentPhysicsTime += systemData->physicsData.timeStep;
            systemData->physicsSteps++;
            //now run the physics engine
            physics->step (  );
        }
        //sound.step ();
        gui->step (  );
        graphics->step (  );    //currently running at max. possible rate (1 time per loop)
        systemData->graphicsSteps++;    //a graphics render is done in every loop
        //data.step (); //if we use a streaming engine i guess...
        input->step (  );       //currently running at max. possible rate (1 time per loop)
        //net.step ();//this modifies some worldData directly (car positions...)
        //ai.step (); //this works exactly like the input engine,
        // except that it's the computer who creates the "input"
        // for the car input parts (car pedals, car st.wheel,...)
    }

    delete graphics;
    delete physics;
    delete input;
    data->unloadWorldData (  );
    delete gui;
    data->unloadSystemData (  );
    delete data;
    log->stop (  );
    //sdl_stop (  );

    //and finally back to the OS
    return ( 0 );
}
