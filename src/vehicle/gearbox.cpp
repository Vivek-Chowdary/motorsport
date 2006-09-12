/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "world.hpp"
#include "system.hpp"
#include "SDL/SDL_keysym.h"

pGearbox Gearbox::create(XmlTag * tag)
{
    pGearbox tmp(new Gearbox(tag));
    return tmp;
}

Gearbox::Gearbox (XmlTag * tag)
    :DriveMass("gearbox")
{
    constructFromTag(tag);
}
void Gearbox::readCustomDataTag(XmlTag * tag)
{
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    friction = 0.01;
    inertia = 1.0;
    prevAngularVel = 0.0;
    inputAngularVel = 0.0;
    outputAngularVel = 0.0;
    angularAcc = 0.0;
    currentGear = 0;
    
    friction = stod (tag->getAttribute("gearboxFriction"));
    inertia = stod (tag->getAttribute("gearboxInertia"));
}

Gearbox::~Gearbox ()
{
}

pGearboxGear GearboxGear::create(XmlTag * tag)
{
    pGearboxGear tmp(new GearboxGear(tag));
    return tmp;
}
GearboxGear::GearboxGear (XmlTag * tag)
    :WorldObject("gearboxgear")
{
    constructFromTag(tag);
}
void GearboxGear::readCustomDataTag(XmlTag * tag)
{
    log->__format (LOG_CCREATOR, "Starting to parse a gearbox gear node");
    ratio = 1.0;
    ratio = stod (tag->getAttribute("ratio"));
    description = tag->getAttribute("description");
}

GearboxGear::~GearboxGear()
{
}
std::string GearboxGear::getDescription()
{
    return description;
}
void Gearbox::stepPhysics ()
{
    double dt;
    double torqueSum;
    
    double gearRatio = getCurrentGear()->getRatio();
    //gearRatio = getGearboxGear(itos(currentGear))->getRatio();
    
    dt = System::get()->getDesiredPhysicsTimestep();
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
    
    //log->__format(LOG_DEVELOPER, "inputTorque=%f outputTorque=%f inputVel=%f outputVel=%f", inputTorqueTransfer, outputTorqueTransfer, inputAngularVel, outputAngularVel);
    inputTorqueTransfer = 0;
    outputTorqueTransfer = 0;
}

void Gearbox::gearUp() {
    currentGear++;

    //FIXME count number of actual gears, not objects (there could be more than gears in the future!)
    int maxGear = objects.size() - 1;
    if(currentGear > maxGear) currentGear = maxGear;

    log->__format(LOG_DEVELOPER, "Gear set to %s", getCurrentGear()->getName().c_str()); 
}

void Gearbox::gearDown() {
    currentGear--;
    if(currentGear < 0) currentGear = 0;

    log->__format(LOG_DEVELOPER, "Gear set to %s", getCurrentGear()->getName().c_str()); 
}
std::string Gearbox::itos(int integer)
{
    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", integer);
    return std::string(numberString);
}
void Gearbox::setGear (int inputGear)     
{
    currentGear = inputGear;
    log->__format(LOG_DEVELOPER, "Gear set to %s", getCurrentGear()->getName().c_str()); 
}
int Gearbox::getCurrentGearIndex()
{
    return currentGear;
}
pGearboxGear Gearbox::getCurrentGear()
{
    return getGearboxGear(itos(currentGear));
}
int GearboxGear::getNumber()
{ 
    return stoi(name);
}
double GearboxGear::getRatio()
{
    return ratio;
}
bool GearboxGear::isEnabled()
{
    return enabled;
}
