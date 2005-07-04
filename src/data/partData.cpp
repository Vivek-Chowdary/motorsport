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
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"

int Part::instancesCount = 0;

Part::Part (const std::string & partName)
{
    log = new LogEngine (LOG_DEVELOPER, "PAR");
    partType = partName;
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/parts/");
    file.append(partName);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(file, "FileSystem", "Parts - " + partName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Parts - " + partName);
    file.append("/part.xml");
    XmlFile * xmlFile = new XmlFile (file.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Part::~Part ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();
    
    log->format(LOG_DEVELOPER, "Removed a part. %i left.", instancesCount);
    delete log;
}


void Part::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (partID);  // need to allocate memory first??
    partNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Part::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (partID);    // need to allocate memory first??
    partNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

void Part::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    DOMNode * partNode = n;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            if (name == "part")
            {
                log->put (LOG_CCREATOR, "Found a part.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "name")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the name: %s", attribute.c_str());
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the description: %s", attribute.c_str());
                        }
                        if (attribute == "author")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the author: %s", attribute.c_str());
                        }
                        if (attribute == "contact")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the contact information: %s", attribute.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the license: %s", attribute.c_str());
                        }
                    }
                }
            }
        }
    }
    startGraphics(partNode);
    startPhysics(partNode);
    startInput ();
}

