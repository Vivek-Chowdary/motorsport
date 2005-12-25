/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "wheel.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"

int Wheel::instancesCount = 0;

Wheel::Wheel (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "WHE");
    log->__format (LOG_DEVELOPER, "Starting to parse a wheel node");
    processXmlRootNode (n);

    instancesCount++;
}

Wheel::~Wheel ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

    delete log;
}


void Wheel::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (wheelID);  // need to allocate memory first??
    wheelNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Wheel::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (wheelID);    // need to allocate memory first??
    wheelNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

std::string Wheel::getIndex()
{
    return index;
}
void Wheel::setBrakePedal (Pedal * pedal)
{
    brakePedal = pedal;
}

void Wheel::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    index.assign ("Unknown");
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                assignXmlString (index, attNode->getValue());
                log->__format (LOG_DEVELOPER, "Found the index: %s", index.c_str());
            }
        }
    }
    startPhysics (n);
    startGraphics (n);
}

void Wheel::setUserDriver ()
{
    userDriver = true;
}
