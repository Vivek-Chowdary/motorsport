
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

#include "inputEngine.hpp"

int InputEngine::start ( WorldData * wrlData, SystemData * sysData )
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ( LOG_VERBOSE, "logInput.txt" );

    //we store the pointers to the 'global' data, so that we access it easily.
    //we might have to change this and only allow changing data through
    // some dataEngine functions, which will act as an interface to the data
    // stored in memory with the IDF.
    log.put ( LOG_INFO, "Setting up data pointers..." );
    inputData = &( sysData->inputData );
    systemData = sysData;
    worldData = wrlData;
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int InputEngine::step ( void )  //processes user input queue
{
    //mega-verbosity
    //log.put (LOG_VERBOSE, "Doing an step: checking for input events...");

    SDL_Event event;            //used to store the current event in the queue

    //sdl has a queue of events. this queue of events is processed as 'steps',
    // called from the mainLoop. every step of the inputEngine will see what
    // the user wants (what input events has he made), and act accordingly.
    // there can be more than one event in each step, that's why there's a
    // 'while' (after all, it's a queue ;)
    while ( SDL_PollEvent ( &event ) )
    {
        switch ( event.type )   //identify the kind of event
        {
        case SDL_KEYDOWN:
            //this is the user pressing a key in the keyboard
            log.put ( LOG_VERBOSE,
                      "New SDL_KEYDOWN event: finding out what key's been pressed." );
            processInput ( event.key.keysym.sym );
            break;
        case SDL_QUIT:
            //this can be the user cliking to close the window
            log.put ( LOG_VERBOSE,
                      "New SDL_QUIT event: notifying to stop mainLoop..." );
            systemData->disableSimLoop (  );
            log.append ( LOG_VERBOSE, "Ok" );
            break;
        default:
            //this can be mouse movement, joystick input,...
            //log.put (LOG_WARNING, "Processing an unknown event: doing nothing");
            break;
        }
    }
    //finished processing the events queue

    return ( 0 );
}

int InputEngine::stop ( void )
{

    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}

void InputEngine::processInput ( SDLKey keySymbol )
{
    // in the future, the player will be able to assign, at least, *which* events
    // (keypress, joystick axis modification....) do *what* in the virtual world
    // data.
    // if people want, we can also allow the user to assign *which* events do
    // *what* in the system data. this means the user could be able to navigate
    // through the menus with whatever they want. for example, instead of using
    // the mouse to click gui buttons, the use the steering wheel to select the
    // button and the gas pedal to 'click' the button.
    switch ( keySymbol )
    {
        // the user wants to modify system data using the inputEngine. in this
        // example, he notifies to the main program that he wants to exit the
        // simulation, therefore exiting the program
        // when we get the real gui (paragui?) working, it will notify that the
        // user wants to go to X menu, or go to the replay system, or pause the
        // simulation, save the game, etc...
        // we indicate that the user wants to exit the simulation
    case SDLK_RETURN:
    case SDLK_ESCAPE:
        log.put ( LOG_VERBOSE,
                  "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop..." );
        systemData->disableSimLoop (  );
        log.append ( LOG_VERBOSE, "Ok" );
        break;

        // the user wants to modify world data using the inputEngine. in this
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
        log.put ( LOG_VERBOSE, "Processing a SDLK_RIGHT keypress..." );
        worldData->rectangleList[0].setVisible ( 0 );
        log.append ( LOG_VERBOSE, "rectangle0 is now invisible." );
        break;

    case SDLK_LEFT:
        log.put ( LOG_VERBOSE, "Processing a SDLK_LEFT keypress..." );
        worldData->rectangleList[0].setVisible ( 1 );
        log.append ( LOG_VERBOSE, "rectangle0 is now visible." );
        break;

    case SDLK_UP:
        log.put ( LOG_VERBOSE, "Processing a SDLK_UP keypress..." );
        worldData->rectangleList[1].setVisible ( 0 );
        log.append ( LOG_VERBOSE, "rectangle1 is now invisible." );
        break;

    case SDLK_DOWN:
        log.put ( LOG_VERBOSE, "Processing a SDLK_DOWN keypress..." );
        worldData->rectangleList[1].setVisible ( 1 );
        log.append ( LOG_VERBOSE, "rectangle1 is now visible." );
        break;

    case SDLK_SPACE:
        log.put ( LOG_VERBOSE, "Processing a SDLK_SPACE keypress..." );
        worldData->rectangleList[0].colorSpeed =
            !( worldData->rectangleList[0].colorSpeed );
        worldData->rectangleList[1].colorSpeed =
            !( worldData->rectangleList[1].colorSpeed );
        log.append ( LOG_VERBOSE, "rectangles colorSpeed are now reversed." );
        break;

    case SDLK_KP_MINUS:
        log.put ( LOG_VERBOSE, "Processing a SDLK_KP_MINUS keypress..." );
        //modify the physics engine rate
        //if current desired fps is below 37, it's better to decrease the fps (frames/sec.)...
        if ( systemData->physicsData.desiredStepsPerSecond < 37 )
        {
            //don't let the rate fall below 1 frame per second (or don't allow more than 1000msecs. step)
            if ( !--systemData->physicsData.desiredStepsPerSecond )
            {
                systemData->physicsData.desiredStepsPerSecond++;
            }
            systemData->physicsData.timeStep =
                1000 / systemData->physicsData.desiredStepsPerSecond;
        }
        else
        {                       //...otherwise, it's better to increase the timestep (msecs.)
            systemData->physicsData.timeStep++;
            systemData->physicsData.desiredStepsPerSecond =
                1000 / systemData->physicsData.timeStep;
        }
        log.append ( LOG_VERBOSE, "physics rate decreased." );
        break;

    case SDLK_KP_PLUS:
        log.put ( LOG_VERBOSE, "Processing a SDLK_KP_PLUS keypress..." );
        //if current desired fps is below 37, it's better to increase the fps (frames/sec.)...

        if ( systemData->physicsData.desiredStepsPerSecond < 37 )
        {
            systemData->physicsData.desiredStepsPerSecond++;
            systemData->physicsData.timeStep =
                1000 / systemData->physicsData.desiredStepsPerSecond;
        }
        else
        {                       //...otherwise, it's better to decrease the timestep (msecs.)
            //don't let the step time fall below 1 msec. (or don't allow more than 1000fps)
            if ( !--systemData->physicsData.timeStep )
            {
                systemData->physicsData.timeStep++;
            }
            systemData->physicsData.desiredStepsPerSecond =
                1000 / systemData->physicsData.timeStep;
        }
        log.append ( LOG_VERBOSE, "physics rate increased." );
        break;

    case 'q':
        log.put ( LOG_VERBOSE, "Processing a 'Q' keypress: exiting program" );
        systemData->disableGuiLoop (  );
        systemData->disableSimLoop (  );
        systemData->disableMainLoop (  );
        log.append ( LOG_VERBOSE, "Ok" );
        break;

        //this is left for non-assigned input events.
    case 'S':
        log.put ( LOG_VERBOSE, "Processing a 'S' keypress: doing nothing..." );
        // something
        log.append ( LOG_VERBOSE, "Ok" );
        break;

    default:
        log.put ( LOG_VERBOSE,
                  "Processing an unknown keypress: doing nothing..." );
        // something
        log.append ( LOG_VERBOSE, "Ok" );
        break;
    }
}
