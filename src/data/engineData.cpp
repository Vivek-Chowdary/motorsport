/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "engine.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

class Pedal;

Engine::Engine (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "ENG");
    log->put (LOG_CCREATOR, "Starting to parse an engine node");
    processXmlRootNode (n);
}

Engine::~Engine ()
{
    stopPhysics ();
    delete log;
}


void Engine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Engine::setGasPedal (Pedal * pedal)
{
    gasPedal = pedal;
}
