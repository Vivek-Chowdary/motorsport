/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "log/logEngine.hpp"
#include "world.hpp"
#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "clutch.hpp"
#include "gearbox.hpp"
#include "diff.hpp"
#include "wheel.hpp"
#include "suspension.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    velocity = 0.0;
    log->telemetry (LOG_TRACE, "VehSpeed EngineTorque DiffAngularVel RRWhAngulVel RLWhAngulVel VehPosition");
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
    // step torque transfer components first
    clutch->stepPhysics();
    transfer->stepPhysics();
    transferW1->stepPhysics();
    transferW2->stepPhysics();
        
    // step rigid bodies    
    engine->stepPhysics();
    gearbox->stepPhysics();
    diff->stepPhysics();
    body->stepPhysics();
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

    const dReal * bodyVel;
    bodyVel = dBodyGetLinearVel(body->bodyID);
    velocity = sqrt(bodyVel[0]*bodyVel[0]+bodyVel[1]*bodyVel[1]+bodyVel[2]*bodyVel[2]);    
    log->telemetry (LOG_TRACE, "%9.5f %12.8f %12.8f %12.8f %12.8f (%12.8f,%12.8f,%12.8f)", velocity, engine->getTorque(), diff->getAngularVel(), wheelMap["RearRight"]->getAngularVel(), wheelMap["RearLeft"]->getAngularVel(), getPosition().x, getPosition().y, getPosition().z);
}
