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
#include "ode.h"
#include "log/logEngine.hpp"

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
    torque = 0.0;
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
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics radius: %s", attribute.c_str() );
                radius = stod (attribute);
            }
            if (attribute == "width")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "mass")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "powered")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the wheel power transmission: %s", attribute.c_str() );
                powered = stod (attribute);
            }
            attribute.clear();
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

void Wheel::setRotation (Vector3d rotation)
{
    dMatrix3 rot;
    dRFromEulerAngles (rot, rotation.x, rotation.y, rotation.z);
    dBodySetRotation (wheelID, rot);
}

Vector3d Wheel::getRotation ()
{
    const dReal *temp = dBodyGetQuaternion (wheelID);
    return Vector3d (temp[0], temp[1], temp[2], temp[3]);
}

void Wheel::stopPhysics ()
{
    dGeomDestroy (wheelGeomID);
    dBodyDestroy (wheelID);
}

void Wheel::stepPhysics ()
{
    prevAngularVel = inputAngularVel;

    if(powered!=0){
//        dJointGetHingeAxis (suspJointID, wheelAxisVector);
//        dBodySetFiniteRotationAxis (wheelID, wheelAxisVector[0], wheelAxisVector[1], wheelAxisVector[2]);
//        log->format(LOG_DEVELOPER, "%s:FRAx=%f FRAy=%f FRAz=%f",index.c_str(), wheelAxisVector[0], wheelAxisVector[1], wheelAxisVector[2]);
        torque = inputTorqueTransfer;
    }
    // use hinge's angular rate as angular velocity of wheel (rad/s)
    inputAngularVel = dJointGetHinge2Angle2Rate (suspJointID)*-1;
//    inputAngularVel = dJointGetHingeAngleRate (suspJointID);

    // calculate angular acceleration      
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;

    // tire rolling resistance
    //torque -= 0.1*inputAngularVel;

    // FIXME prevent explosion from too high angular rates
 //   if ( inputAngularVel > 300 || inputAngularVel < -300 )
 //   {
 //       torque *= -1;
 //   }

    // accumulate torques on wheel
    dBodyAddRelTorque (wheelID, 0, 0, powered*torque);
    
    log->format(LOG_DEVELOPER, "%s:angVel=%f angAcc=%f torque=%f",index.c_str(), inputAngularVel, angularAcc, torque);

    torque = 0;
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Wheel::getInputAngularVel()
{
    return inputAngularVel;
}
