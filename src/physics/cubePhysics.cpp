/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "cube.hpp"
#include "xmlParser.hpp"
#include "ode/ode.h"
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
                assignXmlString (author, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the author: %s", author.c_str());
            }
            if (attribute == "contact")
            {
                assignXmlString (contact, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the contact information: %s", contact.c_str());
            }
            if (attribute == "license")
            {
                assignXmlString (license, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the license: %s", license.c_str());
            }
            if (attribute == "size")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the cube physics size: %s", attribute.c_str() );
                size = stoi (attribute);
            }
        }
    }
    dMass mass;
    dMassSetBox (&mass, 1, size, size, size);
    cubeID = dBodyCreate (World::getWorldPointer ()->worldID);
    cubeGeomID = dCreateBox (World::getWorldPointer ()->spaceID, size, size, size);
    dGeomSetBody (cubeGeomID, cubeID);
    dBodySetMass (cubeID, &mass);
}

void Cube::setPosition (Vector3d position)
{
    dBodySetPosition (cubeID, position.x, position.y, position.z);
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
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (cubeID, moveX, moveY, 0.0f);
        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_j)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_k)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (cubeID, moveX, moveY, 0.0f);
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_SPACE)]->getValue() * 500;
        dBodyAddForce (cubeID, 0, 0, moveZ);
    }
    const dReal * pos;
    pos = dBodyGetPosition(cubeID);
    log->format(LOG_DEVELOPER, "cube:x=%f y=%f z=%f", pos[0], pos[1], pos[2]);

}

