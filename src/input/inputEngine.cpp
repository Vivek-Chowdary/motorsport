
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
    InputData * data = new InputData;
    data->input = this;
    processXmlFile ( "inputConfig.xml", &InputEngine::processInputConfigFile, (void*)data);
    
    log = new LogEngine ( data->localLogLevel, data->localLogName );
    log->put ( LOG_INFO, "Temporary parsing data already loaded into memory..." );

    log->put ( LOG_INFO, "Setting up data pointers..." );
    systemData = SystemData::getSystemDataPointer ();
    worldData = WorldData::getWorldDataPointer ();
 
    log->put ( LOG_INFO, "Initializing keyboard data array" );
    keyState = SDL_GetKeyState ( NULL );  
    log->put ( LOG_INFO, "Initializing mouse data variables");
    mouseMovementX = mouseMovementY = 0;

    //Disable cursor on screen
//    SDL_ShowCursor(SDL_DISABLE);
//    SDL_WM_GrabInput(SDL_GRAB_ON);
    
    log->put ( LOG_INFO, "Unloading temporary parsing data from memory..." );
    delete [](data->localLogName);
    delete data;
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
            log->put ( LOG_INFO, "New SDL_QUIT event: User wants to exit. Notifying to stop mainLoop..." );
            systemData->disableMainLoop (  );
            break;
        case SDL_KEYDOWN:
            log->put ( LOG_VERBOSE, "New SDL_KEYDOWN event: finding out what key's been pressed." );
            switch ( event.key.keysym.sym )
            {
            case SDLK_PRINT:
                log->put ( LOG_INFO, "User wants to take a screenshot." );
                systemData->setTakeScreenshot ();
                break;
            case SDLK_f:
                log->put ( LOG_INFO, "User wants to show/hide statistics on screen." );
                systemData->setInvertShowStatistics (  );
            default:
                break;
            }
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
        Camera::cameraList[0]->setRotateRight (mouseMovementX);
    }
    if (mouseMovementY)
    {
        Camera::cameraList[0]->setRotateUp (mouseMovementY);
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
        Camera::cameraList[0]->setRotateRight (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the right." );
    }

    if ( keyState[SDLK_LEFT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_LEFT keypress..." );
        Camera::cameraList[0]->setRotateLeft (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the left." );
    }

    if ( keyState[SDLK_UP] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_UP keypress..." );
        Camera::cameraList[0]->setRotateUp (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the top." );
    }

    if ( keyState[SDLK_DOWN] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_DOWN keypress..." );
        Camera::cameraList[0]->setRotateDown (20);
        log->put ( LOG_VERBOSE, "Camera1 rotated to the bottom." );
    }

    if ( keyState[SDLK_w] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        Camera::cameraList[0]->goForward = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_s] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        Camera::cameraList[0]->goBack = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_a] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        Camera::cameraList[0]->goLeft = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_d] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keypress..." );
        Camera::cameraList[0]->goRight = true;
        log->put ( LOG_VERBOSE, "Camera moved." );
    }

    if ( keyState[SDLK_o] )
    {
        Cube::cubeList[0]->setMoveToXNegative(1);
    }
        
    if ( keyState[SDLK_p] )
    {
        Cube::cubeList[0]->setMoveToXPositive(1);
    }
        
    if ( keyState[SDLK_i] )
    {
        Cube::cubeList[0]->setMoveToYPositive(1);
    }
        
    if ( keyState[SDLK_k] )
    {
        Cube::cubeList[0]->setMoveToYNegative(1);
    }
    
    if ( keyState[SDLK_HOME] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_HOME keypress..." );
        systemData->physicsDesiredStepsPerSecond = 30;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredStepsPerSecond;
    }
        
    if ( keyState[SDLK_END] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_END keypress..." );
        systemData->physicsDesiredStepsPerSecond = 250;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredStepsPerSecond;
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
        log->put ( LOG_INFO, "Processing a 'Q' keypress: User wants to exit. Notifying to stop mainLoop..." );
        systemData->disableMainLoop (  );
    }
        
    //Processing key releases...
    if ( !keyState[SDLK_RIGHT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_RIGHT keyrelease..." );
        Camera::cameraList[0]->setRotateRight (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_LEFT] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_LEFT keyrelease..." );
        Camera::cameraList[0]->setRotateLeft (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_UP] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_UP keyrelease..." );
        Camera::cameraList[0]->setRotateUp (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_DOWN] )
    {
        log->put ( LOG_VERBOSE, "Processing a SDLK_DOWN keyprelease..." );
        Camera::cameraList[0]->setRotateDown (0);
        log->put ( LOG_VERBOSE, "Camera1 stopped rotating." );
    }

    if ( !keyState[SDLK_w] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        Camera::cameraList[0]->goForward = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_s] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        Camera::cameraList[0]->goBack = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_a] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyrelease..." );
        Camera::cameraList[0]->goLeft = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_d] )
    {
        log->put ( LOG_VERBOSE, "Processing a w keyprelease..." );
        Camera::cameraList[0]->goRight = false;
        log->put ( LOG_VERBOSE, "Camera stopped moving." );
    }

    if ( !keyState[SDLK_o] )
    {
        Cube::cubeList[0]->setMoveToXNegative(0);
    }
        
    if ( !keyState[SDLK_p] )
    {
        Cube::cubeList[0]->setMoveToXPositive(0);
    }
        
    if ( !keyState[SDLK_i] )
    {
        Cube::cubeList[0]->setMoveToYPositive(0);
    }
        
    if ( !keyState[SDLK_k] )
    {
        Cube::cubeList[0]->setMoveToYNegative(0);
    }
}

InputEngine::~InputEngine ( void )
{
    //finally stop the log engine
    delete log;
}

