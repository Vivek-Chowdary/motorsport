/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "clutch.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Clutch::instancesCount = 0;

Clutch::Clutch (XERCES_CPP_NAMESPACE::DOMNode * n, Engine *input)
{
    log = new LogEngine (LOG_TRACE, "CLU");
    log->put (LOG_INFO, "Starting to parse a clutch node");
    processXmlRootNode (n);
    instancesCount++;
    inputClass = input;
}

Clutch::~Clutch ()
{
    instancesCount--;
    stopPhysics ();
    delete log;
}


void Clutch::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Clutch::setOutputPointer(Gearbox *output)
{
    pOutTorque = output;
}
