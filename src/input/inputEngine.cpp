/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "inputEngine.hpp"
#include "SDL/SDL.h"
#include "xmlParser.hpp"
#include "system.hpp"
#include "world.hpp"
#include "log/logEngine.hpp"
#include "track.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "axis.hpp"


InputEngine::InputEngine ()
{
    XmlFile * xmlFile = new XmlFile ("inputConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    log->put (LOG_INFO, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_INFO, "Initializing keyboard data array");
    keyState = SDL_GetKeyState (NULL);
    log->put (LOG_INFO, "Initializing mouse data variables");

    systemData->axisMap[getIDMouseAxis (0)] = new Axis; //axis X, mouse #1
    systemData->axisMap[getIDMouseAxis (1)] = new Axis; //axis Y, mouse #1
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (systemData->width);
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (0);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (-systemData->height);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (0);
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        systemData->axisMap[getIDKeyboardKey (i)] = new Axis;
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (1);
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (0);
    }
}

int InputEngine::computeStep (void)
{
    std::string tmpString;
    SDL_Event event;
    while (SDL_PollEvent (&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            // this can be the user cliking to close the window
            log->put (LOG_INFO, "New SDL_QUIT event: User wants to exit. Notifying to stop mainLoop...");
            systemData->disableMainLoop ();
            break;
        case SDL_KEYUP:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->format (LOG_VERBOSE, "New SDL_KEYUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(0);
            break;
        case SDL_KEYDOWN:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->format (LOG_VERBOSE, "New SDL_KEYDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(1);
            if (event.key.keysym.sym == SDLK_KP_MINUS)
            {
                log->put (LOG_VERBOSE, "Processing a SDLK_KP_MINUS keypress...");
                if (systemData->physicsDesiredFrequency < 37)
                {
                    if (!--systemData->physicsDesiredFrequency)
                    {
                        systemData->physicsDesiredFrequency++;
                    }
                    systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
                } else
                {
                    systemData->physicsTimeStep++;
                    systemData->physicsDesiredFrequency = 1000 / systemData->physicsTimeStep;
                }
            }
            if (event.key.keysym.sym == SDLK_KP_PLUS)
            {
                log->put (LOG_VERBOSE, "Processing a SDLK_KP_PLUS keypress...");
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
            break;
        case SDL_MOUSEMOTION:
            axisIDtoStr(getIDMouseAxis (0), tmpString);
            log->format (LOG_VERBOSE, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            axisIDtoStr(getIDMouseAxis (1), tmpString);
            log->format (LOG_VERBOSE, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (0)]->setNewRawValue (event.motion.x);
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (1)]->setNewRawValue (-event.motion.y);
            log->format (LOG_TRACE, "Mouse movement: (%i, %i)", event.motion.x, -event.motion.y);
            break;
        default:
            break;
        }
    }
    processKeyboard ();

    int numTrackCams = World::getWorldPointer()->trackList[0]->cameraList.size();
    for (int i=0; i < numTrackCams; i++)
    {
        World::getWorldPointer()->trackList[0]->cameraList[i]->stepInput();
    }

    return (0);
}

void InputEngine::processKeyboard ()
{
    // Processing key presses...
    if (keyState[SDLK_ESCAPE])
    {
        log->put (LOG_VERBOSE, "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop...");
        systemData->disableMainLoop ();
    }
    if (keyState[SDLK_HOME])
    {
        log->put (LOG_VERBOSE, "Processing a SDLK_HOME keypress...");
        systemData->physicsDesiredFrequency = 30;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    }

    if (keyState[SDLK_END])
    {
        log->put (LOG_VERBOSE, "Processing a SDLK_END keypress...");
        systemData->physicsDesiredFrequency = 250;
        systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    }

    if (keyState[SDLK_q])
    {
        log->put (LOG_INFO, "Processing a SDLK_q keypress: User wants to exit. Notifying to stop mainLoop...");
        systemData->disableMainLoop ();
    }
    // Processing key releases...
}

InputEngine::~InputEngine (void)
{
    // finally stop the log engine
    delete log;
}

void InputEngine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_TRACE;
    std::string localLogName = "INP";
    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_INFO, "Name: %s", name.c_str());
            if (name == "inputConfig")
            {
                tmpLog->put (LOG_INFO, "Found the input engine config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "localLogLevel")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_INFO, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            localLogName.clear();
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_INFO, "Found the log name: %s", localLogName.c_str());
                        }
                        attribute.clear();
                    }
                }
            }
            name.clear();
        }
    }
    delete tmpLog;
    log = new LogEngine (localLogLevel, localLogName.c_str());
    localLogName.clear();
    log->put (LOG_INFO, "All config has been parsed");
}
