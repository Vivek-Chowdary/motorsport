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

    log->put (LOG_INFO, "Initializing mouse axis");
    systemData->axisMap[getIDMouseAxis (0)] = new Axis; //axis X, mouse #1
    systemData->axisMap[getIDMouseAxis (1)] = new Axis; //axis Y, mouse #1
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (systemData->width);
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (0);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (-systemData->height);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (0);
    
    log->put (LOG_INFO, "Initializing keyboard data array");
    keyState = SDL_GetKeyState (NULL);
    
    log->put (LOG_INFO, "Initializing keyboard axis");
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        systemData->axisMap[getIDKeyboardKey (i)] = new Axis;
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (1);
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (0);
    }
    
    log->put (LOG_INFO, "Initializing joystick axis");
    log->put (LOG_WARNING, "Using hardcoded joystick bindings. Don't use more than 5 joysticks, 10 axis per joystick, or 20 buttons per joystick!");
    // Since axis-bindings are hardcoded in Evo1, we create a couple of joystick axis here:
    //  5 joysticks
    //    20 buttons each
    //    10 axis each
    for (int joy = 0; joy < 5; joy++)
    {
        for (int axis = 0; axis < 10; axis++)
        {
            systemData->axisMap[getIDJoyAxis (joy, axis)] = new Axis;
            if (axis == 1 || axis == 2)
            {
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            } else {
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (2);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            }
            log->format (LOG_INFO, "Joystick axis #%i initialized. %f", getIDJoyAxis(joy, axis), SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->getValue());
        }
        for (int button = 0; button < 20; button++)
        {
            systemData->axisMap[getIDJoyButton (joy, button)] = new Axis;
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (1);
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (0);
            log->format (LOG_INFO, "Joystick axis #%i initialized.", getIDJoyButton(joy, button));
        }
    }
    log->put (LOG_INFO, "Hardcoded joystick axis initializated.");

    log->put (LOG_INFO, "Printing info about what joysticks have actually been found on the system...");
    // Now we'll log what's available, just for informative purposes.
    int nJoysticks = SDL_NumJoysticks();
    for(int nJoy = 0; nJoy < nJoysticks; nJoy++ ) 
    {
        SDL_Joystick * joystick = SDL_JoystickOpen(nJoy);
        log->format(LOG_INFO, "Found joystick \"%s\" with %i axis and %i buttons.", SDL_JoystickName(nJoy), SDL_JoystickNumAxes(joystick), SDL_JoystickNumButtons(joystick));
    }
    log->format (LOG_INFO, "%i joystick%s found.", nJoysticks, (nJoysticks==1)?" was":"s were");

    log->telemetry (LOG_TRACE, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");
}

int InputEngine::computeStep (void)
{
    std::string tmpString;
    int id;
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
        case SDL_JOYAXISMOTION:
            id = getIDJoyAxis (event.jaxis.which, event.jaxis.axis);
            axisIDtoStr(id, tmpString);
            log->format (LOG_VERBOSE, "New SDL_JOYAXISMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (event.jaxis.value);
            log->format (LOG_TRACE, "Joystick axis movement: (%i)", event.jaxis.value);
            break;
        case SDL_JOYBUTTONDOWN:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->format (LOG_VERBOSE, "New SDL_JOYBUTTONDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (1);
            break;
        case SDL_JOYBUTTONUP:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->format (LOG_VERBOSE, "New SDL_JOYBUTTONUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (0);
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
    
    
    log->telemetry (LOG_TRACE, "%5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,0)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,1)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,2)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,3)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,4)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,5)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,0)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,1)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,2)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,3)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,4)]->getValue(),
                                   SystemData::getSystemDataPointer()->axisMap[getIDJoyButton(0,5)]->getValue());
 //   log->telemetry (LOG_TRACE, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");

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
