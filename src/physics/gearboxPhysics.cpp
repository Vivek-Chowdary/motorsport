
/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "SDL/SDL_keysym.h"


void Gearbox::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    friction = 0.01;
    inertia = 1.0;
    prevAngularVel = 0.0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    angularAcc = 0.0;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "gearboxFriction")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Friction: %s", attribute.c_str() );
                friction = stod (attribute);
            }
            if (attribute == "gearboxInertia")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gearbox Inertia: %s", attribute.c_str() );
                inertia = stod (attribute);
            }
            if (attribute == "gear1Ratio")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the Gear 1 Ratio: %s", attribute.c_str() );
                gearRatio = stod (attribute);
            }
            attribute.clear();
        }
    }
}

void Gearbox::stopPhysics ()
{
}

void Gearbox::stepPhysics ()
{
    double dt;
    double torqueSum;
    
    dt = SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;

    prevAngularVel = inputAngularVel;

//    inputTorqueTransfer = inputJoint->getOutputTorque();
//    outputTorqueTransfer = outputJoint->getInputTorque()/gearRatio;

    torqueSum = inputTorqueTransfer + outputTorqueTransfer/gearRatio;
    
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);

    outputAngularVel = inputAngularVel/gearRatio;
    
/*    double dtOverJe;
    prevAngularVel = inputAngularVel;

    dtOverJe=(SystemData::getSystemDataPointer()->physicsTimeStep/1000.0)/inertia;

    inputTorqueTransfer = inputJoint->getOutputTorque();
    outputTorqueTransfer = outputJoint->getInputTorque()/gearRatio;

    inputAngularVel = (dtOverJe*(inputTorqueTransfer+outputTorqueTransfer)+prevAngularVel)/(1+(dtOverJe*friction));
    angularAcc = (inputAngularVel-prevAngularVel)/SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    outputAngularVel = inputAngularVel/gearRatio;
*/
    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer, inputAngularVel, outputAngularVel);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

