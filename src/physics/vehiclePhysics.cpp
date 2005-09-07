/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "driveJoint.hpp"
#include "gearbox.hpp"
#include "finalDrive.hpp"
#include "wheel.hpp"
#include "pedal.hpp"
#include "suspension.hpp"
#include "SDL/SDL_keysym.h"
#include "quaternion.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    // empty
}
dBodyID Vehicle::getVehicleID()
{
    return body->bodyID;
}

void Vehicle::setPosition (Vector3d position)
{
    Vector3d posDiff = position - getPosition();
    log->format (LOG_DEVELOPER, "Difference in vehicle position: (%f, %f, %f).", posDiff.x, posDiff.y, posDiff.z);
    
    body->setPosition ( body->getPosition() + posDiff );

    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        wheelIter->second->setPosition ( wheelIter->second->getPosition() + posDiff );
    }

    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        // suspension position needs not be re-set.
        //suspIter->second->setPosition ( suspIter->second->getPosition() + posDiff );
    }
}

Vector3d Vehicle::getPosition ()
{
    return body->getPosition();
}

void Vehicle::applyRotation (Quaternion rotation)
{
    // apply rotation to wheels
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); ++wheelIter)
    {
        wheelIter->second->applyRotation (rotation);
    }

    // apply rotation to body
    body->applyRotation (rotation);

    // apply rotation to suspensions
/*
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); ++suspIter)
    {
        // suspension position needs not be re-set.
        //suspIter->second->applyRotation (rotation);
    }
*/
}

Quaternion Vehicle::getRotation ()
{
    return body->getRotation();
}

void Vehicle::stopPhysics ()
{
}

void Vehicle::placeWheelsOnSuspensions()
{
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        std::map < std::string, Wheel *>::iterator wheelIter =  wheelMap.find(suspIter->first);
        if (wheelIter == wheelMap.end())
        {
            log->format (LOG_ERROR, "No \"%s\" wheel was found!", suspIter->first.c_str());
        }else{
            log->format (LOG_DEVELOPER, "Placing wheel on suspension \"%s\"", suspIter->first.c_str());
            wheelIter->second->applyRotation (suspIter->second->getInitialWheelRotation());
            wheelIter->second->setPosition (suspIter->second->getInitialWheelPosition());
        }
    }
}

void Vehicle::boltWheelsToSuspensions()
{
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        std::map < std::string, Wheel *>::iterator wheelIter =  wheelMap.find(suspIter->first);
        if (wheelIter == wheelMap.end())
        {
            log->format (LOG_ERROR, "No \"%s\" wheel was found!", suspIter->first.c_str());
        }else{
            log->format (LOG_DEVELOPER, "Bolting wheel to suspension \"%s\"", suspIter->first.c_str());
            suspIter->second->attach(*(wheelIter->second), *this);
        }
    }
}

void Vehicle::stepPhysics ()
{
    std::map < std::string, Pedal * >::const_iterator pedalIter;
    for (pedalIter=pedalMap.begin(); pedalIter != pedalMap.end(); pedalIter++)
    {
        pedalIter->second->stepPhysics();
    }
    if ( userDriver )
    {
        // check the gearshift levers
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_a)]->getValue() == 1)
        {
            gearbox->gearUp();
        }
        if(SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_z)]->getValue() == 1)
        {
            gearbox->gearDown();
        }
    }

/*
    // Code for using a higher rate in tranny code, thus eliminating some lag from it. FIXME when double seconds are used instead of int milliseconds.
    const int freq = 3;
    SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() / freq);
    for (int i = 0; i<freq; i++){
// higher rate code continues below... */
    // step torque transfer components first
    clutch->stepPhysics();
    transfer->stepPhysics();
    rearDiff->stepPhysics();
        
    // step rigid bodies    
    engine->stepPhysics();
    gearbox->stepPhysics();
    finalDrive->stepPhysics();
    body->stepPhysics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
//        SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * freq);
        suspIter->second->stepPhysics();
//        SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() / freq);
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stepPhysics();
    }
//    }
//    SystemData::getSystemDataPointer()->setDesiredPhysicsTimestep (SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep() * freq);

    // print telemetry data
    if ( userDriver )
    {
        const dReal * tmp = dBodyGetLinearVel(body->bodyID);
        double velocity = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        tmp = dBodyGetPosition(body->bodyID);
        double distance = sqrt(tmp[0]*tmp[0]+tmp[1]*tmp[1]+tmp[2]*tmp[2]);
        log->log (LOG_ENDUSER, LOG_TELEMETRY | LOG_FILE, "%9.5f %12.8f %12.8f %12.8f %12.8f %s %12.8f", velocity, engine->getOutputAngularVel(), finalDrive->getInputAngularVel(), wheelMap["RearRight"]->getInputAngularVel(), wheelMap["RearLeft"]->getInputAngularVel(), gearbox->getCurrentGearLabel().c_str(), distance);
    }
}
