/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
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
#include <cmath>

#define USE_PRISMATIC false

Suspension::Suspension (std::string name)
    : WorldObject(name)
{
    position = Vector3d (0, 0, 0);
    rotation = Quaternion (1, 0, 0, 0);
}

Suspension::~Suspension ()
{
}

void Suspension::setUserDriver ()
{
    log->__format(LOG_ENDUSER, "Suspension %s will now be driven by a human.", getName().c_str());
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
Vector3d Suspension::getPosition ()
{
    return this->position;
}
void Suspension::setPosition (Vector3d position)
{
    WorldObject::setPosition(position);
    this->position = position;
}
double Unidimensional::getSteeringAngle()
{
    double angle = 0;
    double leftSteering = 0;
    if ( userDriver )
    {
        leftSteering = System::get()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (leftSteering < 0.5)
        {
            leftSteering = 0.5 - leftSteering;
            leftSteering *= 2.0;
        } else {
            leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_1)]->getValue() * 5 / 5;
            if (leftSteering == 0) {
                leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_2)]->getValue() * 4 / 5;
                if (leftSteering == 0) {
                    leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_3)]->getValue() * 3 / 5;
                    if (leftSteering == 0) {
                        leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_4)]->getValue() * 2 / 5;
                        if (leftSteering == 0) {
                            leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_5)]->getValue() * 1 / 5;
                            if (leftSteering == 0) {
                                leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_LEFT)]->getValue() * 5 / 5;
        }   }   }   }   }   }
    }
    double rightSteering = 0;
    if ( userDriver )
    {
        rightSteering = System::get()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (rightSteering > 0.5)
        {
            rightSteering = rightSteering - 0.5;
            rightSteering *= 2.0;
        } else {
            rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_0)]->getValue() * 5 / 5;
            if (rightSteering == 0) {
                rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_9)]->getValue() * 4 / 5;
                if (rightSteering == 0) {
                    rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_8)]->getValue() * 3 / 5;
                    if (rightSteering == 0) {
                        rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_7)]->getValue() * 2 / 5;
                        if (rightSteering == 0) {
                            rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_6)]->getValue() * 1 / 5;
                            if (rightSteering == 0) {
                                rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_RIGHT)]->getValue() * 5 / 5;
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

pUnidimensional Unidimensional::create (XmlTag * tag)
{
    pUnidimensional tmp(new Unidimensional(tag));
    return tmp;
}
void Unidimensional::readCustomDataTag(XmlTag * tag)
{
    springConstant = 0;
    dampingConstant = 0;
    maxSteeringAngle = 0.0;
    position = Vector3d (tag->getAttribute("position"));
    rotation = Quaternion (tag->getAttribute("rotation"));
    springConstant = stod(tag->getAttribute("springConstant"));
    dampingConstant = stod(tag->getAttribute("dampingConstant"));
    maxSteeringAngle = stod(tag->getAttribute("steeringAngle"));
//    jointID = dJointCreateHinge (World::get()->worldID, 0);
    jointID = dJointCreateHinge2 (World::get()->worldID, 0);
    dJointAttach (jointID, 0, 0);
}
Unidimensional::Unidimensional (XmlTag * tag)
    :Suspension("suspension.unidimensional")
{
    constructFromTag(tag);
}
void Unidimensional::stepPhysics()
{
    WorldObject::stepPhysics();
    double angle = getSteeringAngle();
    // Set wheel steering limits. one needs to be done before the other, can't recall which one, so it's dupped
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);
    dJointSetHinge2Param (jointID, dParamLoStop, angle-0.0000001);
    dJointSetHinge2Param (jointID, dParamHiStop, angle+0.0000001);

    // Re-set suspension settings... if physics rate has changed, this needs to be updated.
    double h = System::get()->getDesiredPhysicsTimestep();
    dJointSetHinge2Param (jointID, dParamSuspensionERP, h * springConstant / (h * springConstant + dampingConstant));
    dJointSetHinge2Param (jointID, dParamSuspensionCFM, 1 / (h * springConstant + dampingConstant));
}
Unidimensional::~Unidimensional()
{
    dJointDestroy (jointID);
}
void Unidimensional::attach(pWorldObject base, pWorldObject object)
{
    //FIXME: remove all unnecessary comments
    //Wheel * wheel = dynamic_cast<Wheel*>(object);
    pWheel wheel = boost::dynamic_pointer_cast<Wheel>(object);
    //pWheel wheel ( boost::dynamic_pointer_cast<pWheel>(object) );
    //pWheel wheel = boost::dynamic_pointer_cast<pWheel>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(boost::dynamic_pointer_cast<Suspension>(shared_from_this()));
    //wheel->setSusp(this);
    dJointAttach (jointID, base->getMainOdeObject()->getBodyID(), object->getMainOdeObject()->getBodyID());

    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis, and updated every frame
    dBodySetFiniteRotationMode(object->getMainOdeObject()->getBodyID(), 1);
    dBodySetFiniteRotationAxis(object->getMainOdeObject()->getBodyID(), 0, 0, 1);

    double h = System::get()->getDesiredPhysicsTimestep();
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
}


pFixed Fixed::create(XmlTag * tag)
{
    pFixed tmp(new Fixed(tag));
    return tmp;
}

void Fixed::readCustomDataTag(XmlTag * tag)
{
    position = Vector3d (tag->getAttribute("position"));
    rotation = Quaternion (tag->getAttribute("rotation"));
    jointID = dJointCreateHinge (World::get()->worldID, 0);
    dJointAttach (jointID, 0, 0);
}
Fixed::Fixed (XmlTag * tag)
    :Suspension("suspension.fixed")
{
    constructFromTag(tag);
}
Fixed::~Fixed()
{
    dJointDestroy (jointID);
}
void Fixed::attach(pWorldObject base, pWorldObject object)
{
    //Wheel * wheel = dynamic_cast<Wheel*>(object.get());
    pWheel wheel = boost::dynamic_pointer_cast<Wheel>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(boost::dynamic_pointer_cast<Suspension>(shared_from_this()));
    dJointAttach (jointID, base->getMainOdeObject()->getBodyID(), object->getMainOdeObject()->getBodyID());

    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis
    dBodySetFiniteRotationMode(object->getMainOdeObject()->getBodyID(), 1);
    dBodySetFiniteRotationAxis(object->getMainOdeObject()->getBodyID(), 0, 0, 1);

    Vector3d wPosition = object->getPosition();
    dJointSetHingeAnchor (jointID, wPosition.x, wPosition.y, wPosition.z);
    
    Quaternion wRotation = object->getRotation();
    Vector3d rAxis = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHingeAxis (jointID, rAxis.x, rAxis.y, rAxis.z);
    log->__format (LOG_DEVELOPER, "Axis = %f, %f, %f.", rAxis.x, rAxis.y, rAxis.z);
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

pDoubleWishbone DoubleWishbone::create(XmlTag * tag)
{
    pDoubleWishbone tmp(new DoubleWishbone(tag));
    return tmp;
}
void DoubleWishbone::readCustomDataTag(XmlTag * tag)
{
    pOgreObjectData upperBoneOData(new OgreObjectData);
    pOgreObjectData lowerBoneOData(new OgreObjectData);
    pOgreObjectData uprightBoneOData(new OgreObjectData);
    maxSteeringAngle = 0.0;
    position = Vector3d (tag->getAttribute("position"));
    rotation = Quaternion (tag->getAttribute("rotation"));
    firstPosition = Vector3d (tag->getAttribute("firstPosition"));
    firstRotation = Quaternion (tag->getAttribute("firstRotation"));
    maxSteeringAngle = stod(tag->getAttribute("steeringAngle"));
    springStiffness = stod(tag->getAttribute("springStiffness"));
    springLengthAtEase = stod(tag->getAttribute("springLengthAtEase"));
    damperFastBump = stod(tag->getAttribute("damperFastBump"));
    damperFastRebound = stod(tag->getAttribute("damperFastRebound"));
    upperBoneOData->meshPath = tag->getAttribute("upperBoneMesh");
    lowerBoneOData->meshPath = tag->getAttribute("lowerBoneMesh");
    uprightBoneOData->meshPath = tag->getAttribute("uprightBoneMesh");
    springOldx =  springLengthAtEase;
    right = firstPosition.y > 0;
    uprightBoneLength = 0.4;
    //--------------------------------------------
    double dirMult = 1.0;
    if (!right) dirMult *= -1;

    //create upperWishbone body
    pBoneOdeData upperBoneData(new BoneOdeData);
    upperBoneData->radius = 0.05;
    upperBoneData->length = upperBoneLength = 0.3;
    upperBoneData->mass = 2.0;
    odeObjects["upperBone"] = pOdeObject(new OdeObject(this, upperBoneData, "upperBone"));
    Quaternion upperBoneRot (90, 0, 0);
    odeObjects["upperBone"]->setRotation(upperBoneRot);
    Vector3d upperBonePos (firstPosition.x, firstPosition.y+(dirMult*upperBoneLength*0.5) , firstPosition.z+(uprightBoneLength*0.5));
    odeObjects["upperBone"]->setPosition(upperBonePos);
    //and its graphics
    upperBoneOData->meshPath = getPath() + upperBoneOData->meshPath;
    pOgreObject upperOgreObject (new OgreObject(this, upperBoneOData, getId()));
    ogreObjects["upperBone"] = upperOgreObject;
    ogreObjects["upperBone"]->setOdeReference(odeObjects["upperBone"]);

    //create lowerWishbone body
    pBoneOdeData lowerBoneData(new BoneOdeData);
    lowerBoneData->radius = 0.05;
    lowerBoneData->length = lowerBoneLength = 0.3;
    lowerBoneData->mass = 2.0;
    odeObjects["lowerBone"] = pOdeObject(new OdeObject(this, lowerBoneData, "lowerBone"));
    Quaternion lowerBoneRot (90, 0, 0);
    odeObjects["lowerBone"]->setRotation(lowerBoneRot);
    Vector3d lowerBonePos (firstPosition.x, firstPosition.y+(dirMult*lowerBoneLength*0.5) , firstPosition.z-(uprightBoneLength*0.5));
    odeObjects["lowerBone"]->setPosition(lowerBonePos);
    //and its graphics
    lowerBoneOData->meshPath = getPath() + lowerBoneOData->meshPath;
    pOgreObject lowerOgreObject (new OgreObject(this, lowerBoneOData, getId()));
    ogreObjects["lowerBone"] = lowerOgreObject;
    ogreObjects["lowerBone"]->setOdeReference(odeObjects["lowerBone"]);

    //create upright bone body
    pBoneOdeData uprightBoneData(new BoneOdeData);
    uprightBoneData->radius = 0.05;
    uprightBoneData->length = uprightBoneLength;
    uprightBoneData->mass = 2.0;
    odeObjects["uprightBone"] = pOdeObject(new OdeObject(this, uprightBoneData, "uprightBone"));
    Quaternion uprightBoneRot (0, 0, 0);
    odeObjects["uprightBone"]->setRotation(uprightBoneRot);
    Vector3d uprightBonePos (firstPosition.x, firstPosition.y+(dirMult*upperBoneData->length), firstPosition.z);
    odeObjects["uprightBone"]->setPosition(uprightBonePos);
    //and its graphics
    uprightBoneOData->meshPath = getPath() + uprightBoneOData->meshPath;
    pOgreObject uprightOgreObject (new OgreObject(this, uprightBoneOData, getId()));
    ogreObjects["uprightBone"] = uprightOgreObject;
    ogreObjects["uprightBone"]->setOdeReference(odeObjects["uprightBone"]);

    //create upper joint
    upperJoint = dJointCreateHinge( World::get()->worldID, 0 );
    dJointAttach ( upperJoint, odeObjects["uprightBone"]->getBodyID(), odeObjects["upperBone"]->getBodyID() );
    dJointSetHingeAnchor( upperJoint , firstPosition.x , firstPosition.y+(dirMult*upperBoneData->length), firstPosition.z+(uprightBoneLength*0.5) );
    dJointSetHingeAxis( upperJoint , 1.0, 0.0, 0.0 );
    //limit its rotation
    dJointSetHingeParam ( upperJoint, dParamLoStop, -2.0 );
    dJointSetHingeParam ( upperJoint, dParamHiStop, 2.0 );

    //create lower joint
    lowerJoint = dJointCreateHinge( World::get()->worldID, 0 );
    dJointAttach ( lowerJoint, odeObjects["uprightBone"]->getBodyID(), odeObjects["lowerBone"]->getBodyID() );
    dJointSetHingeAnchor( lowerJoint , firstPosition.x , firstPosition.y+(dirMult*upperBoneData->length), firstPosition.z-(uprightBoneLength*0.5) );
    dJointSetHingeAxis( lowerJoint , 1.0, 0.0, 0.0 );
    //limit its rotation
    dJointSetHingeParam ( lowerJoint, dParamLoStop, -2.0 );
    dJointSetHingeParam ( lowerJoint, dParamHiStop, 2.0 );
}

DoubleWishbone::DoubleWishbone(XmlTag * tag)
    :Suspension("suspension.doublewishbone")
{
    constructFromTag(tag);
}
DoubleWishbone::~DoubleWishbone()
{
    //empty
}

void DoubleWishbone::attach(pWorldObject base, pWorldObject object)
{
    //Wheel * wheel = dynamic_cast<Wheel*>(object.get());
    pWheel wheel = boost::dynamic_pointer_cast<Wheel>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(boost::dynamic_pointer_cast<Suspension>(shared_from_this()));

    double dirMult = 1.0;
    if (!right) dirMult *= -1;

    //create chassisUpper joint
    Vector3d pos = getFirstLinkPosition();
    chassisUpperJoint = dJointCreateHinge( World::get()->worldID, 0 );
    dJointAttach( chassisUpperJoint, base->getMainOdeObject()->getBodyID() , odeObjects["upperBone"]->getBodyID() );
    dJointSetHingeAxis( chassisUpperJoint, 1,0,0 );
    dJointSetHingeAnchor( chassisUpperJoint, firstPosition.x, firstPosition.y, firstPosition.z+(uprightBoneLength*0.5));

    //create chassisLower joint
    chassisLowerJoint = dJointCreateHinge( World::get()->worldID, 0 );
    dJointAttach( chassisLowerJoint, base->getMainOdeObject()->getBodyID() , odeObjects["lowerBone"]->getBodyID() );
    dJointSetHingeAxis( chassisLowerJoint, 1,0,0 );
    dJointSetHingeAnchor( chassisLowerJoint, firstPosition.x, firstPosition.y, firstPosition.z-(uprightBoneLength*0.5));

    //create axis&steering joint
    axisJoint = dJointCreateHinge2( World::get()->worldID, 0 );
    dJointAttach ( axisJoint, odeObjects["uprightBone"]->getBodyID(), object->getMainOdeObject()->getBodyID() );
    dJointSetHinge2Anchor( axisJoint, firstPosition.x, firstPosition.y+(upperBoneLength*dirMult), firstPosition.z);

    Quaternion wRotation = getSecondLinkRotation();
    Vector3d rAxis1 = wRotation.rotateObject(Vector3d(0, 1, 0));
    dJointSetHinge2Axis1 (axisJoint, rAxis1.x, rAxis1.y, rAxis1.z);
    Vector3d rAxis2 = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHinge2Axis2 (axisJoint, rAxis2.x, rAxis2.y, rAxis2.z);
    log->__format (LOG_DEVELOPER, "Axis2 = %f, %f, %f.", rAxis2.x, rAxis2.y, rAxis2.z);

    //create spring-damper joint ------------------------------------------
    if (USE_PRISMATIC)
    {
        sdamper = dJointCreateSlider(World::get()->worldID, 0);
        dJointAttach( sdamper, base->getMainOdeObject()->getBodyID(), odeObjects["uprightBone"]->getBodyID() );
        dJointSetSliderAxis (sdamper, 0,dirMult*0.66,0.66);
        //dJointSetSliderAxis (sdamper, forceDirection[0], forceDirection[1], forceDirection[2]);
    }
}
void DoubleWishbone::stepPhysics()
{
    WorldObject::stepPhysics();
    //compute steering
    double angle = getSteeringAngle();
    //if (right) angle *= -1;
    dJointSetHinge2Param (axisJoint, dParamLoStop, angle-0.0000001);
    dJointSetHinge2Param (axisJoint, dParamHiStop, angle+0.0000001);

    //compute suspension
    double timeStep = System::get()->getDesiredPhysicsTimestep();
    dVector3 chassisHingePos,boneHingePos;
    dJointGetHingeAnchor( chassisUpperJoint, chassisHingePos );
    dJointGetHingeAnchor( lowerJoint, boneHingePos );

    dReal x = dDISTANCE( chassisHingePos, boneHingePos );
    dReal v = (x-springOldx)/timeStep;
    dReal f = (springLengthAtEase-x)*springStiffness ;
    //dReal s = 1.f;
    if (v<0.0)	// compression
        f -= v*damperFastBump;
    else
        f -= v*damperFastRebound;

    dVector3 forceDirection;
    for (int i=0;i<3;++i) forceDirection[i] = f*(chassisHingePos[i]-boneHingePos[i])/x;
    if (USE_PRISMATIC)
    {
    dJointSetSliderAxis (sdamper, forceDirection[0], forceDirection[1], forceDirection[2]);
        dJointAddSliderForce (sdamper, f/100.0);
        double h = System::get()->getDesiredPhysicsTimestep();
        dJointSetSliderParam (sdamper, dParamSuspensionERP, h * springStiffness / (h * springStiffness + damperFastBump));
        dJointSetSliderParam (sdamper, dParamSuspensionCFM, 1 / (h * springStiffness + damperFastBump));
    } else {
        dBodyAddForceAtPos(odeObjects["upperBone"]->getBodyID(), forceDirection[0], forceDirection[1], forceDirection[2], chassisHingePos[0], chassisHingePos[1], chassisHingePos[2]);
        dBodyAddForceAtPos(odeObjects["lowerBone"]->getBodyID(), -forceDirection[0], -forceDirection[1], -forceDirection[2], boneHingePos[0], boneHingePos[1], boneHingePos[2]);
    }

    springOldx = x;
}
double DoubleWishbone::getRate()
{
    return dJointGetHinge2Angle2Rate (axisJoint);
}
Vector3d DoubleWishbone::getAxis()
{
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (axisJoint, odeTAxis);
    return Vector3d (odeTAxis);
}
void DoubleWishbone::setVelocity(double velocity)
{
    dJointSetHinge2Param(axisJoint, dParamVel, 0);
}
double DoubleWishbone::getSteeringAngle()
{
    double angle = 0;
    double leftSteering = 0;
    if ( userDriver )
    {
        leftSteering = System::get()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (leftSteering < 0.5)
        {
            leftSteering = 0.5 - leftSteering;
            leftSteering *= 2.0;
        } else {
            leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_1)]->getValue() * 5 / 5;
            if (leftSteering == 0) {
                leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_2)]->getValue() * 4 / 5;
                if (leftSteering == 0) {
                    leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_3)]->getValue() * 3 / 5;
                    if (leftSteering == 0) {
                        leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_4)]->getValue() * 2 / 5;
                        if (leftSteering == 0) {
                            leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_5)]->getValue() * 1 / 5;
                            if (leftSteering == 0) {
                                leftSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_LEFT)]->getValue() * 5 / 5;
        }   }   }   }   }   }
    }
    double rightSteering = 0;
    if ( userDriver )
    {
        rightSteering = System::get()->axisMap[getIDJoyAxis(0,0)]->getValue();
        if (rightSteering > 0.5)
        {
            rightSteering = rightSteering - 0.5;
            rightSteering *= 2.0;
        } else {
            rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_0)]->getValue() * 5 / 5;
            if (rightSteering == 0) {
                rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_9)]->getValue() * 4 / 5;
                if (rightSteering == 0) {
                    rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_8)]->getValue() * 3 / 5;
                    if (rightSteering == 0) {
                        rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_7)]->getValue() * 2 / 5;
                        if (rightSteering == 0) {
                            rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_6)]->getValue() * 1 / 5;
                            if (rightSteering == 0) {
                                rightSteering = System::get()->axisMap[getIDKeyboardKey(SDLK_RIGHT)]->getValue() * 5 / 5;
        }   }   }   }   }   }
    }

    // Override keyboard data with joystick axis if the keyboard is not used.
    const double piK = 3.14159265358979323846264338327950288419716939937510 / 180;
    rightSteering *= maxSteeringAngle * piK / 2;
    leftSteering *= maxSteeringAngle * piK / 2;
    angle += rightSteering - leftSteering;
    return angle;
}
