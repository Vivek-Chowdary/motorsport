/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "ode/ode.h"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"

void Wheel::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    double mass = 0.0;
    double radius = 0.0;
    double width = 0.0;
    powered = 0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    prevAngularVel = 0.0;
    angularAcc = 0.0;
    inputTorqueTransfer = 0.0;
    outputTorqueTransfer = 0.0;
    inertia = 1.0;
    friction = 0.1;
     
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
            if (attribute == "radius")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics radius: %s", attribute.c_str() );
                radius = stod (attribute);
            }
            if (attribute == "width")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "powered")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel power transmission: %s", attribute.c_str() );
                powered = stod (attribute);
            }
        }
    }
    dMass tmpMass;
//    dMassSetCylinderTotal (&tmpMass, mass, 3, radius, width);
    dMassSetParameters (&tmpMass, mass,
                         0, 0, 0,
                         0.237, 0.237, 0.409,
                         0, 0, 0);

    wheelID = dBodyCreate (World::getWorldPointer ()->worldID);
    wheelGeomID = dCreateCCylinder (World::getWorldPointer ()->spaceID, radius, width);
    dBodySetLinearVel  (wheelID, 0, 0, 0);
    dBodySetAngularVel (wheelID, 0, 0, 0);

    dGeomSetBody (wheelGeomID, wheelID);
    dBodySetMass (wheelID, &tmpMass);
//    setPosition (Vector3d (0, 0, 0) );
//    setRotation (Vector3d (0, 0, 0) );
    
//    if(powered) {
//        dBodySetFiniteRotationMode (wheelID, 1);
//    }
//    dBodySetFiniteRotationMode (wheelID, 1);
}

void Wheel::setPosition (Vector3d position)
{               
    dBodySetPosition (wheelID, position.x, position.y, position.z);
}

Vector3d Wheel::getPosition ()
{
    const dReal *temp = dBodyGetPosition (wheelID);
    return Vector3d (temp[0], temp[1], temp[2]);
}

void Wheel::applyRotation (Quaternion rotation)
{
    setPosition ( rotation.rotateObject(getPosition()) );
    dMatrix3 rot;
    Quaternion finalRotation = rotation * getRotation();
    finalRotation.getOdeMatrix (rot);
    dBodySetRotation (wheelID, rot);
}

Quaternion Wheel::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (wheelID);
    return Quaternion (temp);
}

void Wheel::stopPhysics ()
{
    dGeomDestroy (wheelGeomID);
    dBodyDestroy (wheelID);
}

void Wheel::stepPhysics ()
{
    prevAngularVel = inputAngularVel;

    // use hinge's angular rate as angular velocity of wheel (rad/s)
    inputAngularVel = dJointGetHinge2Angle2Rate (suspJointID)*powered*-1;
    //inputAngularVel = dJointGetHingeAngleRate (suspJointID); // old kart suspension code

    // calculate angular acceleration      
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();

    // tire rolling resistance
    //inputTorqueTransfer -= 0.1*inputAngularVel;

    // accumulate torques on wheel:
    // first, get the axis of the suspension
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (suspJointID, odeTAxis);
    Vector3d tAxis (odeTAxis);

    double brake = brakePedal->getNormalizedAngle();
    if (!userDriver) brake = 1;
    if (inputAngularVel > 0) brake *= -1;
    double maxBrakeTorque = 1250;
    inputTorqueTransfer += brake * maxBrakeTorque;
    
    // then, scale it by desired torque in the desired direction of the axis
    tAxis.scalarMultiply (inputTorqueTransfer * powered);

    // finally, apply it
    dBodyAddTorque (wheelID, tAxis.x, tAxis.y, tAxis.z);

    log->format(LOG_DEVELOPER, "%s:angVel=%f angAcc=%f torque=%f powered=%f axis=(%f,%f,%f)",index.c_str(), inputAngularVel, angularAcc, inputTorqueTransfer, powered, tAxis.x, tAxis.y, tAxis.z);
    
    // zero the accumulators
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Wheel::getInputAngularVel()
{
    return inputAngularVel;
}
