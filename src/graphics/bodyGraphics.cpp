/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "body.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

void Body::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
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
            if (attribute == "material")
            {
                material.clear();
                assignXmlString (material, attNode->getValue());
                log->format (LOG_TRACE, "Found the body graphics material: %s", material.c_str());
            }
            if (attribute == "mesh")
            {
                mesh.clear();
                assignXmlString (mesh, attNode->getValue());
                log->format (LOG_TRACE, "Found the body graphics mesh filename: %s", mesh.c_str());
            }
            if (attribute == "ogreName")
            {
                ogreName.clear();
                assignXmlString (ogreName, attNode->getValue());
                log->format (LOG_TRACE, "Found the body graphics ogre-identifier format: %s", ogreName.c_str());
            }
            attribute.clear();
        }
    }
    char name[256];
    sprintf (name, ogreName.c_str(), instancesCount);
    bodyEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity (name, mesh.c_str());
   bodyEntity->setMaterialName (material.c_str());
    bodyNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
    bodyNode->attachObject (bodyEntity);
 
    material.clear();
    mesh.clear();
    ogreName.clear();
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

