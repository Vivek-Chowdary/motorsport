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
#include "xmlParser.hpp"
#include "ode.h"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "track.hpp"
#include "SDL/SDL_keysym.h"

void Cube::startPhysics (DOMNode * n)
{
    int size = 100;
    std::string author = "Anonymous";
    std::string contact = "None";
    std::string license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    log->put (LOG_TRACE, "Parsing cube physics.");
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
            if (attribute == "size")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the cube physics size: %s", attribute.c_str() );
                size = stoi (attribute);
            }
            attribute.clear();
        }
    }
    log->put (LOG_TRACE, "Finished cube physics.");
    dMass mass;
    dMassSetBox (&mass, 1, size, size, size);
    cubeID = dBodyCreate (World::getWorldPointer ()->worldID);
    cubeGeomID = dCreateBox (World::getWorldPointer ()->spaceID, size, size, size);
    dGeomSetBody (cubeGeomID, cubeID);

    author.clear();
    license.clear();
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
    if (this == World::getWorldPointer ()->trackList[0]->cubeList[0])
    {
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_l)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_i)]->getValue();
        moveX /= 10000;
        moveY /= 10000;
        dBodyAddForce (cubeID, moveX, moveY, 0.0f);
        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_j)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_k)]->getValue();
        moveX /= 10000;
        moveY /= 10000;
        dBodyAddForce (cubeID, moveX, moveY, 0.0f);
    }
}

