
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
    SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (0)] = new Axis; //axis X, mouse #1
    SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (1)] = new Axis; //axis Y, mouse #1
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (i)] = new Axis;
    }
}

int InputEngine::computeStep (void)
{
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
            log->format (LOG_VERBOSE, "New SDL_KEYUP event; axis ID = %i", getIDKeyboardKey (event.key.keysym.sym));
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(0);
            break;
        case SDL_KEYDOWN:
            log->format (LOG_VERBOSE, "New SDL_KEYDOWN event; axis ID = %i", getIDKeyboardKey (event.key.keysym.sym));
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(1);
            break;
        case SDL_MOUSEMOTION:
            log->put (LOG_VERBOSE, "New SDL_MOUSEMOTION event: updating mouse axis.");
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (0)]->setNewRawValue (event.motion.x);
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (1)]->setNewRawValue (-event.motion.y);
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
    if (keyState[SDLK_RETURN] || keyState[SDLK_ESCAPE])
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

    if (keyState[SDLK_KP_MINUS])
    {
        log->put (LOG_VERBOSE, "Processing a SDLK_KP_MINUS keypress...");
        // modify the physics engine rate
        // if current desired fps is below 37, it's better to decrease the fps (frames/sec.)...
        if (systemData->physicsDesiredFrequency < 37)
        {
            // don't let the rate fall below 1 frame per second (or don't allow more than 1000msecs. step)
            if (!--systemData->physicsDesiredFrequency)
            {
                systemData->physicsDesiredFrequency++;
            }
            systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
        } else
        {                       // ...otherwise, it's better to increase the timestep (msecs.)
            systemData->physicsTimeStep++;
            systemData->physicsDesiredFrequency = 1000 / systemData->physicsTimeStep;
        }
    }

    if (keyState[SDLK_KP_PLUS])
    {
        log->put (LOG_VERBOSE, "Processing a SDLK_KP_PLUS keypress...");
        // if current desired fps is below 37, it's better to increase the fps (frames/sec.)...
        if (systemData->physicsDesiredFrequency < 37)
        {
            systemData->physicsDesiredFrequency++;
            systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
        } else
        {                       // ...otherwise, it's better to decrease the timestep (msecs.)
            // don't let the step time fall below 1 msec. (or don't allow more than 1000fps)
            if (!--systemData->physicsTimeStep)
            {
                systemData->physicsTimeStep++;
            }
            systemData->physicsDesiredFrequency = 1000 / systemData->physicsTimeStep;
        }
    }

    if (keyState[SDLK_q])
    {
        log->put (LOG_INFO, "Processing a 'Q' keypress: User wants to exit. Notifying to stop mainLoop...");
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
