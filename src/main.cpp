/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "main.hpp"

#include "SDL.h"
#include "world.hpp"            // contains the IDF for the simulated/virtual world data
#include "system.hpp"           // contains the IDF for the system data
#include "log/logEngine.hpp"    // allows to easily log actions
#include "inputEngine.hpp"      // process the queue of input events
#include "graphicsEngine.hpp"   // displays the virtual and system data (sim+gui)
#include "physicsEngine.hpp"    // calculates the physics of the world data
#include "guiEngine.hpp"        // displays all the user interface on screen
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#endif
#include <string>
#include "data/xmlParser.hpp"
#include <iostream>
#include "SDL/SDL_keysym.h"

int main (int argc, char **argv)
{
    // Display copyright note.
    std::cout << "===============================================================================" << std::endl;
    std::cout << "Motorsport, Copyright (C) 2003, 2004 \"Motorsport\" developers (*)." << std::endl;
    std::cout << "Motorsport comes with ABSOLUTELY NO WARRANTY; for details, read (*)." << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it under certain conditions; ";
    std::cout << "read (*) for details." << std::endl << std::endl;
    std::cout << "(*)Complete information provided at http://motorsport-sim.org/svn/trunk/doc/LICENSE" << std::endl;
    std::cout << "===============================================================================" << std::endl;

    // We start the main log engine.
    XmlFile *xmlFile = new XmlFile ("mainConfig.xml");
    LogEngine *log = processXmlRootNode (xmlFile->getRootNode ());
    delete xmlFile;

    // We declare the 'global' data and engines.
    log->put (LOG_ENDUSER, "( 1 ): Loading engines and libraries...");
    log->put (LOG_DEVELOPER, "Creating system data");
    new SystemData ();
    log->put (LOG_DEVELOPER, "Creating graphics engine");
    GraphicsEngine *graphicsEngine = new GraphicsEngine ();
    log->put (LOG_DEVELOPER, "Creating gui engine");
    GuiEngine *guiEngine = new GuiEngine ();
    guiEngine->showLoadscreen ();
    log->loadscreen (LOG_DEVELOPER, "Creating physics engine...");
    PhysicsEngine *physicsEngine = new PhysicsEngine ();

    log->loadscreen (LOG_DEVELOPER, "Starting SDL subsystems...");
    startSdl (log);
    log->loadscreen (LOG_DEVELOPER, "Creating input engine...");
    InputEngine *inputEngine = new InputEngine ();

    log->loadscreen (LOG_DEVELOPER, "Getting system data pointer...");
    SystemData *systemData = SystemData::getSystemDataPointer ();

    // We load the world data from hdd into memory.
    log->put (LOG_ENDUSER, "( 2 ): Loading world data...");
    log->loadscreen (LOG_CCREATOR, "Loading initial world data...");
    new World ("default.xml");

    // We start the main loop.
    log->put (LOG_ENDUSER, "( 3 ): Starting simulation...");
    log->loadscreen (LOG_DEVELOPER, "Initializating main loop");
    systemData->statisticsTime = systemData->simulationTime = SDL_GetTicks ();
    log->loadscreen (LOG_DEVELOPER, "Enabling main loop");
    systemData->enableMainLoop ();
    log->loadscreen (LOG_ENDUSER, "Hiding load screen");
    guiEngine->hideLoadscreen();
    log->put (LOG_DEVELOPER, "Starting main loop");
    while (systemData->isMainLoopEnabled ())
    {
        // Update current real loop time.
        systemData->realTime = SDL_GetTicks ();
        // Update statistics data every second.
        if (systemData->simulationTime - systemData->statisticsTime >= 1000)
        {
            systemData->graphicsFrequency = systemData->graphicsSteps;
            systemData->physicsFrequency = systemData->physicsSteps;
            systemData->graphicsSteps = systemData->physicsSteps = 0;
            systemData->statisticsTime += 1000;
            log->format (LOG_DEVELOPER, "Main Loop Stats: graphicsFps=%i - physicsFps=%i", systemData->graphicsFrequency, systemData->physicsFrequency);
        }
        inputEngine->computeStep ();
        // Run the gui engine.
        guiEngine->computeStep ();
        // Run the graphics engine.
        graphicsEngine->computeStep ();
        systemData->graphicsSteps++;
        // Clear all event-like behaving axis. This must be moved to the input engine as axis filters asap. TODO
        inputEngine->clearGraphicsEventAxis ();
        // Run the physics engine until the game time is in sync with the real loop time.
        while (((systemData->realTime - systemData->simulationTime) >= systemData->physicsTimeStep) && (systemData->isMainLoopEnabled ()))
        {
            systemData->simulationTime += systemData->physicsTimeStep;
            static int step = systemData->timeScale;
            step--;
            if (!step)
            {
                systemData->physicsSteps++;
                static int steps = 1;
                if ((systemData->pauseStep != 0 && steps < systemData->pauseStep) || (systemData->pauseStep == 0))
                {
                    computeLogic (log);
                    inputEngine->clearLogicEventAxis ();
                    inputEngine->computeStep ();
                    physicsEngine->computeStep ();
                    steps++;
                    if (SystemData::getSystemDataPointer ()->videoRecordTimestep > 0)
                    {
                        recordVideoFrames ();
                    }
                }
                step = systemData->timeScale;
            }
        }
    }
    log->put (LOG_DEVELOPER, "Main loop finished");

    // We unload the world data from memory.
    log->put (LOG_ENDUSER, "( 4 ): Unloading world data...");
    log->put (LOG_CCREATOR, "Unloading world data");
    delete World::getWorldPointer ();

    // We delete the 'global' data and engines.
    log->put (LOG_ENDUSER, "( 5 ): Unloading engines and libraries...");
    log->put (LOG_DEVELOPER, "Deleting graphics engine");
    delete graphicsEngine;
    log->put (LOG_DEVELOPER, "Deleting physics engine");
    delete physicsEngine;
    log->put (LOG_DEVELOPER, "Deleting input engine");
    delete inputEngine;
    log->put (LOG_DEVELOPER, "Deleting gui engine");
    delete guiEngine;
    log->put (LOG_DEVELOPER, "Deleting log engine");
    delete log;
    stopSdl ();

    // We go back to the OS.
    return (0);
}

void startSdl (LogEngine * log)
{
    if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
    {
        log->format (LOG_ERROR, "Couldn't initialize SDL: %s\n", SDL_GetError ());
    }
}

void stopSdl (void)
{
    SDL_Quit ();
}

void computeLogic (LogEngine * log)
{
    SystemData *systemData = SystemData::getSystemDataPointer ();
    if (SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_ESCAPE)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop...");
        systemData->disableMainLoop ();
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_HOME)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_HOME keypress...");
        systemData->physicsDesiredFrequency = 30;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_END)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_END keypress...");
        systemData->physicsDesiredFrequency = 250;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_q)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_q keypress: User wants to exit. Notifying to stop mainLoop...");
        systemData->disableMainLoop ();
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_KP_MINUS keypress...");
        if (systemData->physicsDesiredFrequency < 37)
        {
            if (!--systemData->physicsDesiredFrequency)
            {
                systemData->physicsDesiredFrequency++;
            }
            systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
        } else {
            systemData->physicsTimeStep++;
            systemData->physicsDesiredFrequency = 1000 / systemData->physicsTimeStep;
        }
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->getValue () == 1)
    {
        log->put (LOG_DEVELOPER, "Processing a SDLK_KP_PLUS keypress...");
        if (systemData->physicsDesiredFrequency < 37)
        {
            systemData->physicsDesiredFrequency++;
            systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
        } else {
            if (!--systemData->physicsTimeStep)
            {
                systemData->physicsTimeStep++;
            }
            systemData->physicsDesiredFrequency = 1000 / systemData->physicsTimeStep;
        }
    }
}

void recordVideoFrames ()
{
    static unsigned int time = 0;
    int takeShot = 0;
    if ((int) time >= SystemData::getSystemDataPointer ()->videoRecordTimestep)
    {
        takeShot++;
        time -= SystemData::getSystemDataPointer ()->physicsTimeStep;
    }
    if ((takeShot > 0) && (SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->getValue () == 0))
    {
        SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->setNewRawValue (1);
        takeShot--;
    }
    time += SystemData::getSystemDataPointer ()->physicsTimeStep;
}

LogEngine *processXmlRootNode (DOMNode * n)
{
    LogEngine *tmpLog = new LogEngine (LOG_DEVELOPER, "XML");
    tmpLog->put (LOG_DEVELOPER, "Assigning default values");
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "MAI";
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_DEVELOPER, "Name: %s", name.c_str());
            if (name == "mainConfig")
            {
                tmpLog->put (LOG_DEVELOPER, "Found the main config element.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                    int nSize = pAttributes->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) pAttributes->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "localLogLevel")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_ENDUSER, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the log name: %s", localLogName.c_str());
                        }
                        if (attribute == "videoRecordTimestep")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found video recording timestep value: %s", attribute.c_str ());
                            SystemData::getSystemDataPointer ()->videoRecordTimestep = stoi (attribute);
                        }
                        if (attribute == "dataDir")
                        {
                            assignXmlString (SystemData::getSystemDataPointer ()->dataDir, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the data directory: %s", SystemData::getSystemDataPointer ()->dataDir.c_str());
                        }
                    }
                }
            }
        }
    }
    LogEngine *returnLog = new LogEngine (localLogLevel, localLogName.c_str ());
    delete tmpLog;
    returnLog->put (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");
    returnLog->put (LOG_DEVELOPER, "Unloading temporary parsing data from memory...");
    return returnLog;
}
