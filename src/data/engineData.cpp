/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Engine::instancesCount = 0;

Engine::Engine (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_TRACE, "ENG");
    log->put (LOG_INFO, "Starting to parse an engine node");
    processXmlRootNode (n);
    instancesCount++;
}

Engine::~Engine ()
{
    instancesCount--;
    stopPhysics ();
    delete log;
}


void Engine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Engine::setOutputPointer(Clutch *output)
{
    pOutTorque = output;
}
