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
#include "clutch.hpp"
#include "gearbox.hpp"
#include "wheel.hpp"
#include "suspension.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}
dBodyID Vehicle::getVehicleID()
{
    return body->bodyID;
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = getPosition();
    log->format (LOG_INFO, "Setting vehicle position (%f, %f, %f) to (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z, position.x, position.y, position.z);
    posDiff = position - posDiff;
    log->format (LOG_INFO, "Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    
    body->setPosition (body->getPosition() + posDiff);

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        Vector3d newPos = suspIter->second->getPosition();
        newPos += posDiff;
        //log->format (LOG_INFO, "Setting suspension \"%s\" position to (%f, %f, %f)", suspIter->first.c_str(), newPos.x, newPos.y, newPos.z);
        //suspIter->second->setPosition (newPos);
    }

    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        Vector3d newPos = wheelIter->second->getPosition();
        newPos += posDiff;
        log->format (LOG_INFO, "Setting wheel \"%s\" position to (%f, %f, %f)", wheelIter->first.c_str(), newPos.x, newPos.y, newPos.z);
        wheelIter->second->setPosition (newPos);
    }
}

Vector3d Vehicle::getPosition ()
{
    return body->getPosition();
}

void Vehicle::setRotation (Vector3d rotation)
{
    Vector3d initialPos = getPosition();
    setPosition (Vector3d(0, 0, 0));

    Vector3d rotDiff = getRotation();
    log->format (LOG_INFO, "Setting vehicle rotation (%f, %f, %f) to (%f, %f, %f).", rotDiff.x, rotDiff.y, rotDiff.z, rotation.x, rotation.y, rotation.z);
    rotDiff = rotation - rotDiff;
    log->format (LOG_INFO, "Difference in vehicle rotation: (%f, %f, %f).", rotDiff.x, rotDiff.y, rotDiff.z);
    
    body->setRotation (body->getRotation() + rotDiff);

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        Vector3d newRot = suspIter->second->getRotation();
        newRot += rotDiff;
        //log->format (LOG_INFO, "Setting suspension \"%s\" rotation to (%f, %f, %f)", suspIter->first.c_str(), newRot.x, newRot.y, newRot.z);
        //suspIter->second->setRotation (newRot);
    }

    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        Vector3d newRot = wheelIter->second->getRotation();
        newRot += rotDiff;
        log->format (LOG_INFO, "Setting wheel \"%s\" rotation to (%f, %f, %f)", wheelIter->first.c_str(), newRot.x, newRot.y, newRot.z);
        wheelIter->second->setRotation (newRot);
        Vector3d rot = wheelIter->second->getRotation();
        log->format (LOG_INFO, "Wheel \"%s\" rotation set to (%f, %f, %f)", wheelIter->first.c_str(), rot.x, rot.y, rot.z);
    }

    setPosition(initialPos);
}

Vector3d Vehicle::getRotation ()
{
    return body->getRotation();
}

void Vehicle::stopPhysics ()
{
    body->stopPhysics();
    engine->stopPhysics();
    clutch->stopPhysics();
    gearbox->stopPhysics();
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

void Vehicle::attachWheelsToBody()
{
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        std::map < std::string, Wheel *>::iterator wheelIter =  wheelMap.find(suspIter->first);
        if (wheelIter == wheelMap.end())
        {
            log->format (LOG_ERROR, "No \"%s\" wheel was found!", suspIter->first.c_str());
        }else{
            log->format (LOG_INFO, "Attaching wheel and suspension \"%s\"", suspIter->first.c_str());
            suspIter->second->attach(*(wheelIter->second), *this);
        }
    }
}

void Vehicle::stepPhysics ()
{
    body->stepPhysics();
    engine->stepPhysics();
    clutch->stepPhysics();
    gearbox->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stepPhysics();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->addTorque (gearbox->getTorque());
        wheelIter->second->stepPhysics();
    }
}
