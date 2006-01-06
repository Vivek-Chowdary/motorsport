/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "ode/ode.h"
#include "world.hpp"
#include "area.hpp"
#include "SDL/SDL_keysym.h"

Part::Part (WorldObject * container, const std::string & name)
    :WorldObject(container, name)
{
    setPath(Paths::part(name));
    setXmlPath(Paths::partXml(name));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "parts." + name);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("parts." + name);
    XmlFile * xmlFile = new XmlFile (getXmlPath().c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}

Part::~Part ()
{
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
                log->__format (LOG_CCREATOR, "Found a part.");
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
                            log->__format (LOG_CCREATOR, "Found the name: %s", attribute.c_str());
                            setName(attribute);
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the description: %s", attribute.c_str());
                        }
                        if (attribute == "author")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the author: %s", attribute.c_str());
                        }
                        if (attribute == "contact")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the contact information: %s", attribute.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the license: %s", attribute.c_str());
                        }
                    }
                }
            }
        }
    }
    startGraphics(partNode);
    startPhysics(partNode);
    ogreObjects.begin()->second->setOdeReference(odeObjects.begin()->second);
}

void Part::startGraphics (DOMNode * n)
{
    OgreObjectData data;
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "mesh")
            {
                assignXmlString (data.meshPath, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the part graphics mesh filename: %s", data.meshPath.c_str());
            }
        }
    }
    data.meshPath = getPath() + data.meshPath;
    OgreObject * ogreObject = new OgreObject(this, data, getId());
    ogreObjects[getId()] = ogreObject;
}
void Part::startPhysics (DOMNode * n)
{
    OdeObjectData data;
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
            if (attribute == "mass")
            {
                assignXmlString (attribute, attNode->getValue());
                log->__format (LOG_CCREATOR, "Found the part physics mass: %s", attribute.c_str() );
                data.mass = stod (attribute);
            }
        }
    }
    std::string shape = "none";
    for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
    {
        if (n)
        {
            if (n->getNodeType () == DOMNode::ELEMENT_NODE)
            {
                std::string name;
                assignXmlString (name, n->getNodeName());
                if (name == "box")
                {
                    data.shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    data.dimensions = Vector3d (1, 1, 1);
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "dimensions")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part dimensions: %s", attribute.c_str() );
                            data.dimensions = Vector3d (attribute);
                        }
                    }
                }
                if (name == "sphere")
                {
                    data.shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    data.radius = 1;
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "radius")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part radius: %s", attribute.c_str() );
                            data.radius = stod (attribute);
                        }
                    }
                }
                if (name == "cappedCylinder")
                {
                    shape = name;
                    log->__format (LOG_CCREATOR, "Found the part physics shape: %s.", name.c_str());
                    data.radius = 1;
                    data.length = 1;
                    data.directionAxis = 3;
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "radius")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part radius: %s", attribute.c_str() );
                            data.radius = stod (attribute);
                        }
                        if (attribute == "length")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part length: %s", attribute.c_str() );
                            data.length = stod (attribute);
                        }
                        if (attribute == "directionAxis")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->__format (LOG_CCREATOR, "Found the part length: %s", attribute.c_str() );
                            if (attribute == "x") data.directionAxis = 1;
                            if (attribute == "y") data.directionAxis = 2;
                            if (attribute == "z") data.directionAxis = 3;
                        }
                    }
                }
            }
        }
    }
    if (data.shape == "none") log->__format(LOG_ERROR, "No physics shape specified for this part.");
    odeObjects[getId()] = new OdeObject(this, data, getId());
}

void Part::stepPhysics ()
{
    dBodyID partID = odeObjects.begin()->second->getBodyID();
    // //////////////simplified air friction (test)(should be forces!)
    dBodySetAngularVel (partID, (*(dReal *) (dBodyGetAngularVel (partID) + 0)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 1)) * (dReal) (0.999), (*(dReal *) (dBodyGetAngularVel (partID) + 2)) * (dReal) (0.999));
    // ////////////////////////////////////simplified air friction
    // applying user input [forces]
    if (this == World::getWorldPointer ()->areaList[0]->partList[0])
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
    log->__format(LOG_DEVELOPER, "part:x=%f y=%f z=%f", pos[0], pos[1], pos[2]);
}

