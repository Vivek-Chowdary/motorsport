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

#include "cube.hpp"

Cube::Cube (char * xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "CUB");

    XmlFile * xmlFile = new XmlFile (xmlFilename);
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Cube::~Cube ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
}


void Cube::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (cubeID);  // need to allocate memory first??
    cubeNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
float Cube::getMoveToXPositive ()
{
    return moveToXPositive;
}
float Cube::getMoveToXNegative ()
{
    return moveToXNegative;
}

float Cube::getMoveToYPositive ()
{
    return moveToYPositive;
}
float Cube::getMoveToYNegative ()
{
    return moveToYNegative;
}
void Cube::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (cubeID);    // need to allocate memory first??
    cubeNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

void Cube::processXmlRootNode (DOMNode * n)
{
    DOMNode * graphicsNode = 0;
    DOMNode * physicsNode = 0;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            log->format (LOG_TRACE, "Name: %s", name);;

            if (!strncmp (name, "cube", 5))
            {
                log->put (LOG_TRACE, "Found the cube main data config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                    int nSize = pAttributes->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *name = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (name, "name", 5))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_TRACE, "\tFound the name: %s", name);
                        }
                        if (!strncmp (name, "description", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_TRACE, "\tFound the description: %s", name);
                        }
                        if (!strncmp (name, "author", 7))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_TRACE, "\tFound the author: %s", name);
                        }
                        if (!strncmp (name, "contact", 8))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_TRACE, "\tFound the contact information: %s", name);
                        }
                        if (!strncmp (name, "license", 8))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_TRACE, "\tFound the license: %s", name);
                        }
                        XMLString::release (&name);
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            char *name = XMLString::transcode (n->getNodeName ());
                            log->format (LOG_TRACE, "Name: %s", name);;
                            if (!strncmp (name, "graphics", 9))
                            {
                                log->put (LOG_TRACE, "Found the cube graphics data element.");
                                graphicsNode = n;
                            }
                            if (!strncmp (name, "physics", 8))
                            {
                                log->put (LOG_TRACE, "Found the cube physics data element.");
                                physicsNode = n;
                            }
                        }
                    }
                }
            }
        }
    }
    startGraphics(graphicsNode);
    startPhysics(physicsNode);
    startInput ();
}

