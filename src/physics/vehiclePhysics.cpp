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

#include "log/logEngine.hpp"
#include "world.hpp"
#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "wheel.hpp"
#include "suspension.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}

void Vehicle::setPosition (Vector3d position)
{
/*    log->put(LOG_INFO, "Setting body position");
    body->setPosition (position);

    log->put(LOG_INFO, "Setting suspensions position");
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        Vector3d absPos = suspIter->second->getPosition();
        absPos -= position;
        suspIter->second->setPosition(absPos);
    }
    log->put(LOG_INFO, "Setting wheels position");
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        Vector3d absPos = wheelIter->second->getPosition();
        absPos -= position;
        wheelIter->second->setPosition(absPos);
    }*/
}

Vector3d Vehicle::getPosition ()
{
    return body->getPosition();
}

void Vehicle::setRotation (Vector3d rotation)
{
    // Move to center
    Vector3d position = getPosition();
    setPosition(Vector3d(0, 0, 0));

    //Rotate around center
    body->setRotation (rotation);
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        Vector3d absRot = suspIter->second->getRotation();
        absRot -= rotation;
        suspIter->second->setRotation(absRot);
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        Vector3d absRot = wheelIter->second->getRotation();
        absRot -= rotation;
        wheelIter->second->setRotation(absRot);
    }

    //Move back to original position
    setPosition(position);
}
Vector3d Vehicle::getRotation ()
{
    return body->getRotation();
}

void Vehicle::stopPhysics ()
{
    body->stopPhysics();
    engine->stopPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stopPhysics();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stopPhysics();
    }
}

void Vehicle::stepPhysics ()
{
    body->stepPhysics();
    engine->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stepPhysics();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stepPhysics();
    }
/*    wheelMap[2]->addTorque (engine->getTorque());
    wheelMap[3]->addTorque (-engine->getTorque());*/
}
