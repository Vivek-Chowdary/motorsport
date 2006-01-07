/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "suspension.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "world.hpp"
#include "system.hpp"
#include "ode/ode.h"
#include "vector3d.hpp"
#include "vehicle.hpp"
#include "body.hpp"
#include "wheel.hpp"
#include "axis.hpp"
#include "SDL/SDL_keysym.h"

Suspension::Suspension (WorldObject * container, std::string name)
    :WorldObject(container, name)
{
    position = Vector3d (0, 0, 0);
    rotation = Quaternion (1, 0, 0, 0);
}

Suspension::~Suspension ()
{
}

void Suspension::setUserDriver ()
{
    userDriver = true;
}

Vector3d Suspension::getSecondLinkPosition ()
{
    return position;
}
Quaternion Suspension::getSecondLinkRotation ()
{
    return rotation;
}
double Unidimensional::getSteeringAngle()
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
    rightSteering *= maxSteeringAngle * piK / 2;
    leftSteering *= maxSteeringAngle * piK / 2;
    angle += rightSteering - leftSteering;
    return angle;
}
double Unidimensional::getRate()
{
    return dJointGetHinge2Angle2Rate (jointID);
}
Vector3d Unidimensional::getAxis()
{
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (jointID, odeTAxis);
    return Vector3d (odeTAxis);
}
void Unidimensional::setVelocity(double velocity)
{
    dJointSetHinge2Param(jointID, dParamVel, 0);
}

Unidimensional::Unidimensional (WorldObject * container, XmlTag * tag)
    :Suspension(container, "suspension.unidimensional")
{
    userDriver = false;
    springConstant = 0;
    dampingConstant = 0;
    maxSteeringAngle = 0.0;
    if (tag->getName() == "suspension.unidimensional")
    {
        setName (     tag->getAttribute("name"));
        position = Vector3d (tag->getAttribute("position"));
        rotation = Quaternion (tag->getAttribute("rotation"));
        springConstant = stod(tag->getAttribute("springConstant"));
        dampingConstant = stod(tag->getAttribute("dampingConstant"));
        maxSteeringAngle = stod(tag->getAttribute("steeringAngle"));
    }
//    jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
    jointID = dJointCreateHinge2 (World::getWorldPointer()->worldID, 0);
    dJointAttach (jointID, 0, 0);
}
void Unidimensional::stepPhysics()
{
    double angle = getSteeringAngle();
    // Set wheel steering limits. one needs to be done before the other, can't recall which one, so it's dupped
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);
    dJointSetHinge2Param (jointID, dParamLoStop, angle-0.0000001);
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);

    // Re-set suspension settings... if physics rate has changed, this needs to be updated.
    double h = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
}
Unidimensional::~Unidimensional()
{
    dJointDestroy (jointID);
}
void Unidimensional::attach(WorldObject * base, WorldObject * object)
{
    Wheel * wheel = dynamic_cast<Wheel*>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(this);
    dJointAttach (jointID, base->getMainOdeObject()->getBodyID(), object->getMainOdeObject()->getBodyID());

    // Set suspension travel limits. one needs to be done before the other, can't recall which one, so it's dupped
/*    dJointSetHinge2Param (jointID, dParamHiStop2, +0.01);
      dJointSetHinge2Param (jointID, dParamLoStop2, -0.01);
      dJointSetHinge2Param (jointID, dParamHiStop2, +0.01);
*/
    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis
    dBodySetFiniteRotationMode(object->getMainOdeObject()->getBodyID(), 1);
    dBodySetFiniteRotationAxis(object->getMainOdeObject()->getBodyID(), 0, 0, 1);

    double h = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
    Vector3d wPosition = object->getPosition();
    dJointSetHinge2Anchor (jointID, wPosition.x, wPosition.y, wPosition.z);
    
    Quaternion wRotation = getSecondLinkRotation();
    Vector3d rAxis1 = wRotation.rotateObject(Vector3d(0, 1, 0));
    dJointSetHinge2Axis1 (jointID, rAxis1.x, rAxis1.y, rAxis1.z);
    Vector3d rAxis2 = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHinge2Axis2 (jointID, rAxis2.x, rAxis2.y, rAxis2.z);
    log->__format (LOG_DEVELOPER, "Axis2 = %f, %f, %f.", rAxis2.x, rAxis2.y, rAxis2.z);
    
    // old kart suspension
    //dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
    //dJointSetHingeAxis (jointID, 0,1,0);
}




Fixed::Fixed (WorldObject * container, XmlTag * tag)
    :Suspension(container, "suspension.unidimensional")
{
    userDriver = false;
    if (tag->getName() == "suspension.fixed")
    {
        setName (     tag->getAttribute("name"));
        position = Vector3d (tag->getAttribute("position"));
        rotation = Quaternion (tag->getAttribute("rotation"));
    }
    jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
    dJointAttach (jointID, 0, 0);
}
void Fixed::stepPhysics()
{
    // Empty...
}
Fixed::~Fixed()
{
    dJointDestroy (jointID);
}
void Fixed::attach(WorldObject * base, WorldObject * object)
{
    Wheel * wheel = dynamic_cast<Wheel*>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(this);
    dJointAttach (jointID, base->getMainOdeObject()->getBodyID(), object->getMainOdeObject()->getBodyID());

    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis
    dBodySetFiniteRotationMode(object->getMainOdeObject()->getBodyID(), 1);
    dBodySetFiniteRotationAxis(object->getMainOdeObject()->getBodyID(), 0, 0, 1);

    Vector3d wPosition = object->getPosition();
    dJointSetHingeAnchor (jointID, wPosition.x, wPosition.y, wPosition.z);
    
    Quaternion wRotation = object->getRotation();
    Vector3d rAxis = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHingeAxis (jointID, rAxis.x, rAxis.y, rAxis.z);
    //dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
    //dJointSetHingeAxis (jointID, 0, 1, 0);
    log->__format (LOG_DEVELOPER, "Axis = %f, %f, %f.", rAxis.x, rAxis.y, rAxis.z);
    
    // old kart suspension
    //dJointSetHingeAxis (jointID, rotation.x, rotation.y, rotation.z);
    //dJointSetHingeAxis (jointID, 0,1,0);
}
double Fixed::getRate()
{
    return dJointGetHingeAngleRate (jointID);
}
Vector3d Fixed::getAxis()
{
    dVector3 odeTAxis;
    dJointGetHingeAxis (jointID, odeTAxis);
    return Vector3d (odeTAxis);
}
void Fixed::setVelocity(double velocity)
{
    dJointSetHingeParam(jointID, dParamVel, 0);
}
