/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "diff.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

Diff::Diff (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_TRACE, "DFF");
    log->put (LOG_INFO, "Starting to parse a differential node");
    processXmlRootNode (n);
}

Diff::~Diff ()
{
    stopPhysics ();
    delete log;
}


void Diff::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

