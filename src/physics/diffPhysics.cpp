/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "diff.hpp"
#include "world.hpp"
#include "system.hpp"
#include "ode.h"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Diff::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    torqueTransfer = 0.0;
    revTorqueTransfer = 0.0;
    angularVel = 0.0;
    revAngularVel = 0.0;
    inertia = 0.1;
    friction = 0.001;
    
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
            if (attribute == "diffFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the differential friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            if (attribute == "diffInertia")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the differential inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "finalDriveRatio")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the final drive ratio: %s", attribute.c_str() );
                finalDriveRatio = stod (attribute);
            }
            attribute.clear();
        }
    }
    // create ODE rigid body
    bodyID = dBodyCreate (World::getWorldPointer ()->worldID);

    // initialize inertia
    dMass tmpMass;
    dMassSetParameters (&tmpMass, 0, 0, 0, 0, inertia, 1, 1, 0, 0, 0);
    dBodySetMass (bodyID, &tmpMass);

    // initialize position
    dBodySetPosition (bodyID, 0, 0, 0);

    // initialize rotation
    dMatrix3 rot;
    dRFromEulerAngles (rot, 0, 0, 0);
    dBodySetRotation (bodyID, rot);

    // set body for high speed rotation on x-axis 
    dBodySetFiniteRotationMode (bodyID, 1);
    dBodySetFiniteRotationAxis (bodyID, 1, 0, 0);
}

void Diff::stopPhysics ()
{
    dBodyDestroy (bodyID);
}

void Diff::stepPhysics ()
{
    double dtoverJe;

    prevAngularVel = angularVel;

    dtoverJe=(SystemData::getSystemDataPointer()->physicsTimeStep/1000.0)/inertia;

    torqueTransfer = inputJoint->getTorque();
    revTorqueTransfer = (outputJoint->getRevTorque() + outputJoint2->getRevTorque())/finalDriveRatio;

    angularVel = (dtoverJe*(torqueTransfer-revTorqueTransfer)+prevAngularVel)/(1+(dtoverJe*friction));



    // get current angular velocity
//    const dReal * angularVelVector;
//    angularVelVector = dBodyGetAngularVel (bodyID);
//    angularVel = angularVelVector[1];    

    // calculate angular acceleration (for reference)
    angularAcc = (angularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    revAngularVel = angularVel/finalDriveRatio;

    // subtract friction torque
//    double torque;
//    torque = angularVel*friction;
//    dBodyAddRelTorque (bodyID, 0, 0, inputDrive->getTorque()-torque);

//        torqueTransfer = 0.001*(inputDrive->getAngularVel()-outputDrive->getAngularVel());
//        revTorqueTransfer = 0.001*(inputDrive->getAngularVel()-outputDrive->getAngularVel());
//        angularVel = outputDrive->getAngularVel();
    log->format(LOG_TRACE, "angVel=%f angAcc=%f torque=%f", angularVel, angularAcc, torqueTransfer);
}

double Diff::getAngularVel()
{
    return angularVel;
}
