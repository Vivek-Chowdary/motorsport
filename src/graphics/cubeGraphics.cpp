/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "cube.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

void Cube::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Cube::stopGraphics ()
{
    // empty
}

void Cube::startGraphics (DOMNode * n)
{
    std::string author = "Anonymous";
    std::string contact = "None";
    std::string license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    std::string material = "None";
    std::string mesh = "None";
    std::string ogreName = "None";
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
            if (attribute == "material")
            {
                assignXmlString (material, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the cube graphics material: %s", material.c_str());
            }
            if (attribute == "mesh")
            {
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the cube graphics mesh filename: %s", mesh.c_str());
            }
            if (attribute == "ogreName")
            {
                assignXmlString (ogreName, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the cube graphics ogre-identifier format: %s", ogreName.c_str());
            }
        }
    }
    char name[256];
    sprintf (name, ogreName.c_str(), instancesCount);
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/parts/cube/");
    file.append(mesh);
    cubeEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, mesh.c_str());
    cubeEntity->setMaterialName (material.c_str());
    cubeNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    cubeNode->attachObject (cubeEntity);
}
