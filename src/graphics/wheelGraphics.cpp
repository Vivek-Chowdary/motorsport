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

#include "wheel.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

void Wheel::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    std::string material = "None";
    std::string mesh = "None";
    log->put (LOG_TRACE, "Parsing wheel graphics");
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "material")
            {
                material.clear();
                assignXmlString (material, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the wheel graphics material: %s", material.c_str());
            }
            if (attribute == "mesh")
            {
                mesh.clear();
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_TRACE, "\tFound the wheel graphics mesh filename: %s", mesh.c_str());
            }
            attribute.clear();
        }
    }
    log->put (LOG_TRACE, "Finished wheel graphics.");

    char name[256];
    std::string ogreName = name;
    ogreName.append ("%i");
    sprintf (name, ogreName.c_str(), instancesCount);
    wheelEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, mesh.c_str());
   wheelEntity->setMaterialName (material.c_str());
    wheelNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    wheelNode->attachObject (wheelEntity);
 
    material.clear();
    mesh.clear();
    ogreName.clear();
}
void Wheel::stepGraphics ()
{
    updateOgrePosition ();
    updateOgreOrientation ();
}

void Wheel::stopGraphics ()
{
    // empty
}

