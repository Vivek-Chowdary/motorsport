/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#include "wheel.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"

int Wheel::instancesCount = 0;

Wheel::Wheel (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_TRACE, "WHE");
    log->put (LOG_INFO, "Starting to parse a wheel node");
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
                index.clear();
                assignXmlString (index, attNode->getValue());
                log->format (LOG_TRACE, "Found the index: %s", index.c_str());
            }
            attribute.clear();
        }
    }
    startPhysics (n);
    startGraphics (n);
}
