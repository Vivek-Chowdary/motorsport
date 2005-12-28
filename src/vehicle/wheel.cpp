/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "world.hpp"
#include "ode/ode.h"
#include "SDL/SDL_keysym.h"
#include "pedal.hpp"
#include "vehicle.hpp"

Wheel::Wheel (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n)
    :DriveMass(container, name)
{
    log->__format (LOG_DEVELOPER, "Starting to parse a wheel node");
    processXmlRootNode (n);
}

Wheel::~Wheel ()
{
}

void Wheel::setBrakePedal (Pedal * pedal)
{
    brakePedal = pedal;
}

void Wheel::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "name")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_DEVELOPER, "Found the name: %s", attribute.c_str());
                setName(attribute);
            }
        }
    }
    startPhysics (n);
    startGraphics (n);
    ogreObjects.begin()->second->setOdeReference(getMainOdeObject());
}

void Wheel::setUserDriver ()
{
    userDriver = true;
}
void Wheel::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    OgreObjectData data;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "mesh")
            {
                assignXmlString (data.meshPath, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel graphics mesh filename: %s", data.meshPath.c_str());
            }
        }
    }
    data.meshPath = getPath() + data.meshPath;
    OgreObject * ogreObject = new OgreObject(this, data, getId());
    ogreObjects[getId()] = ogreObject;
}

void Wheel::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    VehicleWheelOdeObjectData data;
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
                log->__format (LOG_CCREATOR, "Found the wheel physics radius: %s", attribute.c_str() );
                data.radius = stod (attribute);
            }
            if (attribute == "width")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel physics width: %s", attribute.c_str() );
                data.width = stod (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel physics mass: %s", attribute.c_str() );
                data.mass = stod (attribute);
            }
            if (attribute == "powered")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel power transmission: %s", attribute.c_str() );
                powered = stod (attribute);
            }
        }
    }
    odeObjects[getId()] = new OdeObject(this, data, getId());
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

    // first, get the axis of the suspension
    dVector3 odeTAxis;
    dJointGetHinge2Axis2 (suspJointID, odeTAxis);
    Vector3d tAxis (odeTAxis);

    // accumulate torques:
    double brake = brakePedal->getNormalizedAngle();
    if (!userDriver) brake = 1;
    if (inputAngularVel > 0) brake *= -1;
    double maxBrakeTorque = 1250;
    double brakeTorque = brake * maxBrakeTorque;
    int initialSign = inputTorqueTransfer>0?1:-1;
    inputTorqueTransfer += brakeTorque;
    int finalSign = inputTorqueTransfer>0?1:-1;
    if (initialSign != finalSign)
    {
        inputTorqueTransfer = 0;
    }

    // then, scale it by desired torque in the desired direction of the axis
    tAxis.scalarMultiply (inputTorqueTransfer * powered);


    ///TODO TODO TODO TODO TODO
    // use feedback thingy in ODE in order to find out torques
    ///////////////////////////////////////////////////////////////////////
    // get accumulated torque
    const dReal * odeTorque = dBodyGetTorque (baseID);
    Vector3d accumulatedTorque (odeTorque);
    // show acc torque
    log->__format(LOG_WARNING, "Accumulated torque = (%f, %f, %f)", accumulatedTorque.x, accumulatedTorque.y, accumulatedTorque.z);
    // compare it to our torque
    ///////////////////////////////////////////////////////////////////////
    
    // finally, apply the torques
    dBodyAddTorque (getMainOdeObject()->getBodyID(), tAxis.x, tAxis.y, tAxis.z);

    log->__format(LOG_DEVELOPER, "%s:angVel=%f angAcc=%f torque=%f powered=%f axis=(%f,%f,%f)",getName().c_str(), inputAngularVel, angularAcc, inputTorqueTransfer, powered, tAxis.x, tAxis.y, tAxis.z);
    
    // zero the accumulators
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Wheel::getInputAngularVel()
{
    return inputAngularVel;
}
