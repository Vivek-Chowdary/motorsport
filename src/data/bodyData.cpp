/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "body.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"

int Body::instancesCount = 0;

Body::Body (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_TRACE, "BOD");
    processXmlRootNode (n);

    instancesCount++;
}

Body::~Body ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

    delete log;
}


void Body::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (bodyID);  // need to allocate memory first??
    bodyNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Body::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (bodyID);    // need to allocate memory first??
    bodyNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

void Body::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
    startGraphics (n);
}
