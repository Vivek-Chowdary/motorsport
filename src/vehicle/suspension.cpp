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

    // finite rotation on wheels helps avoid explosions, FIXME prolly needs to be relative to suspension axis, and updated every frame
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

DoubleWishbone::DoubleWishbone(WorldObject * container, XmlTag * tag)
    :Suspension(container, "suspension.doublewishbone")
{
    userDriver = false;
    maxSteeringAngle = 0.0;
    if (tag->getName() == "suspension.doublewishbone")
    {
        setName (     tag->getAttribute("name"));
        position = Vector3d (tag->getAttribute("position"));
        rotation = Quaternion (tag->getAttribute("rotation"));
        firstPosition = Vector3d (tag->getAttribute("firstPosition"));
        firstRotation = Quaternion (tag->getAttribute("firstRotation"));
        maxSteeringAngle = stod(tag->getAttribute("steeringAngle"));
        springStiffness = stod(tag->getAttribute("springStiffness"));
        springLengthAtEase = stod(tag->getAttribute("springLengthAtEase"));
        damperFastBump = stod(tag->getAttribute("damperFastBump"));
        damperFastRebound = stod(tag->getAttribute("damperFastRebound"));
        springStiffness = stod(tag->getAttribute("springStiffness"));
        springStiffness = stod(tag->getAttribute("springStiffness"));
    }
    springOldx =  springLengthAtEase;
    if (firstPosition.y > 0) right = true;
    boneDim = Vector3d(0.1, 0.05, 0.4);
    boneWeight = 2.0;
    //--------------------------------------------
    double dirMult = 1.0;
    if (!right) dirMult *= -1;


    //create upperWishbone body
    BoneOdeData upperBoneData;
    upperBoneData.radius = 0.05;
    upperBoneData.length = upperBoneLength = 0.3;
    upperBoneData.mass = 2.0;
    odeObjects["upperBone"] = new OdeObject(this, upperBoneData, "upperBone");
    dBodySetData (odeObjects["upperBone"]->getBodyID(), (void*) container);
    Quaternion upperBoneRot (90, 0, 0);
    odeObjects["upperBone"]->setRotation(upperBoneRot);
    Vector3d upperBonePos (firstPosition.x, firstPosition.y+(dirMult*upperBoneLength*0.5) , firstPosition.z+(boneDim.z*0.5));
    odeObjects["upperBone"]->setPosition(upperBonePos);

    //create lowerWishbone body
    BoneOdeData lowerBoneData;
    lowerBoneData.radius = 0.05;
    lowerBoneData.length = lowerBoneLength = 0.3;
    lowerBoneData.mass = 2.0;
    odeObjects["lowerBone"] = new OdeObject(this, lowerBoneData, "lowerBone");
    dBodySetData (odeObjects["lowerBone"]->getBodyID(), (void*) container);
    Quaternion lowerBoneRot (90, 0, 0);
    odeObjects["lowerBone"]->setRotation(lowerBoneRot);
    Vector3d lowerBonePos (firstPosition.x, firstPosition.y+(dirMult*lowerBoneLength*0.5) , firstPosition.z-(boneDim.z*0.5));
    odeObjects["lowerBone"]->setPosition(lowerBonePos);
    

    dMass m;
    //create bone body
    boneBody = dBodyCreate( World::getWorldPointer()->worldID );
    dBodySetData (boneBody, (void*) container);
    dMassSetBox(&m, 1, boneDim.x, boneDim.y, boneDim.z);
    dMassAdjust(&m, boneWeight);
    dBodySetMass(boneBody, &m);
    dBodySetPosition(boneBody, firstPosition.x, firstPosition.y+(dirMult*upperBoneData.length), firstPosition.z);

    //create visual debug aids
    Ogre::BillboardSet * bset = SystemData::getSystemDataPointer()->ogreSceneManager->createBillboardSet(std::string("lightbbs") + getId(), 3);
    upperB = bset->createBillboard(1,1,1,Ogre::ColourValue(0.5,0.3,0.0f));
    lowerB = bset->createBillboard(1,0,1,Ogre::ColourValue(0.5,0.8,1.0f));
    bB = bset->createBillboard(1,-1.3,1,Ogre::ColourValue(0.5,0.3,1.0f));
    SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->attachObject(bset);
    upperB->setDimensions(0.3,0.05);
    lowerB->setDimensions(0.3,0.05);
    bB->setDimensions(0.05,0.4);

    //create upper joint
    upperJoint = dJointCreateHinge( World::getWorldPointer()->worldID, 0 );
    dJointAttach ( upperJoint, boneBody, odeObjects["upperBone"]->getBodyID() );
    dJointSetHingeAnchor( upperJoint , firstPosition.x , firstPosition.y+(dirMult*upperBoneData.length), firstPosition.z+(boneDim.z*0.5) );
    dJointSetHingeAxis( upperJoint , 1.0, 0.0, 0.0 );
    //limit its rotation
    dJointSetHingeParam ( upperJoint, dParamLoStop, -2.0 );
    dJointSetHingeParam ( upperJoint, dParamHiStop, 2.0 );

    //create lower joint
    lowerJoint = dJointCreateHinge( World::getWorldPointer()->worldID, 0 );
    dJointAttach ( lowerJoint, boneBody, odeObjects["lowerBone"]->getBodyID() );
    dJointSetHingeAnchor( lowerJoint , firstPosition.x , firstPosition.y+(dirMult*upperBoneData.length), firstPosition.z-(boneDim.z*0.5) );
    dJointSetHingeAxis( lowerJoint , 1.0, 0.0, 0.0 );
    //limit its rotation
    dJointSetHingeParam ( lowerJoint, dParamLoStop, -2.0 );
    dJointSetHingeParam ( lowerJoint, dParamHiStop, 2.0 );

    //create bone geom
    boneGeom = dCreateBox(World::getWorldPointer()->spaceID, boneDim.x, boneDim.y, boneDim.z-0.05);
    dGeomSetBody(boneGeom , boneBody );
}
DoubleWishbone::~DoubleWishbone()
{
    //empty
}

void DoubleWishbone::attach(WorldObject * base, WorldObject * object)
{
    Wheel * wheel = dynamic_cast<Wheel*>(object);
    if (wheel == NULL) log->__format(LOG_ERROR, "Trying to attach a non-wheel object to the suspension!");
    if (base->getMainOdeObject() == NULL) log->__format(LOG_ERROR, "Trying to attach a wheel object to an object with no physics!");
    wheel->setSusp(this);

    double dirMult = 1.0;
    if (!right) dirMult *= -1;

    //create chassisUpper joint
    Vector3d pos = getFirstLinkPosition();
    chassisUpperJoint = dJointCreateHinge( World::getWorldPointer()->worldID, 0 );
    dJointAttach( chassisUpperJoint, base->getMainOdeObject()->getBodyID() , odeObjects["upperBone"]->getBodyID() );
    dJointSetHingeAxis( chassisUpperJoint, 1,0,0 );
    dJointSetHingeAnchor( chassisUpperJoint, firstPosition.x, firstPosition.y, firstPosition.z+(boneDim.z*0.5));

    //create chassisLower joint
    chassisLowerJoint = dJointCreateHinge( World::getWorldPointer()->worldID, 0 );
    dJointAttach( chassisLowerJoint, base->getMainOdeObject()->getBodyID() , odeObjects["lowerBone"]->getBodyID() );
    dJointSetHingeAxis( chassisLowerJoint, 1,0,0 );
    dJointSetHingeAnchor( chassisLowerJoint, firstPosition.x, firstPosition.y, firstPosition.z-(boneDim.z*0.5));

    //create axis&steering joint
    axisJoint = dJointCreateHinge2( World::getWorldPointer()->worldID, 0 );
    dJointAttach ( axisJoint, boneBody, object->getMainOdeObject()->getBodyID() );
    dJointSetHinge2Anchor( axisJoint, firstPosition.x, firstPosition.y+(upperBoneLength*dirMult), firstPosition.z);

    Quaternion wRotation = getSecondLinkRotation();
    Vector3d rAxis1 = wRotation.rotateObject(Vector3d(0, 1, 0));
    dJointSetHinge2Axis1 (axisJoint, rAxis1.x, rAxis1.y, rAxis1.z);
    Vector3d rAxis2 = wRotation.rotateObject(Vector3d(0, 0, 1));
    dJointSetHinge2Axis2 (axisJoint, rAxis2.x, rAxis2.y, rAxis2.z);
    log->__format (LOG_DEVELOPER, "Axis2 = %f, %f, %f.", rAxis2.x, rAxis2.y, rAxis2.z);
}
void DoubleWishbone::stepPhysics()
{
    //compute steering
    double angle = getSteeringAngle();
    //if (right) angle *= -1;
    dJointSetHinge2Param (axisJoint, dParamLoStop, angle-0.0000001);
    dJointSetHinge2Param (axisJoint, dParamHiStop, angle+0.0000001);

    //compute suspension
    double timeStep = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
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
    int i;
    for (i=0;i<3;++i)
        forceDirection[i] = f*(chassisHingePos[i]-boneHingePos[i])/x;

    dBodyAddForceAtPos(odeObjects["upperBone"]->getBodyID(), forceDirection[0], forceDirection[1], forceDirection[2],
            chassisHingePos[0], chassisHingePos[1], chassisHingePos[2]);
    dBodyAddForceAtPos(odeObjects["lowerBone"]->getBodyID(), -forceDirection[0], -forceDirection[1], -forceDirection[2],
            boneHingePos[0], boneHingePos[1], boneHingePos[2]);

    springOldx = x;
    
    const dReal * pos;
    pos = dBodyGetPosition(odeObjects["upperBone"]->getBodyID());
    upperB->setPosition(pos[0],pos[1],pos[2]);
    pos = dBodyGetPosition(odeObjects["lowerBone"]->getBodyID());
    lowerB->setPosition(pos[0],pos[1],pos[2]);
    pos = dBodyGetPosition(boneBody);
    bB->setPosition(pos[0],pos[1],pos[2]);
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
