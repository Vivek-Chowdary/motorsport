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

void Body::startPhysics (DOMNode * n)
{
    BodyPhysicsData * physics = new BodyPhysicsData;
    physics->mass = 100;
    physics->length = 1;
    physics->width = 1;
    physics->height = 1;

    log->put (LOG_TRACE, "Parsing body physic.");
    if (n == 0)
    {
        log->put (LOG_ERROR, "Null body physics XML node");
    }
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

    dMass mass;
    dMassSetBoxTotal (&mass, physics->mass, physics->length, physics->width, physics->height);
    bodyID = dBodyCreate (WorldData::getWorldDataPointer ()->worldID);
    bodyGeomID = dCreateBox (WorldData::getWorldDataPointer ()->spaceID, physics->length, physics->width, physics->height);
    dGeomSetBody (bodyGeomID, bodyID);

    delete physics;
}

void Body::setPosition (float posX, float posY, float posZ)
{               
    dBodySetPosition (bodyID, posX, posY, posZ);
}

void Body::stopPhysics ()
{
    dGeomDestroy (bodyGeomID);
    dBodyDestroy (bodyID);
}

void Body::stepPhysics ()
{
}
