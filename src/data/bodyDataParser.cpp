
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

void Body::processBodyDataFile (DOMNode * n, void *data)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            log->format (LOG_TRACE, "Name: %s", name);;

            if (!strncmp (name, "body", 5))
            {
                log->put (LOG_TRACE, "Found the body main data config element.");
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
                                log->put (LOG_TRACE, "Found the body graphics data element.");
                                (*(BodyData *) data).body->processBodyGraphicsDataNode (n, (*(BodyData *) data).graphics);
                            }
                            if (!strncmp (name, "physics", 8))
                            {
                                log->put (LOG_TRACE, "Found the body physics data element.");
                                (*(BodyData *) data).body->processBodyPhysicsDataNode (n, (*(BodyData *) data).physics);
                            }
                        }
                    }
                }
            }
        }
    }
    delete log;
}

void Body::processBodyGraphicsDataNode (DOMNode * n, BodyGraphicsData * graphics)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    log->put (LOG_TRACE, "Parsing body graphics");
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
                log->format (LOG_TRACE, "\tFound the body graphics material: %s", name);

                graphics->material = new char[strlen (name) + 1];
                strncpy (graphics->material, name, strlen (name) + 1);
            }

            if (!strncmp (name, "mesh", 5))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body graphics mesh filename: %s", name);

                graphics->mesh = new char[strlen (name) + 1];
                strncpy (graphics->mesh, name, strlen (name) + 1);
            }
            if (!strncmp (name, "ogreName", 9))
            {
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body graphics ogre-identifier format: %s", name);

                graphics->ogreName = new char[strlen (name) + 1];
                strncpy (graphics->ogreName, name, strlen (name) + 1);
            }
            XMLString::release (&name);
        }
    }
    log->put (LOG_TRACE, "Finished body graphics.");
    delete log;
}

void Body::processBodyPhysicsDataNode (DOMNode * n, BodyPhysicsData * physics)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    log->put (LOG_TRACE, "Parsing body physic.");
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
            if (!strncmp (name, "mass", 5))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body total mass: %s", name);

                physics->mass = atof (name);
            }
            if (!strncmp (name, "length", 7))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body length: %s", name);

                physics->length = atof (name);
            }
            if (!strncmp (name, "width", 6))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body width: %s", name);

                physics->width = atof (name);
            }
            if (!strncmp (name, "height", 7))
            {
                XMLString::release (&name);
                name = XMLString::transcode (pAttributeNode->getValue ());
                log->format (LOG_TRACE, "\tFound the body height: %s", name);

                physics->height = atof (name);
            }
            XMLString::release (&name);
        }
    }
    log->put (LOG_TRACE, "Finished body physic.");
    delete log;
}
