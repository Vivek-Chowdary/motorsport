/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "gearbox.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

Gearbox::Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "GBX");
    log->__format (LOG_CCREATOR, "Starting to parse a gearbox node");
    processXmlRootNode (n);
    processXmlGearListNode (n);
}

Gearbox::~Gearbox ()
{
    stopPhysics ();
    delete log;
}


void Gearbox::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Gearbox::processXmlGearListNode(DOMNode * gearListNode)
{
    if (gearListNode != 0)
    {
        DOMNode * gearNode;
        for (gearNode = gearListNode->getFirstChild (); gearNode != 0; gearNode = gearNode->getNextSibling ())
        {
            if (gearNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, gearNode->getNodeName());
                if (nodeName == "gear")
                {
                    log->__format (LOG_CCREATOR, "Found a gear.");
                    GearboxGear * tmpGear = new GearboxGear (gearNode);
                    gearMap[tmpGear->getIndex()]=tmpGear;
  //                  tmpGear->setRefBody(body->bodyID);
                }
            }
        }
    }
}

GearboxGear::GearboxGear (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    log = new LogEngine (LOG_DEVELOPER, "GBG");
    log->__format (LOG_CCREATOR, "Starting to parse a gearbox gear node");
    processXmlRootNode (n);
}

void GearboxGear::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    ratio = 1.0;
    index = 0;
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
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the Gear Index: %s", attribute.c_str() );
                index = stoi (attribute);
            }
            if (attribute == "ratio")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the Gear Ratio: %s", attribute.c_str() );
                ratio = stod (attribute);
            }
            if (attribute == "label")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the Gear Label: %s", attribute.c_str() );
                label = attribute;
            }
        }
    }
}

