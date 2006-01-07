/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "inputEngine.hpp"
#include "SDL/SDL_types.h"
#include "SDL/SDL.h"
#include "system.hpp"
#include "world.hpp"
#include "log/logEngine.hpp"
#include "area.hpp"
#include "camera.hpp"
#include "part.hpp"
#include "axis.hpp"


InputEngine::InputEngine ()
{
#ifdef MACOSX
    XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/inputConfig.xml");
#else
    XmlTag * tag = new XmlTag ("../cfg/inputConfig.xml");
#endif    
    delete tag;
    log = new LogEngine (LOG_DEVELOPER, "InputEngine");

    log->__format (LOG_DEVELOPER, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();

    log->__format (LOG_DEVELOPER, "Initializing mouse axis");
    systemData->axisMap[getIDMouseAxis (0)] = new Axis; //axis X, mouse #1
    systemData->axisMap[getIDMouseAxis (1)] = new Axis; //axis Y, mouse #1
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (systemData->width);
    systemData->axisMap[getIDMouseAxis (0)]->setNewRawValue (0);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (-systemData->height);
    systemData->axisMap[getIDMouseAxis (1)]->setNewRawValue (0);
    
    log->__format (LOG_DEVELOPER, "Initializing keyboard axis");
    for (int i=SDLK_FIRST; i<SDLK_LAST; i++)
    {
        systemData->axisMap[getIDKeyboardKey (i)] = new Axis;
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (1);
        systemData->axisMap[getIDKeyboardKey (i)]->setNewRawValue (0);
    }
    
    log->__format (LOG_DEVELOPER, "Initializing dummy joystick devices");
    systemData->axisMap[getIDJoyAxis (0, 0)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 1)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 1)]->setNewRawValue (0);
    systemData->axisMap[getIDJoyAxis (0, 1)]->setNewRawValue (1);
    systemData->axisMap[getIDJoyAxis (0, 2)] = new Axis;
    systemData->axisMap[getIDJoyAxis (0, 2)]->setNewRawValue (0);
    systemData->axisMap[getIDJoyAxis (0, 2)]->setNewRawValue (1);
    
    log->__format (LOG_DEVELOPER, "Initializing real joystick devices");
    int nJoysticks = SDL_NumJoysticks();
    for (int joy = 0; joy < nJoysticks; joy++ ) 
    {
        SDL_Joystick * joystick = SDL_JoystickOpen(joy);
        int nAxis = SDL_JoystickNumAxes(joystick);
        int nButtons = SDL_JoystickNumButtons(joystick);
        log->__format(LOG_ENDUSER, "Found joystick \"%s\" with %i axis and %i buttons.", SDL_JoystickName(joy), nAxis, nButtons);
        for (int axis = 0; axis < nAxis; axis++)
        {
            if (!( (joy == 0) && ((axis == 0)||(axis==1)||(axis==2)) ))
            {
                systemData->axisMap[getIDJoyAxis (joy, axis)] = new Axis;
            }
            if (axis == 1 || axis == 2)
            {
                systemData->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (0);
                systemData->axisMap[getIDJoyAxis (joy, axis)]->setNewRawValue (1);
            }
            log->__format (LOG_DEVELOPER, "Joystick axis #%i initialized. %f", getIDJoyAxis(joy, axis), SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis (joy, axis)]->getValue());
        }
        for (int button = 0; button < nButtons; button++)
        {
            systemData->axisMap[getIDJoyButton (joy, button)] = new Axis;
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (1);
            SystemData::getSystemDataPointer()->axisMap[getIDJoyButton (joy, button)]->setNewRawValue (0);
            log->__format (LOG_DEVELOPER, "Joystick axis #%i initialized.", getIDJoyButton(joy, button));
        }
    }

    log->__format (LOG_DEVELOPER, "Discard initial events.");
    SDL_Event event;
    while (SDL_PollEvent (&event))
    {
        //empty
    }
    
    log->__format (LOG_ENDUSER, "%i joystick%s found.", nJoysticks, (nJoysticks==1)?" was":"s were");
//    log->telemetry (LOG_TRACE, " A0    A1    A2    A3    A4    A5    B0    B1    B2    B3    B4    B5");
}

void InputEngine::clearLogicEventAxis ()
{
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_HOME)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_END)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_KP_MINUS)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_KP_PLUS)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_a)]->setNewRawValue(0);
    SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (SDLK_z)]->setNewRawValue(0);
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
            log->__format (LOG_DEVELOPER, "New SDL_QUIT event: User wants to exit. Notifying to stop mainLoop...");
            systemData->disableMainLoop ();
            break;
        case SDL_KEYUP:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_KEYUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(0);
            break;
        case SDL_KEYDOWN:
            axisIDtoStr(getIDKeyboardKey (event.key.keysym.sym), tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_KEYDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey (event.key.keysym.sym)]->setNewRawValue(1);
            break;
        case SDL_MOUSEMOTION:
            axisIDtoStr(getIDMouseAxis (0), tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            axisIDtoStr(getIDMouseAxis (1), tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_MOUSEMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (0)]->setNewRawValue (event.motion.x);
            SystemData::getSystemDataPointer()->axisMap[getIDMouseAxis (1)]->setNewRawValue (-event.motion.y);
            log->__format (LOG_DEVELOPER, "Mouse movement: (%i, %i)", event.motion.x, -event.motion.y);
            break;
        case SDL_JOYAXISMOTION:
            id = getIDJoyAxis (event.jaxis.which, event.jaxis.axis);
            axisIDtoStr(id, tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_JOYAXISMOTION event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (event.jaxis.value);
            log->__format (LOG_DEVELOPER, "Joystick axis movement: (%i)", event.jaxis.value);
            break;
        case SDL_JOYBUTTONDOWN:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_JOYBUTTONDOWN event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (1);
            break;
        case SDL_JOYBUTTONUP:
            id = getIDJoyButton (event.jbutton.which, event.jbutton.button);
            axisIDtoStr(id, tmpString);
            log->__format (LOG_DEVELOPER, "New SDL_JOYBUTTONUP event: %s", tmpString.c_str());
            SystemData::getSystemDataPointer()->axisMap[id]->setNewRawValue (0);
            break;
        default:
            break;
        }
    }

    int numAreaCams = World::getWorldPointer()->areaList[0]->cameraList.size();
    for (int i=0; i < numAreaCams; i++)
    {
        //World::getWorldPointer()->areaList[0]->cameraList[i]->stepInput();
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
    log->__format (LOG_DEVELOPER, "Steering wheel: %i, %i.",systemData->axisMap[getIDJoyAxis (0, 0)]->getMinRawValue(), systemData->axisMap[getIDJoyAxis (0, 0)]->getMaxRawValue());
    log->__format (LOG_DEVELOPER, "Gas pedal:      %i, %i.",systemData->axisMap[getIDJoyAxis (0, 2)]->getMinRawValue(), systemData->axisMap[getIDJoyAxis (0, 2)]->getMaxRawValue());
    log->__format (LOG_DEVELOPER, "Brake pedal:    %i, %i.",systemData->axisMap[getIDJoyAxis (0, 1)]->getMinRawValue(), systemData->axisMap[getIDJoyAxis (0, 1)]->getMaxRawValue());
    // finally stop the log engine
    delete log;
}
