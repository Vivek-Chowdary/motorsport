/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "pedal.hpp"
#include "engine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"

Pedal::Pedal (WorldObject * container, XmlTag * tag)
    :WorldObject(container, "pedal")
{
    log->__format (LOG_CCREATOR, "Starting to parse a pedal node");
    angleRange = 180;
    currentAngle = 0.0;
    if (tag->getName() == "pedal")
    {
        setName (     tag->getAttribute("name"));
        angleRange = stod (tag->getAttribute("angleRange"));
    }
    userDriver = false;
}

Pedal::~Pedal ()
{
}


void Pedal::setUserDriver ()
{
    userDriver = true;
}

void Pedal::stepPhysics ()
{
    currentAngle = 0;
    if ( userDriver )
    {
        if (getName() == "gasPedal")
        {
            currentAngle = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,2)]->getValue();
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP9)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP6)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP3)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_UP)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (getName() == "brakePedal")
        {
            currentAngle = 1 - SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,1)]->getValue();
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP8)]->getValue() * 3 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP5)]->getValue() * 2 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP2)]->getValue() * 1 / 3;
                        if (currentAngle == 0) {
                            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_DOWN)]->getValue() * 3 / 3;
            }   }   }   }
        } else if (getName() == "clutchPedal")
        {
            currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP7)]->getValue() * 3 / 3;
            if (currentAngle == 0) {
                currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP4)]->getValue() * 2 / 3;
                if (currentAngle == 0) {
                    currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_KP1)]->getValue() * 1 / 3;
                    if (currentAngle == 0) {
                        currentAngle = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_q)]->getValue() * 3 / 3;
            }   }   }
        }
    }
    currentAngle *= angleRange;
}

double Pedal::getNormalizedAngle()
{
    log->__format (LOG_CCREATOR, "Reading pedal \"%s\" angle: %f", getName().c_str(), currentAngle );
    return (currentAngle / angleRange);
}
