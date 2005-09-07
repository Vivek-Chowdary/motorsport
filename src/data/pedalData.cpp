/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "pedal.hpp"

Pedal::Pedal (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "PED");
    log->put (LOG_CCREATOR, "Starting to parse a pedal node");
    processXmlRootNode (n);
    userDriver = false;
}

Pedal::~Pedal ()
{
    stopPhysics ();
    delete log;
}


void Pedal::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Pedal::setUserDriver ()
{
    userDriver = true;
}

std::string Pedal::getId()
{
    return id;
}

