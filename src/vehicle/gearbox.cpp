/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"

Gearbox::Gearbox (WorldObject * container, XmlTag * tag)
    :DriveMass(container, "gearbox")
{
    log->__format (LOG_CCREATOR, "Starting to parse a gearbox node");
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
    
    if (tag->getName() == "gearbox")
    {
        setName (     tag->getAttribute("name"));
        friction = stod (tag->getAttribute("gearboxFriction"));
        inertia = stod (tag->getAttribute("gearboxInertia"));
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "gear")
            {
                GearboxGear * tmp = new GearboxGear (this, t);
                gearMap[tmp->getNumber()] = tmp;
            }
        }
    }
}

Gearbox::~Gearbox ()
{
}


GearboxGear::GearboxGear (WorldObject * container, XmlTag * tag)
    :WorldObject(container, "gearboxGear")
{
    log->__format (LOG_CCREATOR, "Starting to parse a gearbox gear node");
    ratio = 1.0;
    number = 0;
    if (tag->getName() == "gear")
    {
        setName (     tag->getAttribute("name"));
        number = stoi (tag->getAttribute("number"));
        ratio = stod (tag->getAttribute("ratio"));
    }
}

GearboxGear::~GearboxGear()
{
}
void Gearbox::stepPhysics ()
{
    double dt;
    double torqueSum;
    
    gearRatio = gearMap[currentGear]->ratio;
    
    dt = SystemData::getSystemDataPointer()->getDesiredPhysicsTimestep();
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
    
    log->__format(LOG_DEVELOPER, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer, inputAngularVel, outputAngularVel);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

void Gearbox::gearUp() {
    currentGear++;

    int maxGear = gearMap.size() - 1;
    if(currentGear > maxGear) currentGear = maxGear;

    gearRatio = gearMap[currentGear]->ratio;
    log->__format(LOG_DEVELOPER, "Gear set to %s", gearMap[currentGear]->getName().c_str()); 
}

void Gearbox::gearDown() {
    currentGear--;
    if(currentGear < 0) currentGear = 0;

    gearRatio = gearMap[currentGear]->ratio;
    log->__format(LOG_DEVELOPER, "Gear set to %s", gearMap[currentGear]->getName().c_str()); 
}
