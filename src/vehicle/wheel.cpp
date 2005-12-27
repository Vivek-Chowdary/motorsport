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
    stopPhysics ();
    stopGraphics ();
}


void Wheel::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (wheelID);  // need to allocate memory first??
    wheelNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Wheel::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (wheelID);    // need to allocate memory first??
    wheelNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

std::string Wheel::getIndex()
{
    return index;
}
void Wheel::setBrakePedal (Pedal * pedal)
{
    brakePedal = pedal;
}

void Wheel::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    index.assign ("Unknown");
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                assignXmlString (index, attNode->getValue());
                log->__format (LOG_DEVELOPER, "Found the index: %s", index.c_str());
            }
        }
    }
    startPhysics (n);
    startGraphics (n);
}

void Wheel::setUserDriver ()
{
    userDriver = true;
}
void Wheel::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    std::string mesh = "None";
    Ogre::SceneDetailLevel renderMode = Ogre::SDL_SOLID;
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
                assignXmlString (mesh, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel graphics mesh filename: %s", mesh.c_str());
            }
            if (attribute == "renderMode")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel rendering mode: %s", attribute.c_str());

                if(attribute == "points")
                    renderMode=Ogre::SDL_POINTS;
                if(attribute == "wireframe")
                    renderMode=Ogre::SDL_WIREFRAME;
                if(attribute == "solid")
                    renderMode=Ogre::SDL_SOLID;
            }
        }
    }
    std::string meshPath = Paths::vehicle(container->getName()) + mesh;
    wheelEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (getId().c_str(), meshPath.c_str());
    wheelEntity->setRenderDetail(renderMode);

    log->__format (LOG_CCREATOR, "Wheel mesh has %i submeshes", wheelEntity->getNumSubEntities());
    for(unsigned int i = 0; i < wheelEntity->getNumSubEntities(); i++)
    {
        log->__format (LOG_CCREATOR, "Wheel submesh %i material: %s", i, wheelEntity->getSubEntity(i)->getMaterialName().c_str() );
    }

    wheelNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    wheelNode->attachObject (wheelEntity);
}
void Wheel::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Wheel::setRenderDetail(int renderMode)
{
    Ogre::SceneDetailLevel mode;
    switch (renderMode)
    {
    case 1:
        mode = Ogre::SDL_POINTS;
        break;
    case 2:
        mode = Ogre::SDL_WIREFRAME;
        break;
    case 3:
    default:
        mode = Ogre::SDL_SOLID;
        break;
    }
    wheelEntity->setRenderDetail(mode);
}
void Wheel::stopGraphics ()
{
    // empty
}
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
                log->__format (LOG_CCREATOR, "Found the wheel physics radius: %s", attribute.c_str() );
                radius = stod (attribute);
            }
            if (attribute == "width")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel physics width: %s", attribute.c_str() );
                width = stod (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "powered")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the wheel power transmission: %s", attribute.c_str() );
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
    const dReal * odeTorque = dBodyGetTorque (wheelID);
    Vector3d accumulatedTorque (odeTorque);
    // show acc torque
    log->__format(LOG_WARNING, "Accumulated torque = (%f, %f, %f)", accumulatedTorque.x, accumulatedTorque.y, accumulatedTorque.z);
    // compare it to our torque
    ///////////////////////////////////////////////////////////////////////
    
    // finally, apply the torques
    dBodyAddTorque (wheelID, tAxis.x, tAxis.y, tAxis.z);

    log->__format(LOG_DEVELOPER, "%s:angVel=%f angAcc=%f torque=%f powered=%f axis=(%f,%f,%f)",index.c_str(), inputAngularVel, angularAcc, inputTorqueTransfer, powered, tAxis.x, tAxis.y, tAxis.z);
    
    // zero the accumulators
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

double Wheel::getInputAngularVel()
{
    return inputAngularVel;
}
