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

#include "world.hpp"
#include "vehicle.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "clutch.hpp"
#include "gearbox.hpp"
#include "wheel.hpp"
#include "suspension.hpp"
#include "camera.hpp"
#include "cameraPosition.hpp"


int Vehicle::instancesCount = 0;

Vehicle::Vehicle (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "VEH");
    log->put (LOG_INFO, "Starting to parse the vehicle xml file");
    XmlFile * xmlFile = new XmlFile (xmlFilename.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;

    instancesCount++;
}

Vehicle::~Vehicle ()
{
    instancesCount--;

    stopPhysics ();
    stopGraphics ();
    stopInput ();

//    delete body;  //huh? :S
//    delete engine; //why does this crash?
    delete log;
}


void Vehicle::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    name = "None";
    revision = 0;
    description = "None";
    author = "Anonymous";
    contact = "None";
    license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    DOMNode * bodyNode = 0;
    DOMNode * engineNode = 0;
    DOMNode * clutchNode = 0;
    DOMNode * gearboxNode = 0;
    DOMNode * wheelListNode = 0;
    DOMNode * suspListNode = 0; //suspension
    DOMNode * cameraListNode = 0;

    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "vehicle")
            {
                log->put (LOG_TRACE, "Found a vehicle.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "position")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the name: %s", attribute.c_str());
                            Vector3d position;
                            position = stov3d (attribute);
                        }
                        if (attribute == "name")
                        {
                            name.clear();
                            assignXmlString (name, attNode->getValue());
                            log->format (LOG_TRACE, "Found the name: %s", name.c_str());
                        }
                        if (attribute == "revision")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the revision number: %s", attribute.c_str());
                            revision = stoi(attribute);
                        }
                        if (attribute == "description")
                        {
                            description.clear();
                            assignXmlString (description, attNode->getValue());
                            log->format (LOG_TRACE, "Found the description: %s", description.c_str());
                        }
                        if (attribute == "author")
                        {
                            author.clear();
                            assignXmlString (author, attNode->getValue());
                            log->format (LOG_TRACE, "Found the author: %s", author.c_str());
                        }
                        if (attribute == "contact")
                        {
                            contact.clear();
                            assignXmlString (contact, attNode->getValue());
                            log->format (LOG_TRACE, "Found the contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            license.clear();
                            assignXmlString (license, attNode->getValue());
                            log->format (LOG_TRACE, "Found the license: %s", license.c_str());
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
                            nodeName.clear();
                            assignXmlString (nodeName, n->getNodeName());
                            if (nodeName == "body")
                            {
                                log->put (LOG_TRACE, "Found a body.");
                                bodyNode = n;
                            }
                            if (nodeName == "engine")
                            {
                                log->put (LOG_TRACE, "Found an engine.");
                                engineNode = n;
                            }
                            if (nodeName == "clutch")
                            {
                                log->put (LOG_TRACE, "Found a clutch.");
                                clutchNode = n;
                            }
                            if (nodeName == "gearbox")
                            {
                                log->put (LOG_TRACE, "Found a gearbox.");
                                gearboxNode = n;
                            }
                            if (nodeName == "wheelList")
                            {
                                log->put (LOG_TRACE, "Found a wheel list.");
                                wheelListNode = n;
                            }
                            if (nodeName == "suspensionList")
                            {
                                log->put (LOG_TRACE, "Found a suspension list.");
                                suspListNode = n;
                            }
                            if (nodeName == "cameraList")
                            {
                                log->put (LOG_TRACE, "Found a camera list.");
                                cameraListNode = n;
                            }
                        }
                    }
                }
            }
            nodeName.clear();
        }
    }
    body = new Body (bodyNode);
    engine = new Engine (engineNode);
    clutch = new Clutch (clutchNode, engine);
    engine->setOutputPointer(clutch);
    gearbox = new Gearbox (gearboxNode, clutch);
    clutch->setOutputPointer(gearbox);
        
    processXmlWheelListNode(wheelListNode);
    processXmlSuspensionListNode(suspListNode);
    processXmlCameraListNode(cameraListNode);

    gearbox->setOutputPointer(wheelMap["RearRight"], wheelMap["RearLeft"]);
}

void Vehicle::processXmlWheelListNode(DOMNode * wheelListNode)
{
    if (wheelListNode != 0)
    {
        DOMNode * wheelNode;
        for (wheelNode = wheelListNode->getFirstChild (); wheelNode != 0; wheelNode = wheelNode->getNextSibling ())
        {
            if (wheelNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, wheelNode->getNodeName());
                if (nodeName == "wheel")
                {
                    log->put (LOG_TRACE, "Found a wheel.");
                    Wheel * tmpWheel = new Wheel (wheelNode);
                    wheelMap[tmpWheel->getIndex()]=tmpWheel;
                }
                nodeName.clear();
            }
        }
    }
}

void Vehicle::processXmlSuspensionListNode(DOMNode * suspListNode)
{
    if (suspListNode != 0)
    {
        DOMNode * suspNode;
        for (suspNode = suspListNode->getFirstChild (); suspNode != 0; suspNode = suspNode->getNextSibling ())
        {
            if (suspNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, suspNode->getNodeName());
                if (nodeName == "unidimensional")
                {
                    log->put (LOG_TRACE, "Found a suspension.");
                    Suspension * tmpSusp = new Suspension (suspNode);
                    suspensionMap[tmpSusp->getIndex()]=tmpSusp;
                }
                nodeName.clear();
            }
        }
    }
}

void Vehicle::processXmlCameraListNode(DOMNode * cameraListNode)
{
    if (cameraListNode != 0)
    {
        DOMNode * cameraNode;
        for (cameraNode = cameraListNode->getFirstChild (); cameraNode != 0; cameraNode = cameraNode->getNextSibling ())
        {
            if (cameraNode->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string nodeName;
                assignXmlString (nodeName, cameraNode->getNodeName());
                if (nodeName == "camera")
                {
                    log->put (LOG_TRACE, "Found a camera position.");
                    processXmlCameraPositionNode (cameraNode);
                }
                nodeName.clear();
            }
        }
    }
}

void Vehicle::processXmlCameraPositionNode (DOMNode * n)
{
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        std::string index = "0";
        Vector3d position (0, 0, 0);
        Vector3d target (0, 0, 0);
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                index.clear();
                assignXmlString (index, attNode->getValue());
                log->format (LOG_TRACE, "Found the position index: %s", index.c_str());
            }
            if (attribute == "position")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue()); 
                log->format (LOG_TRACE, "Found the position: %s", attribute.c_str());
                position = stov3d(attribute);
            }
            if (attribute == "target")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the target: %s", attribute.c_str());
                target = stov3d(attribute);
            }
            attribute.clear();
        }
        CameraPosition * tmpCam = new CameraPosition (position.x, position.y, position.z, target.x, target.y, target.z);
        cameraPositionMap[index]=tmpCam;
        //load some cameras FIXME should be taken from file config
        log->put (LOG_INFO, "Creating a camera");
        Camera *cameraPointer = new Camera (position.x, position.y, position.z, target.x, target.y, target.z);
        cameraList.push_back (cameraPointer);
    }
}
