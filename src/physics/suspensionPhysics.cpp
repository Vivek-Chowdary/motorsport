/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "suspension.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "vector3d.hpp"
#include "vehicle.hpp"
#include "body.hpp"
#include "wheel.hpp"
#include "axis.hpp"
#include "SDL/SDL_keysym.h"


void Suspension::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    springConstant = 0;
    dampingConstant = 0;
    steeringAngle = 0.0;
    position = Vector3d (0, 0, 0);
    rotation = Quaternion (1, 0, 0, 0);
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
            if (attribute == "position")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the position: %s", attribute.c_str());
                position = stov3d (attribute);
            }
            if (attribute == "rotation")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the rotation: %s", attribute.c_str());
                rotation = stoq (attribute);
            }
            if (attribute == "springConstant")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the suspension spring constant: %s", attribute.c_str() );
                springConstant = stod (attribute);
            }
            if (attribute == "dampingConstant")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the suspension damping constant: %s", attribute.c_str() );
                dampingConstant = stod (attribute);
            }
            if (attribute == "steeringAngle")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the suspension max. steering angle: %s", attribute.c_str() );
                steeringAngle = stod (attribute);
            }
        }
    }
    
//    jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
    jointID = dJointCreateHinge2 (World::getWorldPointer()->worldID, 0);
    dJointAttach (jointID, 0, 0);
}

void Suspension::attach (Wheel & wheel, Vehicle & vehicle)
{
    wheel.setSuspJoint (jointID);
    dJointAttach (jointID, vehicle.body->bodyID, wheel.wheelID);

    // Set suspension travel limits. one needs to be done before the other, can't recall which one, so it's dupped
/*    dJointSetHinge2Param (jointID, dParamHiStop2, +0.01);
      dJointSetHinge2Param (jointID, dParamLoStop2, -0.01);
      dJointSetHinge2Param (jointID, dParamHiStop2, +0.01);
*/
    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis
    dBodySetFiniteRotationMode(wheel.wheelID, 1);
    dBodySetFiniteRotationAxis(wheel.wheelID, 0, 0, 1);

    double h = SystemData::getSystemDataPointer()->physicsTimeStep / 1000.0 ;
    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
    Vector3d wPosition = wheel.getPosition();
    dJointSetHinge2Anchor (jointID, wPosition.x, wPosition.y, wPosition.z);
    
    Quaternion wRotation = wheel.getRotation();
    Vector3d rAxis1 = wRotation.rotateObject(Vector3d(0, 1, 0));
    dJointSetHinge2Axis1 (jointID, rAxis1.x, rAxis1.y, rAxis1.z);
    Vector3d rAxis2 = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHinge2Axis2 (jointID, rAxis2.x, rAxis2.y, rAxis2.z);
    log->format (LOG_DEVELOPER, "Axis2 = %f, %f, %f.", rAxis2.x, rAxis2.y, rAxis2.z);
    
    // old kart suspension
    //dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
    //dJointSetHingeAxis (jointID, 0,1,0);
}
Vector3d Suspension::getInitialWheelPosition ()
{
    return position;
}
Quaternion Suspension::getInitialWheelRotation ()
{
    return rotation;
}

void Suspension::stopPhysics ()
{
    dJointDestroy (jointID);
}

void Suspension::stepPhysics ()
{
    double angle = 0;
    double leftSteering = 0;
    if ( userDriver )
    {
        leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (leftSteering < 0.5)
        {
            leftSteering = 0.5 - leftSteering;
            leftSteering *= 2.0;
        } else {
            leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_1)]->getValue() * 5 / 5;
            if (leftSteering == 0) {
                leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_2)]->getValue() * 4 / 5;
                if (leftSteering == 0) {
                    leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_3)]->getValue() * 3 / 5;
                    if (leftSteering == 0) {
                        leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_4)]->getValue() * 2 / 5;
                        if (leftSteering == 0) {
                            leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_5)]->getValue() * 1 / 5;
                            if (leftSteering == 0) {
                                leftSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_LEFT)]->getValue() * 5 / 5;
        }   }   }   }   }   }
    }
    double rightSteering = 0;
    if ( userDriver )
    {
        rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (rightSteering > 0.5)
        {
            rightSteering = rightSteering - 0.5;
            rightSteering *= 2.0;
        } else {
            rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_0)]->getValue() * 5 / 5;
            if (rightSteering == 0) {
                rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_9)]->getValue() * 4 / 5;
                if (rightSteering == 0) {
                    rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_8)]->getValue() * 3 / 5;
                    if (rightSteering == 0) {
                        rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_7)]->getValue() * 2 / 5;
                        if (rightSteering == 0) {
                            rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_6)]->getValue() * 1 / 5;
                            if (rightSteering == 0) {
                                rightSteering = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_RIGHT)]->getValue() * 5 / 5;
        }   }   }   }   }   }
    }

    // Override keyboard data with joystick axis if the keyboard is not used.
    const double piK = 3.14159265358979323846264338327950288419716939937510 / 180;
    rightSteering *= steeringAngle * piK / 2;
    leftSteering *= steeringAngle * piK / 2;
    angle += rightSteering - leftSteering;

    // Set wheel steering limits. one needs to be done before the other, can't recall which one, so it's dupped
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);
    dJointSetHinge2Param (jointID, dParamLoStop, angle-0.0000001);
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);

    // Re-set suspension settings... if physics rate has changed, this needs to be updated.
    double h = SystemData::getSystemDataPointer()->physicsTimeStep / 1000.0 ;
    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
}
