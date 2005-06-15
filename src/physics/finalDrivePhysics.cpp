/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "finalDrive.hpp"
#include "world.hpp"
#include "system.hpp"
#include "ode/ode.h"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void FinalDrive::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    prevAngularVel = 0.0;
    angularAcc = 0.0;
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
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the differential friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            if (attribute == "diffInertia")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the differential inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "finalDriveRatio")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the final drive ratio: %s", attribute.c_str() );
                finalDriveRatio = stod (attribute);
            }
        }
    }
}

void FinalDrive::stopPhysics ()
{
}

void FinalDrive::stepPhysics ()
{
    double dt;
    double torqueSum;
    
    dt = SystemData::getSystemDataPointer()->physicsTimeStep;

    prevAngularVel = inputAngularVel;

//    inputTorqueTransfer += inputJoint->getOutputTorque();
//    outputTorqueTransfer += outputJoint->getInputTorque()/finalDriveRatio;

    torqueSum = inputTorqueTransfer + outputTorqueTransfer/finalDriveRatio;
    
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);

    outputAngularVel = inputAngularVel/finalDriveRatio;

/*    double dtoverJe;

    prevAngularVel = inputAngularVel;

    dtoverJe=(SystemData::getSystemDataPointer()->physicsTimeStep)/inertia;

    inputTorqueTransfer = inputJoint->getOutputTorque();
    outputTorqueTransfer = (outputJoint->getInputTorque() + outputJoint2->getInputTorque())/finalDriveRatio;

    inputAngularVel = (dtoverJe*(outputTorqueTransfer+inputTorqueTransfer)+prevAngularVel)/(1+(dtoverJe*friction));

    // calculate angular acceleration (for reference)
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep;
    outputAngularVel = inputAngularVel/finalDriveRatio;
*/
    log->format(LOG_DEVELOPER, "angVel=%f angAcc=%f torque=%f", inputAngularVel, angularAcc, outputTorqueTransfer);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}
