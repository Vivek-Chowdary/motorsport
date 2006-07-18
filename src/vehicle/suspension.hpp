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
        Suspension (std::string name);
        ~Suspension ();
        Quaternion rotation;
        Vector3d position;
        bool userDriver;
    public:
        void setUserDriver ();
        Vector3d getSecondLinkPosition ();
        Quaternion getSecondLinkRotation ();
        void setPosition (Vector3d position);
        Vector3d getPosition();

        virtual void stepPhysics ()=0;
        virtual double getRate()=0;
        virtual Vector3d getAxis()=0;
        virtual void setVelocity(double velocity)=0;
        virtual void attach(pWorldObject base, pWorldObject object)=0;
};

SHARED_PTR(Unidimensional, pUnidimensional, wUnidimensional);
class Unidimensional : public Suspension
{
    protected:
        dJointID jointID;
        double springConstant;
        double dampingConstant;
        double maxSteeringAngle;
        double getSteeringAngle();
        Unidimensional (XmlTag * tag);
    public:
        static pUnidimensional create(XmlTag * tag);
        ~Unidimensional();

        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(pWorldObject base, pWorldObject object);
};

SHARED_PTR(Fixed, pFixed, wFixed);
class Fixed : public Suspension
{
    protected:
        dJointID jointID;
        Fixed (XmlTag * tag);
    public:
        static pFixed create(XmlTag * tag);
        ~Fixed();

        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(pWorldObject base, pWorldObject object);
};
SHARED_PTR(DoubleWishbone, pDoubleWishbone, wDoubleWishbone);
class DoubleWishbone : public Suspension
{
    protected:
        Quaternion firstRotation;
        Vector3d firstPosition;
        double maxSteeringAngle;
        double getSteeringAngle();
        bool right;

        //dJointID sdamper;
        dJointID upperJoint;
        dJointID lowerJoint;
        dJointID axisJoint;
        dJointID chassisUpperJoint;        
        dJointID chassisLowerJoint;        

        double uprightBoneLength;
        double upperBoneLength;
        double lowerBoneLength;

        double springOldx;
        double springStiffness;
        double springLengthAtEase;
        double damperFastBump;
        double damperFastRebound;

        Vector3d getFirstLinkPosition (){return firstPosition;};
        Quaternion getFirstLinkRotation (){return firstRotation;};
        DoubleWishbone (XmlTag * tag);
    public:
        static pDoubleWishbone create(XmlTag * tag);
        ~DoubleWishbone();

        void stepPhysics();
        double getRate();
        Vector3d getAxis();
        void setVelocity(double velocity);
        void attach(pWorldObject base, pWorldObject object);
};
#endif

