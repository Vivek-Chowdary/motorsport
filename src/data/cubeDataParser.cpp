
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

#include "cube.hpp"

void Cube::processCubeDataFile (DOMNode * n, void *data)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
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
                        if (!strncmp (name, "contact", 5))
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
                                (*(CubeData *) data).cube->processCubeGraphicsDataNode (n, (*(CubeData *) data).graphics);
                            }
                            if (!strncmp (name, "physics", 8))
                            {
                                log->put (LOG_TRACE, "Found the cube physics data element.");
                                (*(CubeData *) data).cube->processCubePhysicsDataNode (n, (*(CubeData *) data).physics);
                            }
                        }
                    }
                }
            }
        }
    }
    delete log;
}

void Cube::processCubeGraphicsDataNode (DOMNode * n, CubeGraphicsData * graphics)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    log->put (LOG_TRACE, "Parsing cube graphics");
    if (n->hasAttributes ())
    {
        // get all the attributes of the node
        DOMNamedNodeMap *pAttributes = n->getAttributes ();
        int nSize = pAttributes->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
            char *name = XMLString::transcode (pAttributeNode->getName ());
            if (!strncmp (name, "author", 7))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the author: %s", name);
            }
            if (!strncmp (name, "contact", 5))
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
            if (!strncmp (name, "material", 9))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the cube graphics material: %s", name);

                graphics->material = new char[strlen (name) + 1];
                strncpy (graphics->material, name, strlen (name) + 1);
            }

            if (!strncmp (name, "mesh", 5))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the cube graphics mesh filename: %s", name);

                graphics->mesh = new char[strlen (name) + 1];
                strncpy (graphics->mesh, name, strlen (name) + 1);
            }
            if (!strncmp (name, "ogreName", 9))
            {
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the cube graphics ogre-identifier format: %s", name);

                graphics->ogreName = new char[strlen (name) + 1];
                strncpy (graphics->ogreName, name, strlen (name) + 1);
            }
            XMLString::release (&name);
        }
    }
    log->put (LOG_TRACE, "Finished cube graphics.");
    delete log;
}

void Cube::processCubePhysicsDataNode (DOMNode * n, CubePhysicsData * physics)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    log->put (LOG_TRACE, "Parsing cube physic.");
    if (n->hasAttributes ())
    {
        // get all the attributes of the node
        DOMNamedNodeMap *pAttributes = n->getAttributes ();
        int nSize = pAttributes->getLength ();

        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
            char *name = XMLString::transcode (pAttributeNode->getName ());
            if (!strncmp (name, "author", 7))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the author: %s", name);
            }
            if (!strncmp (name, "contact", 5))
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
            if (!strncmp (name, "size", 5))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the cube physics size: %s", name);

                physics->size = atoi (name);
            }
            XMLString::release (&name);
        }
    }
    log->put (LOG_TRACE, "Finished cube physic.");
    delete log;
}
