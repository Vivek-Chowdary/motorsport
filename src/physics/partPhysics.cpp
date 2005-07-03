/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "part.hpp"
#include "xmlParser.hpp"
#include "ode/ode.h"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "system.hpp"
#include "track.hpp"
#include "SDL/SDL_keysym.h"

void Part::startPhysics (DOMNode * n)
{
    int size = 1;
    double mass = -1;
    double density = -1;
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
                log->format (LOG_CCREATOR, "Found the part physics size: %s", attribute.c_str() );
                size = stoi (attribute);
            }
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the part physics mass: %s", attribute.c_str() );
                mass = stod (attribute);
            }
            if (attribute == "density")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the part physics density: %s", attribute.c_str() );
                density = stod (attribute);
            }
        }
    }
    dMass dmass;
    if (mass < 0)
    {
        if (density < 0)
        {
            log->put (LOG_WARNING, "No correct mass or density values were found for this part.");
            dMassSetBox (&dmass, 100, size, size, size);
        } else {
            dMassSetBox (&dmass, density, size, size, size);
        }
    } else {
        if (density < 0) log->put (LOG_WARNING, "Total mass given, density value will be ignored");
        dMassSetBoxTotal (&dmass, mass, size, size, size);
    }
    partID = dBodyCreate (World::getWorldPointer ()->worldID);
    partGeomID = dCreateBox (World::getWorldPointer ()->spaceID, size, size, size);
    dGeomSetBody (partGeomID, partID);
    dBodySetMass (partID, &dmass);
}

void Part::setPosition (Vector3d position)
{
    dBodySetPosition (partID, position.x, position.y, position.z);
}

void Part::setRotation (Quaternion rotation)
{
    dMatrix3 rot;
    rotation.getOdeMatrix (rot);
    dBodySetRotation (partID, rot);
}

void Part::stopPhysics ()
{
    dGeomDestroy (partGeomID);
    dBodyDestroy (partID);
}

void Part::stepPhysics ()
{
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    if (this == World::getWorldPointer ()->trackList[0]->partList[0])
    {
        float moveX = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_l)]->getValue();
        float moveY = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_i)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        moveX = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_j)]->getValue();
        moveY = -SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_k)]->getValue();
        moveX *= 100;
        moveY *= 100;
        dBodyAddForce (partID, moveX, moveY, 0.0f);
        float moveZ = SystemData::getSystemDataPointer()->axisMap[getIDKeyboardKey(SDLK_SPACE)]->getValue() * 500;
        dBodyAddForce (partID, 0, 0, moveZ);
    }
    const dReal * pos;
    pos = dBodyGetPosition(partID);
    log->format(LOG_DEVELOPER, "part:x=%f y=%f z=%f", pos[0], pos[1], pos[2]);

}

