/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "log/logEngine.hpp"    // allows to easily log actions
#include "inputEngine.hpp"      // process the queue of input events
#include "graphicsEngine.hpp"   // displays the virtual and system data (sim+gui)
#include "physicsEngine.hpp"    // calculates the physics of the world data
#include "guiEngine.hpp"        // displays all the user interface on screen
#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#endif
#include <string>
#include "tools/xmlParser.hpp"
#include <iostream>
#include "SDL/SDL_keysym.h"

int main (int argc, char **argv)
{
    // Display copyright note.
    std::cout << "===============================================================================" << std::endl;
    std::cout << "Motorsport, Copyright (C) 2003-2005 \"Motorsport\" developers (*)." << std::endl;
    std::cout << "Motorsport comes with ABSOLUTELY NO WARRANTY; for details, read (*)." << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it under certain conditions; ";
    std::cout << "read (*) for details." << std::endl << std::endl;
    std::cout << "(*)Complete information provided at doc/LICENSE" << std::endl;
    std::cout << "===============================================================================" << std::endl;

    // We start the main log engine.
#ifdef MACOSX
    XmlFile *xmlFile = new XmlFile ("motorsport.app/Contents/Resources/mainConfig.xml");
#else
    XmlFile *xmlFile = new XmlFile ("../cfg/mainConfig.xml");
#endif
    LogEngine *log = processXmlRootNode (xmlFile->getRootNode ());
    delete xmlFile;

    // We declare the 'global' data and engines.
    log->__format (LOG_ENDUSER, "( 1 ): Loading engines and libraries...");
    log->__format (LOG_DEVELOPER, "Creating system data");
    new SystemData ();
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
    SystemData *systemData = SystemData::getSystemDataPointer ();

    // We load the world data from hdd into memory.
    log->__format (LOG_ENDUSER, "( 2 ): Loading world data...");
    log->loadscreen (LOG_CCREATOR, "Loading initial world data...");
    new World ("default");

    // We start the main loop.
    log->__format (LOG_ENDUSER, "( 3 ): Starting simulation...");
    log->loadscreen (LOG_DEVELOPER, "Initializating main loop");
    systemData->statisticsTime = systemData->simulationTime = SDL_GetTicks() / 1000.0;
    log->loadscreen (LOG_DEVELOPER, "Enabling main loop");
    systemData->enableMainLoop ();
    log->loadscreen (LOG_ENDUSER, "Hiding load screen");
    guiEngine->hideLoadscreen();
    log->__format (LOG_DEVELOPER, "Starting main loop");
    while (systemData->isMainLoopEnabled ())
    {
        // Update current real loop time.
        systemData->realTime = SDL_GetTicks () / 1000.0;
        // Update statistics data every second.
        if (systemData->simulationTime - systemData->statisticsTime >= 1)
        {
            systemData->graphicsFrequency = systemData->graphicsSteps;
            systemData->setCurrentPhysicsFrequency (systemData->physicsSteps);
            systemData->graphicsSteps = systemData->physicsSteps = 0;
            systemData->statisticsTime += 1;
            log->__format (LOG_DEVELOPER, "Main Loop Stats: graphicsFps=%i - physicsFps=%i", systemData->graphicsFrequency, systemData->getCurrentPhysicsFrequency());
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
        while (((systemData->realTime - systemData->simulationTime) >= systemData->getDesiredPhysicsTimestep()) && (systemData->isMainLoopEnabled ()))
        {
            systemData->simulationTime += systemData->getDesiredPhysicsTimestep();
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
    log->__format (LOG_DEVELOPER, "Main loop finished");

    // We unload the world data from memory.
    log->__format (LOG_ENDUSER, "( 4 ): Unloading world data...");
    log->__format (LOG_CCREATOR, "Unloading world data");
    delete World::getWorldPointer ();

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
    log->__format (LOG_DEVELOPER, "Deleting log engine");
    delete log;
    stopSdl ();

    // We go back to the OS.
    return (0);
}

void startSdl (LogEngine * log)
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

void computeLogic (LogEngine * log)
{
    SystemData *systemData = SystemData::getSystemDataPointer ();
    if (SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_ESCAPE)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_ESCAPE keypress: User wants to exit. Notifying to stop mainLoop...");
        systemData->disableMainLoop ();
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_HOME)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_HOME keypress...");
        systemData->setDesiredPhysicsFrequency(150.0);
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_END)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_END keypress...");
        systemData->setDesiredPhysicsFrequency(300.0);
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_KP_MINUS keypress...");
        systemData->decreaseDesiredPhysicsRate();
    }
    if (systemData->axisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->getValue () == 1)
    {
        log->__format (LOG_DEVELOPER, "Processing a SDLK_KP_PLUS keypress...");
        systemData->increaseDesiredPhysicsRate();
    }
}

void recordVideoFrames ()
{
    static double time = 0;
    int takeShot = 0;
    if ((int) time >= SystemData::getSystemDataPointer ()->videoRecordTimestep)
    {
        takeShot++;
        time -= SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    }
    if ((takeShot > 0) && (SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->getValue () == 0))
    {
        SystemData::getSystemDataPointer ()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->setNewRawValue (1);
        takeShot--;
    }
    time += SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
}

LogEngine *processXmlRootNode (DOMNode * n)
{
    LogEngine *tmpLog = new LogEngine (LOG_DEVELOPER, "XML Parser");
    tmpLog->__format (LOG_DEVELOPER, "Assigning default values");
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "MAI";
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->__format (LOG_DEVELOPER, "Name: %s", name.c_str());
            if (name == "mainConfig")
            {
                tmpLog->__format (LOG_DEVELOPER, "Found the main config element.");
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
                            tmpLog->__format (LOG_ENDUSER, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->__format (LOG_ENDUSER, "Found the log name: %s", localLogName.c_str());
                        }
                        if (attribute == "videoRecordTimestep")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->__format (LOG_ENDUSER, "Found video recording timestep value: %s", attribute.c_str ());
                            SystemData::getSystemDataPointer ()->videoRecordTimestep = stoi (attribute);
                        }
                        if (attribute == "dataDir")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->__format (LOG_ENDUSER, "Found the data directory: %s", attribute.c_str());
                            Paths::setCustomDataDir(attribute);
                        }
                    }
                }
            }
        }
    }
    LogEngine *returnLog = new LogEngine (localLogLevel, localLogName.c_str ());
    delete tmpLog;
    returnLog->__format (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");
    returnLog->__format (LOG_DEVELOPER, "Unloading temporary parsing data from memory...");
    return returnLog;
}
