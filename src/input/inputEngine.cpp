
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

InputEngine::InputEngine ( )
{
    //first of all start the logger (automatically logs the start of itself)
    log = new LogEngine ( LOG_INFO, "INP" );

    //we store the pointers to the 'global' data, so that we access it easily.
    //we might have to change this and only allow changing data through
    // some dataEngine functions, which will act as an interface to the data
    // stored in memory with the IDF.
    log->put ( LOG_INFO, "Setting up data pointers..." );
    systemData = SystemData::getSystemDataPointer ();
    worldData = WorldData::getWorldDataPointer ();
 
    log->put ( LOG_INFO, "Initializing keyboard data array" );
    keyState = SDL_GetKeyState ( NULL );  
    log->put ( LOG_INFO, "Initializing mouse data variables");
    mouseMovementX = mouseMovementY = 0;
}

int InputEngine::step ( void )
{
    SDL_Event event;
    while ( SDL_PollEvent ( &event ) )
    {
        switch ( event.type )
        {
        case SDL_QUIT:
            //this can be the user cliking to close the window
            log->put ( LOG_VERBOSE, "New SDL_QUIT event: notifying to stop mainLoop..." );
            systemData->disableMainLoop (  );
            break;
        default:
            break;
        }
    }
    int x = 0, y = 0;
    SDL_GetRelativeMouseState( &x, &y);
    mouseMovementX = x;
    mouseMovementY = - y;
    
    processKeyboard ();
    //processMouseButtons ();
    processMouseMovement ( );
    
    return ( 0 );
}

void InputEngine::processMouseMovement ( )
{
    if (mouseMovementX)
    {
        worldData->camera->setRotateRight (mouseMovementX);
    }
    if (mouseMovementY)
    {
        worldData->camera->setRotateUp (mouseMovementY);
    }
}

void InputEngine::processKeyboard ( )
{
    //Processing key presses...
    if ( keyState[SDLK_RETURN] || keyState[SDLK_ESCAPE] )
    {
        log->put ( LOG_VERBOSE,
                  "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop..." );
        systemData->disableMainLoop (  );
    }

    if ( keyState[SDLK_RIGHT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_RIGHT keypress..." );
        worldData->camera->setRotateRight (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the right." );
    }

    if ( keyState[SDLK_LEFT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_LEFT keypress..." );
        worldData->camera->setRotateLeft (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the left." );
    }

    if ( keyState[SDLK_UP] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_UP keypress..." );
        worldData->camera->setRotateUp (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the top." );
    }

    if ( keyState[SDLK_DOWN] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_DOWN keypress..." );
        worldData->camera->setRotateDown (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the bottom." );
    }

    if ( keyState[SDLK_w] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        worldData->camera->goForward = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_s] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        worldData->camera->goBack = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_a] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        worldData->camera->goLeft = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_d] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        worldData->camera->goRight = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_o] )
    {
        worldData->cubeList[0].setMoveToXNegative(1);
    }
        
    if ( keyState[SDLK_p] )
    {
        worldData->cubeList[0].setMoveToXPositive(1);
    }
        
    if ( keyState[SDLK_i] )
    {
        worldData->cubeList[0].setMoveToYPositive(1);
    }
        
    if ( keyState[SDLK_k] )
    {
        worldData->cubeList[0].setMoveToYNegative(1);
    }
        
    if ( keyState[SDLK_KP_MINUS] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_KP_MINUS keypress..." );
        //modify the physics engine rate
        //if current desired fps is below 37, it's better to decrease the fps (frames/sec.)...
        if ( systemData->physicsDesiredStepsPerSecond < 37 )
        {
            //don't let the rate fall below 1 frame per second (or don't allow more than 1000msecs. step)
            if ( !--systemData->physicsDesiredStepsPerSecond )
            {
                systemData->physicsDesiredStepsPerSecond++;
            }
            systemData->physicsTimeStep =
                1000 / systemData->physicsDesiredStepsPerSecond;
        }
        else
        {                       //...otherwise, it's better to increase the timestep (msecs.)
            systemData->physicsTimeStep++;
            systemData->physicsDesiredStepsPerSecond =
                1000 / systemData->physicsTimeStep;
        }
    }

    if ( keyState[SDLK_KP_PLUS] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_KP_PLUS keypress..." );
        //if current desired fps is below 37, it's better to increase the fps (frames/sec.)...
        if ( systemData->physicsDesiredStepsPerSecond < 37 )
        {
            systemData->physicsDesiredStepsPerSecond++;
            systemData->physicsTimeStep =
                1000 / systemData->physicsDesiredStepsPerSecond;
        }
        else
        {                       //...otherwise, it's better to decrease the timestep (msecs.)
            //don't let the step time fall below 1 msec. (or don't allow more than 1000fps)
            if ( !--systemData->physicsTimeStep )
            {
                systemData->physicsTimeStep++;
            }
            systemData->physicsDesiredStepsPerSecond =
                1000 / systemData->physicsTimeStep;
        }
    }

    if ( keyState[SDLK_q] )
    {
        log->put ( LOG_VERBOSE, "Processing a 'Q' keypress: exiting program" );
        systemData->disableMainLoop (  );
    }
        
    if ( keyState[SDLK_PRINT] )
    {
        log->put ( LOG_INFO, "Taking a screenshot to sshot.png" );
        systemData->ogreWindow->writeContentsToFile ( "sshot.png" );
    }

    if ( keyState[SDLK_f] )
    {
        log->put ( LOG_VERBOSE, "Processing a 'F' keypress: turning on/off framerates display..." );
        systemData->invertStatisticsEnabled (  );
    }
    
    //Processing key releases...
    if ( !keyState[SDLK_RIGHT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_RIGHT keyrelease..." );
        worldData->camera->setRotateRight (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_LEFT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_LEFT keyrelease..." );
        worldData->camera->setRotateLeft (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_UP] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_UP keyrelease..." );
        worldData->camera->setRotateUp (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_DOWN] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_DOWN keyprelease..." );
        worldData->camera->setRotateDown (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_w] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        worldData->camera->goForward = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_s] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        worldData->camera->goBack = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_a] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        worldData->camera->goLeft = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_d] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyprelease..." );
        worldData->camera->goRight = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_o] )
    {
        worldData->cubeList[0].setMoveToXNegative(0);
    }
        
    if ( !keyState[SDLK_p] )
    {
        worldData->cubeList[0].setMoveToXPositive(0);
    }
        
    if ( !keyState[SDLK_i] )
    {
        worldData->cubeList[0].setMoveToYPositive(0);
    }
        
    if ( !keyState[SDLK_k] )
    {
        worldData->cubeList[0].setMoveToYNegative(0);
    }
}

InputEngine::~InputEngine ( void )
{
    //finally stop the log engine
    delete log;
}

