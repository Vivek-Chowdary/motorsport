/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "driveJoint.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

Clutch::Clutch (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "CLJ");
    log->put (LOG_CCREATOR, "Starting to parse a clutch node");
    processXmlRootNode (n);
}

Clutch::Clutch ()
{
    log = new LogEngine (LOG_DEVELOPER, "TFR");
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    maxTorqueTransfer = 1000;
    lockedParam = 1;
}

Clutch::~Clutch ()
{
    stopPhysics ();
    delete log;
}


void Clutch::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

Gear::Gear (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "GTJ");
    log->put (LOG_CCREATOR, "Starting to parse a gear joint node");
    processXmlRootNode (n);
}

Gear::Gear ()
{
    log = new LogEngine (LOG_DEVELOPER, "TFJ");
    ratio = 1.0;
    springConstant = 300.0;
    dampConstant = 8;
    relAngle = 0.0;
    prevRelAngle = 0.0;
    relAngularVel = 0.0;
    prevRelAngularVel = 0.0;
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
}

Gear::~Gear ()
{
    stopPhysics ();
    delete log;
}

void Gear::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

LSD::LSD (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "LSJ");
    log->put (LOG_CCREATOR, "Starting to parse a gear joint node");
    processXmlRootNode (n);
}

LSD::LSD ()
{
    log = new LogEngine (LOG_DEVELOPER, "LSJ");
    ratio = 1.0;
    springConstant = 300.0;
    dampConstant = 8;
    relAngle = 0.0;
    prevRelAngle = 0.0;
    relAngularVel = 0.0;
    prevRelAngularVel = 0.0;
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
    outputsRelAngle = 0.0;
    prevOutputsRelAngle = 0.0;
    outputsRelAngularVel = 0.0;
    prevOutputsRelAngularVel = 0.0;

}

LSD::~LSD ()
{
    stopPhysics ();
    delete log;
}

void LSD::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}
