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

    log->put (LOG_TRACE, "Parsing cube graphics");
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
            if (attribute == "material")
            {
                material.clear();
                assignXmlString (material, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the cube graphics material: %s", material.c_str());
            }
            if (attribute == "mesh")
            {
                mesh.clear();
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the cube graphics mesh filename: %s", mesh.c_str());
            }
            if (attribute == "ogreName")
            {
                ogreName.clear();
                assignXmlString (ogreName, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the cube graphics ogre-identifier format: %s", ogreName.c_str());
            }
            attribute.clear();
        }
    }
    log->put (LOG_TRACE, "Finished cube graphics.");

    char name[256];
    sprintf (name, ogreName.c_str(), instancesCount);
    cubeEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, mesh.c_str());
   cubeEntity->setMaterialName (material.c_str());
    cubeNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    cubeNode->attachObject (cubeEntity);
 
    author.clear();
    contact.clear();
    license.clear();
    material.clear();
    mesh.clear();
    ogreName.clear();
}
