
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

int main ( int argc, char **argv )
{
    MainData * mainData = new MainData;
    processXmlFile ( "mainConfig.xml", &processMainConfigFile, (void*)mainData );
    
    LogEngine *log = new LogEngine ( mainData->localLogLevel, mainData->localLogName );
    log->put ( LOG_INFO, "Temporary parsing data already loaded into memory..." );
    log->put ( LOG_INFO, "Unloading temporary parsing data from memory..." );
    delete [](mainData->localLogName);
    delete mainData;

    //we declare the 'global' data and engines
    log->put ( LOG_INFO, "( 1 ): Loading engines and libraries..." );
    log->put ( LOG_INFO, "Starting SDL subsystems" );
    sdl_start ( log );
    log->put ( LOG_INFO, "Creating world data" );
    new WorldData (  );

    log->put ( LOG_INFO, "Creating system data" );
    new SystemData (  );

    log->put ( LOG_INFO, "Creating data engine" );
    DataEngine *data = new DataEngine (  );

    log->put ( LOG_INFO, "Creating input engine" );
    InputEngine *input = new InputEngine (  );

    log->put ( LOG_INFO, "Creating graphics engine" );
    GraphicsEngine *graphics = new GraphicsEngine (  );

    log->put ( LOG_INFO, "Creating physics engine" );
    PhysicsEngine *physics = new PhysicsEngine (  );

    log->put ( LOG_INFO, "Creating gui engine" );
    GuiEngine *gui = new GuiEngine (  );

    log->put ( LOG_INFO, "Getting system data pointer" );
    SystemData *systemData = SystemData::getSystemDataPointer (  );

    log->put ( LOG_INFO, "( 2 ): Loading world data..." );
    log->put ( LOG_INFO, "Loading initial world data" );
    data->loadWorldData (  );

    log->put ( LOG_INFO, "( 3 ): Starting simulation..." );
    log->put ( LOG_INFO, "Initializating main loop" );
    systemData->lastStatTime = systemData->currentPhysicsTime =
        SDL_GetTicks (  );
    log->put ( LOG_INFO, "Enabling main loop" );
    systemData->enableMainLoop (  );
    log->put ( LOG_INFO, "Starting main loop" );
    while ( systemData->canMainLoopRun (  ) )
    {
        systemData->currentMainLoopTime = SDL_GetTicks (  );
        if ( systemData->currentMainLoopTime - systemData->lastStatTime >=
             1000 )
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
                systemData->physicsTimeStep )
        {
            systemData->currentPhysicsTime += systemData->physicsTimeStep;
            systemData->physicsSteps++;
            //now run the physics engine
            physics->step (  );

            static int inputSteps = 5;

            if ( !--inputSteps )
            {
                inputSteps = 5;
                input->step (  );
            }
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
    log->put ( LOG_INFO, "Main loop finished" );

    log->put ( LOG_INFO, "( 4 ): Unloading world data..." );
    log->put ( LOG_INFO, "Unloading world data" );
    data->unloadWorldData (  );

    log->put ( LOG_INFO, "( 5 ): Unloading engines and libraries..." );
    log->put ( LOG_INFO, "Deleting graphics engine" );
    delete graphics;

    log->put ( LOG_INFO, "Deleting physics engine" );
    delete physics;

    log->put ( LOG_INFO, "Deleting input engine" );
    delete input;

    log->put ( LOG_INFO, "Deleting gui engine" );
    delete gui;

    log->put ( LOG_INFO, "Deleting data engine" );
    delete data;

    log->put ( LOG_INFO, "Deleting log engine" );
    delete log;

    sdl_stop (  );

    //and finally back to the OS
    return ( 0 );
}

void sdl_start ( LogEngine * log )
{                               //initialization functions for SDL
    //returns -2 on warning, -1 on error, 0 on success
    if ( SDL_Init ( SDL_INIT_TIMER /* | SDL_INIT_JOYSTICK */  ) < 0 )
    {
        log->format ( LOG_ERROR, "Couldn't initialize SDL: %s\n",
                      SDL_GetError (  ) );
    }
}

void sdl_stop ( void )
{                               //exit functions for SDL
    SDL_Quit (  );
}

int processMainConfigFile ( DOMNode * n, void *data )
{
    if ( n )
    {
        if ( n->getNodeType (  ) == DOMNode::ELEMENT_NODE )
        {
            char *name = XMLString::transcode ( n->getNodeName (  ) );
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER
                endl;

            if ( !strncmp ( name, "mainConfig", 11 ) )
            {
                XERCES_STD_QUALIFIER cout <<
                    "Found the main config element." <<
                    XERCES_STD_QUALIFIER endl;
                if ( n->hasAttributes (  ) )
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes (  );
                    int nSize = pAttributes->getLength (  );

                    for ( int i = 0; i < nSize; ++i )
                    {
                        DOMAttr *pAttributeNode =
                            ( DOMAttr * ) pAttributes->item ( i );
                        char *name =
                            XMLString::transcode ( pAttributeNode->
                                                   getName (  ) );
                        if ( !strncmp ( name, "localLogLevel", 14 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the local log level:";
                            name =
                                XMLString::transcode ( pAttributeNode->
                                                       getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name <<
                                XERCES_STD_QUALIFIER endl;
                            if ( !strncmp ( name, "LOG_ERROR", 10 ) )
                                ( *( MainData * ) data ).localLogLevel =
                                    LOG_ERROR;
                            if ( !strncmp ( name, "LOG_WARNING", 13 ) )
                                ( *( MainData * ) data ).localLogLevel =
                                    LOG_WARNING;
                            if ( !strncmp ( name, "LOG_INFO", 9 ) )
                                ( *( MainData * ) data ).localLogLevel =
                                    LOG_INFO;
                            if ( !strncmp ( name, "LOG_VERBOSE", 12 ) )
                                ( *( MainData * ) data ).localLogLevel =
                                    LOG_VERBOSE;
                            if ( !strncmp ( name, "LOG_TRACE", 9 ) )
                                ( *( MainData * ) data ).localLogLevel =
                                    LOG_TRACE;
                        }
                        if ( !strncmp ( name, "localLogName", 13 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the log name:";
                            name =
                                XMLString::transcode ( pAttributeNode->
                                                       getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name <<
                                XERCES_STD_QUALIFIER endl;

                            ( *( MainData * ) data ).localLogName =
                                new char[strlen ( name ) + 1];
                            strncpy ( ( *( MainData * ) data ).localLogName,
                                      name, strlen ( name ) + 1 );
                        }
                        XMLString::release ( &name );
                    }
                }
            }
        }
    }
    return 1;
}
