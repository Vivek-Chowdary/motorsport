/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "part.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

void Part::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Part::stopGraphics ()
{
    // empty
}

void Part::startGraphics (DOMNode * n)
{
    std::string author = "Anonymous";
    std::string contact = "None";
    std::string license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    std::string mesh = "None";
    if (n->hasAttributes ())
    {
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
            if (attribute == "mesh")
            {
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the part graphics mesh filename: %s", mesh.c_str());
            }
        }
    }
    char number[256];
    sprintf (number, "%i", instancesCount);
    std::string name (partType + " #");
    name.append(number);
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/parts/");
    file.append(partType);
    file.append("/");
    file.append(mesh);
    partEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name.c_str(), file.c_str());
    partNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    partNode->attachObject (partEntity);
}
