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

    log->put (LOG_DEVELOPER, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_DEVELOPER, "Initializing mouse axis");
    systemData->axisMap[getIDMouseAxis (0)] = new Axis; //axis X, mouse #1
    systemData->axisMap[getIDMouseAxis (1)] = new Axis; //axis Y, mouse #1
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (systemData->width);
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (0);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (-systemData->height);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (0);
    
    log->put (LOG_DEVELOPER, "Initializing keyboard axis");
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        systemData->axisMap[getIDKeyboardKey (i)] = new Axis;
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (1);
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (0);
    }
    
    log->put (LOG_DEVELOPER, "Initializing dummy joystick devices");
    systemData->axisMap[getIDJoyAxis (0, 0)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 0)]->setNewRawValue (32767);
    systemData->axisMap[getIDJoyAxis (0, 0)]->setNewRawValue (-32767);
    systemData->axisMap[getIDJoyAxis (0, 0)]->setNewRawValue (0);
    systemData->axisMap[getIDJoyAxis (0, 1)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 1)]->setNewRawValue (-32767);
    systemData->axisMap[getIDJoyAxis (0, 1)]->setNewRawValue (32767);
    systemData->axisMap[getIDJoyAxis (0, 2)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 2)]->setNewRawValue (-32767);
    systemData->axisMap[getIDJoyAxis (0, 2)]->setNewRawValue (32767);
    
    log->put (LOG_DEVELOPER, "Initializing real joystick devices");
    int nJoysticks = SDL_NumJoysticks();
    for (int joy = 0; joy < nJoysticks; joy++ ) 
    {
        SDL_Joystick * joystick = SDL_JoystickOpen(joy);
        int nAxis = SDL_JoystickNumAxes(joystick);
        int nButtons = SDL_JoystickNumButtons(joystick);
        log->format(LOG_ENDUSER, "Found joystick \"%s\" with %i axis and %i buttons.", SDL_JoystickName(joy), nAxis, nButtons);
        for (int axis = 0; axis < nAxis; axis++)
        {
            if (!( (joy == 0) && ((axis == 0)||(axis==1)||(axis==2)) ))
            {
                systemData->axisMap[getIDJoyAxis (joy, axis)] = new Axis;
            }
            if (axis == 1 || axis == 2)
            {
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            } else {
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (2);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            }
            log->format (LOG_DEVELOPER, "Joystick axis #%i initialized. %f", getIDJoyAxis(joy, axis), SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->getValue());
        }
        for (int button = 0; button < nButtons; button++)
        {
            systemData->axisMap[getIDJoyButton (joy, button)] = new Axis;
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (1);
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (0);
            log->format (LOG_DEVELOPER, "Joystick axis #%i initialized.", getIDJoyButton(joy, button));
        }
    }
    log->format (LOG_ENDUSER, "%i joystick%s found.", nJoysticks, (nJoysticks==1)?" was":"s were");
//    log->telemetry (LOG_TRACE, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");
}

void InputEngine::clearLogicEventAxis ()
{
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_HOME)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_END)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_q)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_t)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_b)]->setNewRawValue(0);
}

void InputEngine::clearGraphicsEventAxis ()
{
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_PRINT)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_c)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_v)]->setNewRawValue(0);
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
            log->put (LOG_DEVELOPER, "New SDL_QUIT event: User wants to exit. Notifying to stop mainLoop...");
            systemData->disableMainLoop ();
            break;
        case SDL_KEYUP:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->format (LOG_DEVELOPER, "New SDL_KEYUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(0);
            break;
        case SDL_KEYDOWN:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->format (LOG_DEVELOPER, "New SDL_KEYDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(1);
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                log->put (LOG_DEVELOPER, "Processing a SDLK_ESCAPE keypress: notifying to stop mainLoop...");
                systemData->disableMainLoop ();
                break;
            case SDLK_HOME:
                log->put (LOG_DEVELOPER, "Processing a SDLK_HOME keypress...");
                systemData->physicsDesiredFrequency = 30;
                systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
                break;
            case SDLK_END:
                log->put (LOG_DEVELOPER, "Processing a SDLK_END keypress...");
                systemData->physicsDesiredFrequency = 250;
                systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
                break;
            case SDLK_q:
                log->put (LOG_DEVELOPER, "Processing a SDLK_q keypress: User wants to exit. Notifying to stop mainLoop...");
                systemData->disableMainLoop ();
                break;
            case SDLK_KP_MINUS:
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
                break;
            case SDLK_KP_PLUS:
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
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            axisIDtoStr(getIDMouseAxis (0), tmpString);
            log->format (LOG_DEVELOPER, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            axisIDtoStr(getIDMouseAxis (1), tmpString);
            log->format (LOG_DEVELOPER, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (0)]->setNewRawValue (event.motion.x);
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (1)]->setNewRawValue (-event.motion.y);
            log->format (LOG_DEVELOPER, "Mouse movement: (%i, %i)", event.motion.x, -event.motion.y);
            break;
        case SDL_JOYAXISMOTION:
            id = getIDJoyAxis (event.jaxis.which, event.jaxis.axis);
            axisIDtoStr(id, tmpString);
            log->format (LOG_DEVELOPER, "New SDL_JOYAXISMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (event.jaxis.value);
            log->format (LOG_DEVELOPER, "Joystick axis movement: (%i)", event.jaxis.value);
            break;
        case SDL_JOYBUTTONDOWN:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->format (LOG_DEVELOPER, "New SDL_JOYBUTTONDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (1);
            break;
        case SDL_JOYBUTTONUP:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->format (LOG_DEVELOPER, "New SDL_JOYBUTTONUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (0);
            break;
        default:
            break;
        }
    }

    int numTrackCams = World::getWorldPointer()->trackList[0]->cameraList.size();
    for (int i=0; i < numTrackCams; i++)
    {
        World::getWorldPointer()->trackList[0]->cameraList[i]->stepInput();
    }
    
 /* Be careful with this debugging code!! you need to have created all the axis you're going to debug!
    log->telemetry (LOG_ENDUSER, "%5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
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
    log->telemetry (LOG_ENDUSER, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");*/

    return (0);
}

InputEngine::~InputEngine (void)
{
    // finally stop the log engine
    delete log;
}

void InputEngine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "INP";
    LogEngine * tmpLog = new LogEngine (LOG_DEVELOPER, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_DEVELOPER, "Name: %s", name.c_str());
            if (name == "inputConfig")
            {
                tmpLog->put (LOG_DEVELOPER, "Found the input engine config element.");
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
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_ENDUSER, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the log name: %s", localLogName.c_str());
                        }
                    }
                }
            }
        }
    }
    delete tmpLog;
    log = new LogEngine (localLogLevel, localLogName.c_str());
    log->put (LOG_DEVELOPER, "All config has been parsed");
}
