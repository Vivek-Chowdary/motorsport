/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
