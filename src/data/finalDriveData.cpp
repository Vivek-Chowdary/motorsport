/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "finalDrive.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

FinalDrive::FinalDrive (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "FDM");
    log->__format (LOG_CCREATOR, "Starting to parse a FinalDrive Mass node");
    processXmlRootNode (n);
}

FinalDrive::~FinalDrive ()
{
    stopPhysics ();
    delete log;
}


void FinalDrive::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}
