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

#include "suspension.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"

int Suspension::instancesCount = 0;

Suspension::Suspension (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_TRACE, "SUS");
    log->put (LOG_INFO, "Starting to parse the suspension node");
    processXmlRootNode (n);

    instancesCount++;
}

Suspension::~Suspension ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

    delete log;
}

std::string Suspension::getIndex()
{
    return index;
}

void Suspension::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    index.assign ("Unknown");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "unidimensional")
            {
                log->put (LOG_TRACE, "Found an unidimensional suspension.");
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
            }
            nodeName.clear();
        }
    }
    startPhysics (n);
}
