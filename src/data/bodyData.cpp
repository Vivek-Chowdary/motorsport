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

#include "body.hpp"

int Body::instancesCount = 0;

Body::Body (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "BOD");
    log->put (LOG_INFO, "Starting to parse the body xml file");
    XmlFile * xmlFile = new XmlFile (xmlFilename.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

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

void Body::processXmlRootNode (DOMNode * n)
{
    DOMNode * graphicsNode = 0;
    DOMNode * physicsNode = 0;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            log->format (LOG_TRACE, "Name: %s", name.c_str());;
            if (name == "body")
            {
                log->put (LOG_TRACE, "Found the body main data config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "name")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the name: %s", attribute.c_str());
                        }
                        if (attribute == "description")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the description: %s", attribute.c_str());
                        }
                        if (attribute == "author")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the author: %s", attribute.c_str());
                        }
                        if (attribute == "contact")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the contact information: %s", attribute.c_str());
                        }
                        if (attribute == "license")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the license: %s", attribute.c_str());
                        }
                        attribute.clear();
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            name.clear();
                            assignXmlString (name, n->getNodeName());
                            log->format (LOG_TRACE, "Name: %s", name.c_str());
                            if (name == "graphics")
                            {
                                log->put (LOG_TRACE, "Found the body graphics data element.");
                                graphicsNode = n;
                            }
                            if (name == "physics")
                            {
                                log->put (LOG_TRACE, "Found the body physics data element.");
                                physicsNode = n;
                            }
                        }
                    }
                }
            }
            name.clear();
        }
    }
    startPhysics (physicsNode);
    startGraphics (graphicsNode);
}
