/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef SUSPENSION_HPP
#define SUSPENSION_HPP
#include "ode/objects.h"
#include "worldObject.hpp"
#include "quaternion.hpp"
#include "vector3d.hpp"
#include "location.hpp"
#include "vehicle.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Suspension : public WorldObject
{
    protected:
        Suspension (WorldObject * container, std::string name);
        ~Suspension ();
        Quaternion rotation;
        Vector3d position;
        bool userDriver;
    public:
        void setUserDriver ();
        Vector3d getSecondLinkPosition ();
        Quaternion getSecondLinkRotation ();

        virtual void stepPhysics ()=0;
        virtual double getRate()=0;
        virtual Vector3d getAxis()=0;
        virtual void setVelocity(double velocity)=0;
        virtual void attach(WorldObject * base, WorldObject * object)=0;
};

class Unidimensional : public Suspension
{
    protected:
        dJointID jointID;
        double springConstant;
        double dampingConstant;
        double maxSteeringAngle;
        double getSteeringAngle();
    public:
        Unidimensional (WorldObject * container, XmlTag * tag);
        ~Unidimensional();

        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(WorldObject * base, WorldObject * object);
};
class Fixed : public Suspension
{
    protected:
        dJointID jointID;
    public:
        Fixed (WorldObject * container, XmlTag * tag);
        ~Fixed();

        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(WorldObject * base, WorldObject * object);
};
class DoubleWishbone : public Suspension
{
    protected:
        Quaternion firstRotation;
        Vector3d firstPosition;
        Vector3d getFirstLinkPosition (){return firstPosition;};
        Quaternion getFirstLinkRotation (){return firstRotation;};
        double maxSteeringAngle;
        double getSteeringAngle();

        bool right;
        dBodyID lowerWishBoneBody;
        dBodyID boneBody;
        Ogre::Billboard * upperB;
        Ogre::Billboard * lowerB;
        Ogre::Billboard *  bB;

        dGeomID lowerWishBoneGeom;
        dGeomID boneGeom;
        dJointID upperJoint;
        dJointID lowerJoint;
        dJointID axisJoint;
        dJointID chassisUpperJoint;        
        dJointID chassisLowerJoint;        

        double uprightLength;
        double upperBoneLength;
        Vector3d lowerDim;
        Vector3d boneDim;
        double lowerWeight;
        double boneWeight;

        double springOldx;
        double springStiffness;
        double springLengthAtEase;
        double damperFastBump;
        double damperFastRebound;

    public:
        DoubleWishbone (WorldObject * container, XmlTag * tag);
        ~DoubleWishbone();
        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(WorldObject * base, WorldObject * object);
};
#endif

