
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
    gearRatio = 0.0;
    currentGear = 0;
    
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
    
    gearRatio = gearMap[currentGear]->ratio;
    
    dt = SystemData::getSystemDataPointer()->physicsTimeStep/1000.0;
    prevAngularVel = inputAngularVel;

    if(gearRatio>0.0 || gearRatio <0.0) {
        torqueSum = inputTorqueTransfer + outputTorqueTransfer/gearRatio;
    }
    else {
        torqueSum = inputTorqueTransfer;
    }
    
    angularAcc = (torqueSum - friction * prevAngularVel)/inertia;
    
    // improved Euler ODE solve
    inputAngularVel = prevAngularVel + dt / 2 * (angularAcc + (torqueSum - friction*(prevAngularVel + angularAcc*dt))/inertia);

    if(gearRatio>0.0 || gearRatio < 0.0) {
        outputAngularVel = inputAngularVel/gearRatio;
    }
    else {
        double outputAngularAcc;
        outputAngularAcc = (outputTorqueTransfer - 0.1 * outputAngularVel)/0.1;
        outputAngularVel = outputAngularVel + dt / 2 * (outputAngularAcc + (outputTorqueTransfer - 0.1*(outputAngularVel+outputAngularAcc*dt))/0.1);
    }
    
    log->format(LOG_TRACE, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer, inputAngularVel, outputAngularVel);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

void Gearbox::gearUp() {
    if(currentGear >= 6) {
        currentGear = 6;
    }
    else {
        currentGear++;
    }
    gearRatio = gearMap[currentGear]->ratio;
    log->format(LOG_TRACE, "Gear set to %s", gearMap[currentGear]->label.c_str()); 
}

void Gearbox::gearDown() {
    if(currentGear <=0) {
        currentGear = 0;
    }
    else {
        currentGear--;
    }
    gearRatio = gearMap[currentGear]->ratio;
    log->format(LOG_TRACE, "Gear set to %s", gearMap[currentGear]->label.c_str()); 
}
