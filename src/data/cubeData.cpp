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
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Cube::instancesCount = 0;

Cube::Cube (char * xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "CUB");

    XmlFile * xmlFile = new XmlFile (xmlFilename);
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Cube::~Cube ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();
    
    log->format(LOG_TRACE, "Removed a cube. %i left.", instancesCount);
    delete log;
}


void Cube::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (cubeID);  // need to allocate memory first??
    cubeNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
void Cube::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (cubeID);    // need to allocate memory first??
    cubeNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}

void Cube::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    DOMNode * graphicsNode = 0;
    DOMNode * physicsNode = 0;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            if (name == "cube")
            {
                log->put (LOG_TRACE, "Found a cube.");
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
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the name: %s", attribute.c_str());
                        }
                        if (attribute == "revision")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the revision number: %s", attribute.c_str());
                        }
                        if (attribute == "description")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the description: %s", attribute.c_str());
                        }
                        if (attribute == "author")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the author: %s", attribute.c_str());
                        }
                        if (attribute == "contact")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the contact information: %s", attribute.c_str());
                        }
                        if (attribute == "license")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the license: %s", attribute.c_str());
                        }
                        attribute.clear();
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            name.clear();
                            assignXmlString (name, n->getNodeName());
                            if (name == "graphics")
                            {
                                log->put (LOG_TRACE, "Found the cube graphics.");
                                graphicsNode = n;
                            }
                            if (name == "physics")
                            {
                                log->put (LOG_TRACE, "Found the cube physics.");
                                physicsNode = n;
                            }
                        }
                    }
                }
            }
            name.clear();
        }
    }
    startGraphics(graphicsNode);
    startPhysics(physicsNode);
    startInput ();
}

