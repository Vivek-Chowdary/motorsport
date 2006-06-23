/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "main.hpp"
#include "SDL/SDL.h"
#include "world.hpp"            // contains the IDF for the simulated/virtual world data
#include "system.hpp"           // contains the IDF for the system data
#include "logEngine.hpp"    // allows to easily log actions
#include "inputEngine.hpp"      // process the queue of input events
#include "graphicsEngine.hpp"   // displays the virtual and system data (sim+gui)
#include "physicsEngine.hpp"    // calculates the physics of the world data
#include "guiEngine.hpp"        // displays all the user interface on screen
#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#endif
#include <string>
#include "xmlTag.hpp"
#include <iostream>
#include "SDL/SDL_keysym.h"
#include "tools/paths.hpp"

int main (int argc, char **argv)
{
    // Display copyright note.
    std::cout << "===============================================================================" << std::endl;
    std::cout << "Motorsport, Copyright (C) 2003 - 2006 \"Motorsport\" developers (*)." << std::endl;
    std::cout << "Motorsport comes with ABSOLUTELY NO WARRANTY; for details, read (*)." << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it under certain conditions; ";
    std::cout << "read (*) for details." << std::endl << std::endl;
    std::cout << "(*)Complete information provided at doc/LICENSE" << std::endl;
    std::cout << "===============================================================================" << std::endl;

    // We start the main log engine.
#ifdef MACOSX
    XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/mainConfig.xml");
#else
    XmlTag * tag = new XmlTag ("../cfg/mainConfig.xml");
#endif
    pLogEngine log = LogEngine::create (LOG_DEVELOPER, "MainProgram");
    log->__format (LOG_ENDUSER, "( 1 ): Loading engines and libraries...");
    log->__format (LOG_DEVELOPER, "Creating system data");
    System::get();

    if (tag->getName() == "mainConfig")
    {
        System::get()->videoRecordTimestep = stoi (tag->getAttribute("videoRecordTimestep"));
        Paths::setCustomDataDir(tag->getAttribute("dataDir"));
    }
    delete tag;

    // We declare the 'global' data and engines.
    log->__format (LOG_DEVELOPER, "Creating graphics engine");
    GraphicsEngine *graphicsEngine = new GraphicsEngine ();
    log->__format (LOG_DEVELOPER, "Creating gui engine");
    GuiEngine *guiEngine = new GuiEngine ();
    guiEngine->showLoadscreen ();
    log->loadscreen (LOG_DEVELOPER, "Creating physics engine...");
    PhysicsEngine *physicsEngine = new PhysicsEngine ();

    log->loadscreen (LOG_DEVELOPER, "Starting SDL subsystems...");
    startSdl (log);
    log->loadscreen (LOG_DEVELOPER, "Creating input engine...");
    InputEngine *inputEngine = new InputEngine ();

    log->loadscreen (LOG_DEVELOPER, "Getting system data pointer...");
    pSystem system = System::get();

    // We load the world data from hdd into memory.
    log->__format (LOG_ENDUSER, "( 2 ): Loading world data...");
    log->loadscreen (LOG_CCREATOR, "Loading initial world data...");
    World::setNewWorld("default");

    // We start the main loop.
    log->__format (LOG_ENDUSER, "( 3 ): Starting simulation...");
    log->loadscreen (LOG_DEVELOPER, "Initializating main loop");
    system->statisticsTime = system->simulationTime = SDL_GetTicks() / 1000.0;
    log->loadscreen (LOG_DEVELOPER, "Enabling main loop");
    system->enableMainLoop ();
    log->loadscreen (LOG_ENDUSER, "Hiding load screen");
    guiEngine->hideLoadscreen();
    log->__format (LOG_DEVELOPER, "Starting main loop");
    while (system->isMainLoopEnabled ())
    {
        // Update current real loop time.
        system->realTime = SDL_GetTicks () / 1000.0;
        // Update statistics data every second.
        if (system->simulationTime - system->statisticsTime >= 1)
        {
            system->graphicsFrequency = system->graphicsSteps;
            system->setCurrentPhysicsFrequency (system->physicsSteps);
            system->graphicsSteps = system->physicsSteps = 0;
            system->statisticsTime += 1;
            log->__format (LOG_DEVELOPER, "Main Loop Stats: graphicsFps=%i - physicsFps=%i", system->graphicsFrequency, system->getCurrentPhysicsFrequency());
        }
        inputEngine->computeStep ();
        // Run the gui engine.
        guiEngine->computeStep ();
        // Run the graphics engine.
        graphicsEngine->computeStep ();
        system->graphicsSteps++;
        // Clear all event-like behaving axis. This must be moved to the input engine as axis filters asap. TODO
        inputEngine->clearGraphicsEventAxis ();
        // Run the physics engine until the game time is in sync with the real loop time.
        while (((system->realTime - system->simulationTime) >= system->getDesiredPhysicsTimestep()) && (system->isMainLoopEnabled ()))
        {
            system->simulationTime += system->getDesiredPhysicsTimestep();
            static int step = system->timeScale;
            step--;
            if (!step)
            {
                system->physicsSteps++;
                static int steps = 1;
                if ((system->pauseStep != 0 && steps < system->pauseStep) || (system->pauseStep == 0))
                {
                    computeLogic (log);
                    inputEngine->clearLogicEventAxis ();
                    inputEngine->computeStep ();
                    physicsEngine->computeStep ();
                    steps++;
                    if (System::get()->videoRecordTimestep > 0)
                    {
                        recordVideoFrames ();
                    }
                }
                step = system->timeScale;
            }
        }
    }
    log->__format (LOG_DEVELOPER, "Main loop finished");

    // We unload the world data from memory.
    log->__format (LOG_ENDUSER, "( 4 ): Unloading world data...");
    log->__format (LOG_CCREATOR, "Unloading world data");
    World::destroy();

    // We delete the 'global' data and engines.
    log->__format (LOG_ENDUSER, "( 5 ): Unloading engines and libraries...");
    log->__format (LOG_DEVELOPER, "Deleting graphics engine");
    delete graphicsEngine;
    log->__format (LOG_DEVELOPER, "Deleting physics engine");
    delete physicsEngine;
    log->__format (LOG_DEVELOPER, "Deleting input engine");
    delete inputEngine;
    log->__format (LOG_DEVELOPER, "Deleting gui engine");
    delete guiEngine;
    system.reset();
    System::destroy();

    stopSdl ();

    //FIXME: uncomment later on
    //WorldObject::logAll();
    LogEngine::logAll();
    //LogEngine::deleteAll();
    
    log->__format (LOG_DEVELOPER, "About to exit motorsport");
    // We go back to the OS.
    return (0);
}

void startSdl (pLogEngine log)
{
    if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
    {
        log->__format (LOG_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError ());
    }
}

void stopSdl (void)
{
    SDL_Quit ();
}

void computeLogic (pLogEngine log)
{
    pSystem system = System::get();
    if (System::get()->axisMap[getIDKeyboardKey (SDLK_ESCAPE)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_ESCAPE keypress: User wants to exit. Notifying to stop mainLoop...");
        system->disableMainLoop ();
    }
    if (system->axisMap[getIDKeyboardKey (SDLK_HOME)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_HOME keypress...");
        system->setDesiredPhysicsFrequency(150.0);
    }
    if (system->axisMap[getIDKeyboardKey (SDLK_END)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_END keypress...");
        system->setDesiredPhysicsFrequency(300.0);
    }
    if (system->axisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_KP_MINUS keypress...");
        system->decreaseDesiredPhysicsRate();
    }
    if (system->axisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_KP_PLUS keypress...");
        system->increaseDesiredPhysicsRate();
    }
    if (system->axisMap[getIDKeyboardKey(SDLK_b)]->getValue() == 1)
    {
        system->cameraDirector = !(system->cameraDirector);
        system->axisMap[getIDKeyboardKey(SDLK_b)]->setNewRawValue(0);
    }
}

void recordVideoFrames ()
{
    static double time = 0;
    int takeShot = 0;
    if ((int) time >= System::get()->videoRecordTimestep)
    {
        takeShot++;
        time -= System::get()->getDesiredPhysicsTimestep();
    }
    if ((takeShot > 0) && (System::get()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->getValue () == 0))
    {
        System::get()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->setNewRawValue (1);
        takeShot--;
    }
    time += System::get()->getDesiredPhysicsTimestep();
}
