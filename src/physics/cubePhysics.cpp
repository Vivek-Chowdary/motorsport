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

void Cube::startPhysics (DOMNode * n)
{
    CubePhysicsData * physics = new CubePhysicsData;
    physics->size = 100;
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
    dMass mass;
    dMassSetBox (&mass, 1000, physics->size, physics->size, physics->size);
    cubeID = dBodyCreate (WorldData::getWorldDataPointer ()->worldID);
    cubeGeomID = dCreateBox (WorldData::getWorldDataPointer ()->spaceID, physics->size, physics->size, physics->size);
    dGeomSetBody (cubeGeomID, cubeID);
    delete physics;
}

void Cube::setPosition (float posX, float posY, float posZ)
{
    dBodySetPosition (cubeID, posX, posY, posZ);
}

void Cube::stopPhysics ()
{
    dGeomDestroy (cubeGeomID);
    dBodyDestroy (cubeID);
}

void Cube::stepPhysics ()
{
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (cubeID, (*(dReal *) (dBodyGetAngularVel (cubeID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (cubeID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (cubeID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    float moveToX = 0, moveToY = 0;

    moveToX += getMoveToXPositive ();
    moveToX -= getMoveToXNegative ();
    moveToY += getMoveToYPositive ();
    moveToY -= getMoveToYNegative ();
    moveToX /= 10000;
    moveToY /= 10000;
    dBodyAddForce (cubeID, moveToX, moveToY, 0.0f);
}

