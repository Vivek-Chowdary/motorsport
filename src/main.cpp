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

int main (int argc, char **argv)
{
    // We start the main log engine.
    XmlFile * xmlFile = new XmlFile ("mainConfig.xml");
    LogEngine * log = processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    // We declare the 'global' data and engines.
    log->put (LOG_INFO, "( 1 ): Loading engines and libraries...");
    log->put (LOG_INFO, "Creating world data");
    new WorldData ();
    log->put (LOG_INFO, "Creating system data");
    new SystemData ();
    log->put (LOG_INFO, "Creating data engine");
    DataEngine *dataEngine = new DataEngine ();
    log->put (LOG_INFO, "Creating graphics engine");
    GraphicsEngine *graphicsEngine = new GraphicsEngine ();
    log->put (LOG_INFO, "Creating physics engine");
    PhysicsEngine *physicsEngine = new PhysicsEngine ();
    log->put (LOG_INFO, "Creating gui engine");
    GuiEngine *guiEngine = new GuiEngine ();

    log->put (LOG_INFO, "Starting SDL subsystems");
    startSdl (log);
    log->put (LOG_INFO, "Creating input engine");
    InputEngine *inputEngine = new InputEngine ();

    log->put (LOG_INFO, "Getting system data pointer");
    SystemData *systemData = SystemData::getSystemDataPointer ();

    // We load the world data from hdd into memory.
    log->put (LOG_INFO, "( 2 ): Loading world data...");
    log->put (LOG_INFO, "Loading initial world data");
    dataEngine->loadWorldData ();

    // We start the main loop.
    log->put (LOG_INFO, "( 3 ): Starting simulation...");
    log->put (LOG_INFO, "Initializating main loop");
    systemData->statisticsTime = systemData->simulationTime = SDL_GetTicks ();
    log->put (LOG_INFO, "Enabling main loop");
    systemData->enableMainLoop ();
    log->put (LOG_INFO, "Starting main loop");
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
            log->format (LOG_VERBOSE, "Main Loop Stats: graphicsFps=%i - physicsFps=%i", systemData->graphicsFrequency, systemData->physicsFrequency);
        }
        // Run the physics engine until the game time is in sync with the real loop time.
        while (((systemData->realTime - systemData->simulationTime) >= systemData->physicsTimeStep) && (systemData->isMainLoopEnabled ()))
        {
            systemData->simulationTime += systemData->physicsTimeStep;
            systemData->physicsSteps++;
            physicsEngine->computeStep ();
            inputEngine->computeStep ();
        }
        // Run the gui engine.
        guiEngine->computeStep ();
        // Run the graphics engine.
        graphicsEngine->computeStep ();
        systemData->graphicsSteps++;
    }
    log->put (LOG_INFO, "Main loop finished");

    // We unload the world data from memory.
    log->put (LOG_INFO, "( 4 ): Unloading world data...");
    log->put (LOG_INFO, "Unloading world data");
    dataEngine->unloadWorldData ();

    // We delete the 'global' data and engines.
    log->put (LOG_INFO, "( 5 ): Unloading engines and libraries...");
    log->put (LOG_INFO, "Deleting graphics engine");
    delete graphicsEngine;
    log->put (LOG_INFO, "Deleting physics engine");
    delete physicsEngine;
    log->put (LOG_INFO, "Deleting input engine");
    delete inputEngine;
    log->put (LOG_INFO, "Deleting gui engine");
    delete guiEngine;
    log->put (LOG_INFO, "Deleting data engine");
    delete dataEngine;
    log->put (LOG_INFO, "Deleting log engine");
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

LogEngine * processXmlRootNode (DOMNode * node)
{
    ParsingMainData * data = new ParsingMainData;
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    if (node)
    {
        if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (node->getNodeName ());
            log->format (LOG_INFO, "Name: %s", name);

            if (!strncmp (name, "mainConfig", 11))
            {
                log->put (LOG_INFO, "Found the main config element.");
                if (node->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = node->getAttributes ();
                    int nodeSize = pAttributes->getLength ();

                    for (int i = 0; i < nodeSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *name = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (name, "localLogLevel", 14))
                        {
                            XMLString::release (&name);
                            log->format (LOG_INFO, "\tFound the local log level:", name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(ParsingMainData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(ParsingMainData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(ParsingMainData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(ParsingMainData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(ParsingMainData *) data).localLogLevel = LOG_TRACE;
                        }
                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_INFO, "\tFound the log name:", name);

                            (*(ParsingMainData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(ParsingMainData *) data).localLogName, name, strlen (name) + 1);
                        }
                        XMLString::release (&name);
                    }
                }
            }
            XMLString::release (&name);
        }
    }
    delete log;

    LogEngine *returnLog = new LogEngine (data->localLogLevel, data->localLogName);
    returnLog->put (LOG_INFO, "Temporary parsing data already loaded into memory...");
    returnLog->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    delete[](data->localLogName);
    delete data;
    return returnLog;
}
