/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Gearbox::instancesCount = 0;

Gearbox::Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n, Clutch *input)
{
    log = new LogEngine (LOG_TRACE, "GBX");
    log->put (LOG_INFO, "Starting to parse a gearbox node");
    processXmlRootNode (n);
    instancesCount++;
    inputClass = input;
}

Gearbox::~Gearbox ()
{
    instancesCount--;
    stopPhysics ();
    delete log;
}


void Gearbox::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Gearbox::setOutputPointer(Wheel * wheel1, Wheel * wheel2)
{
    pOutWheel1 = wheel1;
    pOutWheel2 = wheel2;
}
