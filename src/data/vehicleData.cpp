/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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

#include "vehicle.hpp"

int Vehicle::instancesCount = 0;

Vehicle::Vehicle (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "VEH");
    log->put (LOG_INFO, "Starting to parse the vehicle xml file");
    XmlFile * xmlFile = new XmlFile (xmlFilename.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Vehicle::~Vehicle ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

    delete log;
}


void Vehicle::processXmlRootNode (DOMNode * n)
{
    name = "None";
    description = "None";
    author = "Anonymous";
    contact = "None";
    license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    DOMNode * bodyNode = 0;

    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            log->format (LOG_TRACE, "Name: %s", nodeName.c_str());;
            if (nodeName == "vehicle")
            {
                log->put (LOG_TRACE, "Found the vehicle main data config element.");
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
                            name.clear();
                            assignXmlString (name, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the name: %s", name.c_str());
                        }
                        if (attribute == "description")
                        {
                            description.clear();
                            assignXmlString (description, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the description: %s", description.c_str());
                        }
                        if (attribute == "author")
                        {
                            author.clear();
                            assignXmlString (author, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the author: %s", author.c_str());
                        }
                        if (attribute == "contact")
                        {
                            contact.clear();
                            assignXmlString (contact, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            license.clear();
                            assignXmlString (license, attNode->getValue());
                            log->format (LOG_TRACE, "\tFound the license: %s", license.c_str());
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
                            nodeName.clear();
                            assignXmlString (nodeName, n->getNodeName());
                            log->format (LOG_TRACE, "Name: %s", nodeName.c_str());
                            if (nodeName == "body")
                            {
                                log->put (LOG_TRACE, "Found the vehicle body element.");
                                bodyNode = n;
                            }
/*                            if (nodeName == "physics")
                            {
                                log->put (LOG_TRACE, "Found the vehicle physics data element.");
                                physicsNode = n;
                            }*/
                        }
                    }
                }
            }
            nodeName.clear();
        }
    }
    body = new Body (bodyNode);
}
