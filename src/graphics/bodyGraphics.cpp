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

void Body::startGraphics (DOMNode * n)
{
    BodyGraphicsData * graphics = new BodyGraphicsData;
    
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

    char name[20];
    sprintf (name, graphics->ogreName, instancesCount);
    bodyEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, graphics->mesh);
    bodyEntity->setMaterialName (graphics->material);
    bodyNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    bodyNode->attachObject (bodyEntity);
 
    delete[](graphics->material);
    delete[](graphics->mesh);
    delete[](graphics->ogreName);
    delete graphics;
}
void Body::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Body::stopGraphics ()
{
    // empty
}

